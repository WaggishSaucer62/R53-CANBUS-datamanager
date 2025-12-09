// /* NOTE: DISPLAY AND TOUCH ORIGIN IS TOP LEFT CORNER
// This is the main file, mainly contains just basic logic, most actual code is within methods and functions.
// WaggishSaucer62, 27/11/25
// */

#include "functions.h"
#include "globals.h"

unsigned long lastDraw = 0;
unsigned long lastPowerCalc = 0;
unsigned long lastFuelCheck = 0;


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
        Serial.println("SD FAIL");
    }

    if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
        Serial.println("MCP2515 Init OK!");
        CAN.setMode(MCP_NORMAL);
    } else {
        Serial.println("MCP2515 Init FAIL!");
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
    fuelGauge.yPos = tft.height()-10-100;
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

    loggingToggle.xPos = 200;
    loggingToggle.yPos = 100;
    loggingToggle.radius = 20;
    loggingToggle.colourOn = TFT_GREEN;
    loggingToggle.colourOff = TFT_RED;
    loggingToggle.text = "Logging";
    loggingToggle.state = false;

    brightnessSlider.xPos = 30;
    brightnessSlider.yPos = 30;
    brightnessSlider.height = 180;
    brightnessSlider.width = 6;
    brightnessSlider.radius = 20;

    LEDbrightnessSlider.xPos = 90;
    LEDbrightnessSlider.yPos = 30;
    LEDbrightnessSlider.height = 180;
    LEDbrightnessSlider.width = 6;
    LEDbrightnessSlider.radius = 20;

    // calibrateAndPrintTouchData(); // UNCOMMENT, COMMENT OUT LOOP TO CALIBRATE TOUCH, prints calibration data to serial


    // Load and apply config file settings, second value is default if key not found
    config.load("/config.cfg");

    brightnessPercentage = config.get("screenBrightness", "100").toInt();
    analogWrite(22, map(brightnessPercentage, 0, 100, 0, 255));
    currentScreen = parseScreenID(config.get("screen", "MAIN_SCREEN"));

    LEDbrightnessPercentage = config.get("LEDsBrightness", "100").toInt();
    shiftDotsLED.flashSpeed = config.get("LEDsFlashingSpeed", "60").toInt();
    shiftDotsLED.flashingRPM = config.get("LEDsFlashingRPM", "6000").toInt();

    powerCalc.dragCoeff = config.get("dragCoeff", "0.39").toFloat();
    powerCalc.airDensity = config.get("airDensity", "1.225").toFloat();
    powerCalc.frontalArea = config.get("frontalArea", "1.98").toFloat();
    powerCalc.mass = config.get("mass", "1215.0").toFloat();

    logger.logIntervalMs = config.get("loggingRate", "500").toInt();

    logger.data["rpm"] = &canBus.rpm;
    logger.data["speed"] = &canBus.spdAvg;
    logger.data["throttlePos"] = &canBus.throttlePos;
    logger.data["fuelPercent"] = &canBus.fuelPercent;
    logger.data["externalTemp"] = &canBus.externalTemp;
    logger.data["power"] = &powerCalc.smoothedPower;
    logger.data["acceleration"] = &powerCalc.acceleration;
    logger.data["fullWeight"] = &powerCalc.fullWeight;

    fuelWarningLevel = config.get("fuelWarningLevel", "5").toInt();

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
    }
}


void loop() {
    canBus.update();
    logger.log();

    if (millis() - lastDraw > 20) { // Only draw and check touch at 50fps
        pressed = tft.getTouch(&xTouch, &yTouch);
        checkScreenSwitch(xTouch, yTouch); // Checks if screen should be switched
        
        shiftDotsLED.update(canBus.rpm); // Update LEDs every frame irrespective of screen
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
        }
        lastDraw = millis();
    }

    if (millis() - lastPowerCalc >= logger.logIntervalMs) {
        powerCalc.calculatePower();
        lastPowerCalc = millis();
    }

    if (millis() - lastFuelCheck > 5000) {
        if (canBus.fuelPercent <= fuelWarningLevel) {
            fullscreenWarning("CHECK FUEL");
        }
        lastFuelCheck = millis();
    }
}