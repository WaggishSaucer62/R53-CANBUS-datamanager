// /* NOTE: DISPLAY AND TOUCH ORIGIN IS TOP LEFT CORNER
// This is the main file, mainly contains just basic logic, most actual code is within methods and functions.
// WaggishSaucer62, 27/11/25
// */

#include "functions.h"
#include "globals.h"

unsigned long lastDraw = 0;
unsigned long lastPowerCalc = 0;
unsigned long lastFuelCheck = 0;

bool sdInit = false;
bool canInit = false;
bool fuelWarned = false;

int autoLogCloseAfterTime;
int throttleBelowStartTime = 0;

bool hibernationStarted = false;


void setup(void) {
    Serial.begin(115200);
    // Disable TFT/Touch while MCP initializes to prevent clashes
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    pinMode(MCP_CS, OUTPUT);
    digitalWrite(MCP_CS, HIGH);
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
    pinMode(TOUCH_CS, OUTPUT);
    digitalWrite(TOUCH_CS, HIGH);

    SPI.begin(18, 19, 23);

    if (!SD.begin(SD_CS)) {
        sdInit = false;
    } else {
        sdInit = true;
    }

    if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
        canInit = true;
        CAN.setMode(MCP_NORMAL);
    } else {
        canInit = false;
    }

    // Now initialize TFT
    tft.init();
    tft.setRotation(3);
    touchCalibrate();
    tft.fillScreen(TFT_BLACK);

    FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.clear();


    // Initialise object parameters, these are global objects defined in globals.h/.cpp
    rpmDial.xPos = tft.width()/2;
    rpmDial.yPos = (tft.height()/2) + 20;
    rpmDial.radius = 80;
    rpmDial.lowerBound = 0;
    rpmDial.upperBound = 7000;
    rpmDial.val1 = 0;
    rpmDial.val2 = 0;

    fuelGauge.xPos = 10;
    fuelGauge.yPos = tft.height()-10-100-30;
    fuelGauge.width = 20;
    fuelGauge.height = 100;
    fuelGauge.percentage = 0;

    throttleGauge.xPos = tft.width()-10-20;
    throttleGauge.yPos = tft.height()-10-100-30;
    throttleGauge.width = 20;
    throttleGauge.height = 100;
    throttleGauge.percentage = 0;

    shiftDots.xPos = tft.width()/2;
    shiftDots.yPos = 20;
    shiftDots.spacing = 30;
    shiftDots.radius = 10;

    tempText.xPos = tft.width() - 20;
    tempText.yPos = tft.height() - 20;
    tempText.textSize = 3;

    coolantTemp.xPos = 50;
    coolantTemp.yPos = tft.height() - 20;
    coolantTemp.textSize = 3;

    powerText.xPos = (tft.width()/2)-70;
    powerText.yPos = 30;
    powerText.textSize = 4;

    powerPeakText.xPos = (tft.width()/2)+70;
    powerPeakText.yPos = 30;
    powerPeakText.textSize = 4;

    loggingToggle.xPos = 200;
    loggingToggle.yPos = 100;
    loggingToggle.radius = 20;
    loggingToggle.colourOn = TFT_GREEN;
    loggingToggle.colourOff = TFT_RED;
    loggingToggle.text = "Logging";
    loggingToggle.state = false;

    autoLoggingToggle.xPos = 200;
    autoLoggingToggle.yPos = 160;
    autoLoggingToggle.radius = 20;
    autoLoggingToggle.colourOn = TFT_GREEN;
    autoLoggingToggle.colourOff = TFT_RED;
    autoLoggingToggle.text = "Auto Logging";
    autoLoggingToggle.state = false;

    brightnessSlider.xPos = 30;
    brightnessSlider.yPos = 35;
    brightnessSlider.height = 170;
    brightnessSlider.width = 6;
    brightnessSlider.radius = 20;
    brightnessSlider.label = "Screen";

    LEDbrightnessSlider.xPos = 90;
    LEDbrightnessSlider.yPos = 35;
    LEDbrightnessSlider.height = 170;
    LEDbrightnessSlider.width = 6;
    LEDbrightnessSlider.radius = 20;
    LEDbrightnessSlider.label = "LEDs";

    // calibrateAndPrintTouchData(); // UNCOMMENT, COMMENT OUT LOOP TO CALIBRATE TOUCH, prints calibration data to serial


    // Load and apply config file settings, second value is default if key not found or SD fails
    config.load("/config.cfg");

    brightnessPercentage = config.get("screenBrightness", "100").toInt();
    analogWrite(22, map(brightnessPercentage, 0, 100, 0, 255));
    currentScreen = parseScreenID(config.get("screen", "MAIN_SCREEN"));

    LEDbrightnessPercentage = config.get("LEDsBrightness", "100").toInt();
    shiftDotsLED.flashSpeed = config.get("LEDsFlashingSpeed", "60").toInt();
    shiftDotsLED.flashingRPM = config.get("LEDsFlashingRPM", "123435").toInt();

    powerCalc.dragCoeff = config.get("dragCoeff", "0.39").toFloat();
    powerCalc.airDensity = config.get("airDensity", "1.225").toFloat();
    powerCalc.frontalArea = config.get("frontalArea", "1.98").toFloat();
    powerCalc.mass = config.get("mass", "1215.0").toFloat();

    logger.logIntervalMs = config.get("loggingRate", "500").toInt();

    autoLoggingThreshold = config.get("autoLoggingThreshold", "70").toInt();
    autoLoggingToggle.state = config.get("autoLoggingToggle", "0").toInt();
    autoLogger.loggingEnabled = config.get("autoLoggingToggle", "0").toInt();
    autoLogCloseAfterTime = config.get("autoLogCloseAfterTimeMS", "10000").toInt();
    autoLogger.logIntervalMs = config.get("loggingRate", "500").toInt();

    logger.data["rpm"] = &canBus.rpm;
    logger.data["speed"] = &canBus.spdAvg;
    logger.data["Throttle Position"] = &canBus.throttlePos;
    logger.data["fuelPercent"] = &canBus.fuelPercent;
    logger.data["externalTemp"] = &canBus.externalTemp;
    logger.data["power"] = &powerCalc.power;
    logger.data["acceleration"] = &powerCalc.acceleration;
    logger.data["fullWeight"] = &powerCalc.fullWeight;
    logger.data["coolantTemp"] = &canBus.engineTemp;

    autoLogger.data["rpm"] = &canBus.rpm;
    autoLogger.data["speed"] = &canBus.spdAvg;
    autoLogger.data["Throttle Position"] = &canBus.throttlePos;
    autoLogger.data["fuelPercent"] = &canBus.fuelPercent;
    autoLogger.data["externalTemp"] = &canBus.externalTemp;
    autoLogger.data["power"] = &powerCalc.power;
    autoLogger.data["acceleration"] = &powerCalc.acceleration;
    autoLogger.data["fullWeight"] = &powerCalc.fullWeight;
    logger.data["coolantTemp"] = &canBus.engineTemp;

    fuelWarningLevel = config.get("fuelWarningLevel", "5").toInt();


    int presetCenterOffset = 50;
    int leftPresetCenterOffset = 50;

    accelStartSpeedUp.xPos = 20;
    accelStartSpeedUp.yPos = 100;
    accelStartSpeedUp.radius = 13;
    accelStartSpeedUp.colourOn = TFT_GREEN;
    accelStartSpeedUp.colourOff = TFT_GREEN;
    accelStartSpeedUp.text = "^";
    accelStartSpeedUp.state = false;

    accelStartSpeedDown.xPos = 20;
    accelStartSpeedDown.yPos = 140;
    accelStartSpeedDown.radius = 13;
    accelStartSpeedDown.colourOn = TFT_RED;
    accelStartSpeedDown.colourOff = TFT_RED;
    accelStartSpeedDown.text = "v";
    accelStartSpeedDown.state = false;


    accelStartSpeedPreset1.xPos = tft.width()/2 - presetCenterOffset - leftPresetCenterOffset;
    accelStartSpeedPreset1.yPos = 160;
    accelStartSpeedPreset1.radius = 20;
    accelStartSpeedPreset1.colourOn = TFT_BLUE;
    accelStartSpeedPreset1.colourOff = TFT_BLUE;
    accelStartSpeedPreset1.text = "0";
    accelStartSpeedPreset1.state = false;

    accelStartSpeedPreset2.xPos = tft.width()/2 - presetCenterOffset - leftPresetCenterOffset + 50;
    accelStartSpeedPreset2.yPos = 160;
    accelStartSpeedPreset2.radius = 20;
    accelStartSpeedPreset2.colourOn = TFT_BLUE;
    accelStartSpeedPreset2.colourOff = TFT_BLUE;
    accelStartSpeedPreset2.text = "3";
    accelStartSpeedPreset2.state = false;

    accelStartSpeedPreset3.xPos = tft.width()/2 - presetCenterOffset - leftPresetCenterOffset;
    accelStartSpeedPreset3.yPos = 210;
    accelStartSpeedPreset3.radius = 20;
    accelStartSpeedPreset3.colourOn = TFT_BLUE;
    accelStartSpeedPreset3.colourOff = TFT_BLUE;
    accelStartSpeedPreset3.text = "5";
    accelStartSpeedPreset3.state = true;

    accelStartSpeedPreset4.xPos = tft.width()/2 - presetCenterOffset - leftPresetCenterOffset + 50;
    accelStartSpeedPreset4.yPos = 210;
    accelStartSpeedPreset4.radius = 20;
    accelStartSpeedPreset4.colourOn = TFT_BLUE;
    accelStartSpeedPreset4.colourOff = TFT_BLUE;
    accelStartSpeedPreset4.text = "10";
    accelStartSpeedPreset4.state = false;



    accelEndSpeedUp.xPos = (tft.width()) - 150;
    accelEndSpeedUp.yPos = 100;
    accelEndSpeedUp.radius = 13;
    accelEndSpeedUp.colourOn = TFT_GREEN;
    accelEndSpeedUp.colourOff = TFT_GREEN;
    accelEndSpeedUp.text = "^";
    accelEndSpeedUp.state = false;

    accelEndSpeedDown.xPos = (tft.width()) - 150;
    accelEndSpeedDown.yPos = 140;
    accelEndSpeedDown.radius = 13;
    accelEndSpeedDown.colourOn = TFT_RED;
    accelEndSpeedDown.colourOff = TFT_RED;
    accelEndSpeedDown.text = "v";
    accelEndSpeedDown.state = false;


    accelEndSpeedPreset1.xPos = tft.width()/2 + presetCenterOffset;
    accelEndSpeedPreset1.yPos = 160;
    accelEndSpeedPreset1.radius = 20;
    accelEndSpeedPreset1.colourOn = TFT_BLUE;
    accelEndSpeedPreset1.colourOff = TFT_BLUE;
    accelEndSpeedPreset1.text = "60";
    accelEndSpeedPreset1.state = false;

    accelEndSpeedPreset2.xPos = tft.width()/2 + presetCenterOffset + 50;
    accelEndSpeedPreset2.yPos = 160;
    accelEndSpeedPreset2.radius = 20;
    accelEndSpeedPreset2.colourOn = TFT_BLUE;
    accelEndSpeedPreset2.colourOff = TFT_BLUE;
    accelEndSpeedPreset2.text = "100";
    accelEndSpeedPreset2.state = false;

    accelEndSpeedPreset3.xPos = tft.width()/2 + presetCenterOffset;
    accelEndSpeedPreset3.yPos = 210;
    accelEndSpeedPreset3.radius = 20;
    accelEndSpeedPreset3.colourOn = TFT_BLUE;
    accelEndSpeedPreset3.colourOff = TFT_BLUE;
    accelEndSpeedPreset3.text = "120";
    accelEndSpeedPreset3.state = true;

    accelEndSpeedPreset4.xPos = tft.width()/2 + presetCenterOffset + 50;
    accelEndSpeedPreset4.yPos = 210;
    accelEndSpeedPreset4.radius = 20;
    accelEndSpeedPreset4.colourOn = TFT_BLUE;
    accelEndSpeedPreset4.colourOff = TFT_BLUE;
    accelEndSpeedPreset4.text = "150";
    accelEndSpeedPreset4.state = false;



    accelStartSpeedText.xPos = (tft.width()/2)-70;
    accelStartSpeedText.yPos = (tft.height()/2)-10;
    accelStartSpeedText.textSize = 4;
    accelStartSpeedText.lastVal = "5";

    accelEndSpeedText.xPos = (tft.width()/2)+70;
    accelEndSpeedText.yPos = (tft.height()/2)-10;
    accelEndSpeedText.textSize = 4;
    accelEndSpeedText.lastVal = "100";

    accelStartText.xPos = (tft.width()/2)-100+10;
    accelStartText.yPos = (tft.height()/2)-100;
    accelStartText.textSize = 2;
    accelStartText.lastVal = "Start Speed";

    accelEndText.xPos = (tft.width()/2)+100;
    accelEndText.yPos = (tft.height()/2)-100;
    accelEndText.textSize = 2;
    accelEndText.lastVal = "End Speed";

    accelTime.xPos = tft.width()/2;
    accelTime.yPos = (tft.height()/2)-60;
    accelTime.textSize = 4;
    accelTime.lastVal = "0.00s";



    if (sdInit == false) {
        fullscreenWarning("SD INIT FAIL");
        delay(5000);
    }
    if (canInit == false) {
        fullscreenWarning("CAN INIT FAIL");
        delay(5000);
    }

    // Initialise the screen as per the loaded config
    switch(currentScreen) {
    case MAIN_SCREEN:
        mainScreenInit();
        break;
    case SETTINGS_SCREEN:
        settingsScreenInit();
        break;
    case POWER_SCREEN:
        powerScreenInit();
        break;
    case ACCEL_SCREEN:
        accelerationTestScreenInit();
        break;
    }
}


void loop() {
    if (hibernate == true) {
        canBus.update();
        if (hibernationStarted == false) {
            if (logger.loggingActive == true) {
                logger.close();
            }
            if (autoLogger.loggingActive == true) {
                autoLogger.close();
            }
            hibernationStarted = true;
        }
    }

    else {
        canBus.update();
        if (logger.loggingEnabled == true) {
            logger.log();
        }
        if (autoLogger.loggingEnabled == true) {
            if (canBus.throttlePos >= autoLoggingThreshold) {
                throttleBelowStartTime = 0;
                if (autoLogger.loggingActive == false) {
                    autoLogger.init();
                }
                autoLogger.log();
            }

            if (autoLogger.loggingActive == true) {
                if (canBus.throttlePos < autoLoggingThreshold) { // Close log after configurable amount of seconds under throttle threshold.
                    autoLogger.log(); // Ensures also logging when under the threshold, a little ugly but it works and I'm too lazy to change this...
                    if (throttleBelowStartTime == 0) {
                        throttleBelowStartTime = millis();
                    }
                    if (millis() - throttleBelowStartTime >= autoLogCloseAfterTime) {
                        autoLogger.close();
                        tft.fillScreen(TFT_BLACK);
                        tft.setTextColor(TFT_WHITE, TFT_BLACK);
                        tft.setTextSize(2);
                        tft.setTextDatum(MC_DATUM);
                        tft.drawString("Log saved to " + autoLogger.fileName, tft.width()/2, tft.height()/2);
                        stopDrawForMillis = millis() + 2000;
                        reinit = true;
                    }
                }
            }
        }
        
        

        if (millis() > stopDrawForMillis) { // Only draw when it hasn't been disabled for a time by another function (used by warnings and info screens)
            if (millis() - lastDraw > 20) { // Only draw and check touch at 50fps
                if (reinit == true) {
                    switch (currentScreen) {
                        case MAIN_SCREEN:
                            mainScreenInit();
                            break;
                        case SETTINGS_SCREEN:
                            settingsScreenInit();
                            break;
                        case POWER_SCREEN:
                            powerScreenInit();
                            break;
                        case ACCEL_SCREEN:
                            accelerationTestScreenInit();
                            break;
                    }
                    reinit = false;
                }

                pressed = tft.getTouch(&xTouch, &yTouch);
                checkScreenSwitch(xTouch, yTouch); // Checks if screen should be switched
                
                shiftDotsLED.update(canBus.rpm); // Update LEDs every frame irrespective of screen
                hpMax.add(millis(), powerCalc.power); // Update hpMax every frame from any screen
                switch(currentScreen) {
                case MAIN_SCREEN:
                    mainScreen();
                    break;
                case SETTINGS_SCREEN:
                    settingsScreen();
                    break;
                case POWER_SCREEN:
                    powerScreen();
                    break;
                case ACCEL_SCREEN:
                    accelerationTestScreen();
                    break;
                }
                lastDraw = millis();
            }
        }
        

        if (millis() - lastPowerCalc >= logger.logIntervalMs) {
            powerCalc.calculatePower();
            lastPowerCalc = millis();
        }

        if (fuelWarned == false && (millis() - lastFuelCheck >= 1000)) {
            if (canBus.fuelPercent <= fuelWarningLevel) {
                fullscreenWarning("CHECK FUEL");
                stopDrawForMillis = millis() + 2000;
                reinit = true;
                fuelWarned = true;
            }
            lastFuelCheck = millis();
        }
    }
    
}
