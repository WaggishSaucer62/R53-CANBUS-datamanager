/*
File for functions related to the screens, mainly including initialisation functions to be run when switching screens, and main functions to be run every frame.
WaggishSaucer62, 27/11/25
*/

#include "functions.h"
#include "globals.h"


screenID parseScreenID(const String &name) {
    if (name == "0") return MAIN_SCREEN; // The enum class actually converts them to ints, starting from 0, so in the config it is stored as such.
    if (name == "1") return SETTINGS_SCREEN;
    if (name == "2") return POWER_SCREEN;
}


void checkScreenSwitch(uint16_t xTouch, uint16_t yTouch) {
    static unsigned long lastTouchRead = 0;
    static bool touchLastState = false;

    if (millis() - lastTouchRead < 50) { // Only check every 50ms to reduce load
        return;
    }
    lastTouchRead = millis();
    if (touchLastState == false && pressed == true) {
      if (xTouch >= 290) { // If right side of screen is touched
        tft.fillScreen(TFT_BLACK);
        switch(currentScreen) {
            case MAIN_SCREEN:
                settingsScreenInit();
                currentScreen = SETTINGS_SCREEN;
                break;
            case SETTINGS_SCREEN:
                powerScreenInit();
                currentScreen = POWER_SCREEN;
                break;
            case POWER_SCREEN:
                accelerationTestScreenInit();
                currentScreen = ACCEL_SCREEN;
                break;
            case ACCEL_SCREEN:
                mainScreenInit();
                currentScreen = MAIN_SCREEN;
                break;
        }
        config.set("screen", String(currentScreen));
        config.save("/config.cfg");
      }
    }
    touchLastState = pressed;
}


void mainScreenInit() {
    tft.fillScreen(TFT_BLACK);
    rpmDial.init();
    fuelGauge.init();
    tempText.init(canBus.externalTemp);
    coolantTemp.init(canBus.engineTemp);
    throttleGauge.init();
}

void settingsScreenInit() {
    tft.fillScreen(TFT_BLACK);
    brightnessSlider.init(brightnessPercentage);
    LEDbrightnessSlider.init(LEDbrightnessPercentage);
    loggingToggle.init();
    autoLoggingToggle.init();
}

void powerScreenInit() {
    tft.fillScreen(TFT_BLACK);
    powerText.init("0");
    powerPeakText.init("0");
    tft.drawString("HP", powerText.xPos, powerText.yPos+40);
    tft.drawString("Peak", powerPeakText.xPos, powerPeakText.yPos+40);
    powerGraph.init();
}

void accelerationTestScreenInit() {
    tft.fillScreen(TFT_BLACK);

    accelStartSpeedUp.init();
    accelStartSpeedDown.init();
    accelStartSpeedPreset1.init();
    accelStartSpeedPreset2.init();
    accelStartSpeedPreset3.init();
    accelStartSpeedPreset4.init();

    accelEndSpeedUp.init();
    accelEndSpeedDown.init();
    accelEndSpeedPreset1.init();
    accelEndSpeedPreset2.init();
    accelEndSpeedPreset3.init();
    accelEndSpeedPreset4.init();

    accelStartSpeedText.init(accelStartSpeedText.lastVal);
    accelEndSpeedText.init(accelEndSpeedText.lastVal);
    accelStartText.init(accelStartText.lastVal);
    accelEndText.init(accelEndText.lastVal);
    accelTime.init(accelTime.lastVal);
}



void mainScreen() {
    rpmDial.update(canBus.rpm, canBus.spdAvg);
    fuelGauge.update(canBus.fuelPercent);
    throttleGauge.update(canBus.throttlePos);
    shiftDots.update(canBus.rpm);
    tempText.update(canBus.externalTemp);
    coolantTemp.update(canBus.engineTemp);
}


void powerScreen() {
    static uint32_t lastCalc = 0;
    static uint32_t lastUpdatedGraph = 0;

    if (millis() - lastCalc > 50) { // Every 50ms, calculate power and update related display items
        float currentPower = powerCalc.power;

        powerPeakText.update(String(hpMax.max(), 1));
        powerText.update(String(currentPower, 1));

        lastCalc = millis();
    }

    if (millis()-lastUpdatedGraph > 30) { // Still update graph separately, to keep control of scanrate
        powerGraph.update(powerCalc.power);
        lastUpdatedGraph = millis();
    }
}


void settingsScreen() {
    if (pressed == false) {
        lastPressedState = false;
    }

    if (pressed == true) { // Slider can update as frequently as possible, but the button only triggers once per press.
        if ((xTouch >= (brightnessSlider.xPos-20)) && (xTouch < (brightnessSlider.xPos+20))) {
            if (yTouch <= brightnessSlider.yPos) {
                brightnessPercentage = 100;
            } else if (yTouch >= (brightnessSlider.yPos+brightnessSlider.height)) {
                brightnessPercentage = 0;
            } else {
                brightnessPercentage = 100 - ((yTouch - brightnessSlider.yPos) * 100) / brightnessSlider.height;
            }
            brightnessSlider.update(brightnessPercentage);
            analogWrite(22, map(brightnessPercentage, 0, 100, 0, 255));
            config.set("screenBrightness", String(brightnessPercentage));
            config.save("/config.cfg");
        }

        if ((xTouch >= (LEDbrightnessSlider.xPos-20)) && (xTouch < (LEDbrightnessSlider.xPos+20))) {
            if (yTouch <= LEDbrightnessSlider.yPos) {
                LEDbrightnessPercentage = 100;
            } else if (yTouch >= (LEDbrightnessSlider.yPos+LEDbrightnessSlider.height)) {
                LEDbrightnessPercentage = 0;
            } else {
                LEDbrightnessPercentage = 100 - ((yTouch - LEDbrightnessSlider.yPos) * 100) / LEDbrightnessSlider.height;
            }
            LEDbrightnessSlider.update(LEDbrightnessPercentage);
            config.set("LEDsBrightness", String(LEDbrightnessPercentage));
            config.save("/config.cfg");
        }

        if (lastPressedState == false) { // buttons only updates once per press.
            if (loggingToggle.checkIfPressed(xTouch, yTouch)) {
                bool state = loggingToggle.state;
                logger.loggingEnabled = state;
                if (state == true) {
                    logger.init();
                } else {
                    logger.close();
                    tft.fillScreen(TFT_BLACK);
                    tft.setTextColor(TFT_WHITE, TFT_BLACK);
                    tft.setTextSize(2);
                    tft.setTextDatum(MC_DATUM);
                    tft.drawString("Log saved to " + logger.fileName, tft.width()/2, tft.height()/2);
                    stopDrawForMillis = millis() + 2000;
                    reinit = true;
                }
            }

            if (autoLoggingToggle.checkIfPressed(xTouch, yTouch)) { // This checks if pressed, automatically toggles the state if pressed, and returns whether or not it was pressed (bool).
                bool state = autoLoggingToggle.state;
                autoLogger.loggingEnabled = state;
                if (state == true) {
                    config.set("autoLoggingToggle", "1");
                } else {
                    config.set("autoLoggingToggle", "0");
                }
                config.save("/config.cfg");
            }
        }
        lastPressedState = true;
    }
}


void accelerationTestScreen() {
    static bool accelStarted = false;
    static bool runCompleted = false;
    static unsigned long accelStartTime = 0;
    static unsigned long accelEndTime = 0;
    static unsigned long lastPressTime = 0;

    if (pressed == false) {
        if (canBus.spdAvg < accelStartSpeedText.lastVal.toInt()) {
            accelStarted = false;
            runCompleted = false;
        }

        if ((canBus.spdAvg > accelStartSpeedText.lastVal.toInt()) && (canBus.spdAvg < accelEndSpeedText.lastVal.toInt()) && (runCompleted == false)) {
            if (accelStarted == false) {
                accelStarted = true;
                accelStartTime = millis();
            } else if (accelStarted == true) {
                accelTime.update(String((millis() - accelStartTime) / 1000.0, 2) + "s");
            }
        }

        if ((canBus.spdAvg >= accelEndSpeedText.lastVal.toInt()) && (accelStarted == true)) {
            accelStarted = false;
            runCompleted = true;
            accelEndTime = millis();
            accelTime.update(String((accelEndTime - accelStartTime) / 1000.0, 2) + "s");
        }
    }

    if ((millis() - lastPressTime > 100) && pressed == true) {
        lastPressTime = millis();
        if (xTouch < tft.width()/2) { // Splits screen in half for quicker checks.
            if (accelStartSpeedUp.checkIfPressed(xTouch, yTouch)) {
                accelStartSpeedText.update(String(accelStartSpeedText.lastVal.toInt() + 1));
            } else if (accelStartSpeedDown.checkIfPressed(xTouch, yTouch)) {
                accelStartSpeedText.update(String(accelStartSpeedText.lastVal.toInt() - 1));
            } else if (accelStartSpeedPreset1.checkIfPressed(xTouch, yTouch)) {
                accelStartSpeedText.update(String(accelStartSpeedPreset1.text.toInt()));
            } else if (accelStartSpeedPreset2.checkIfPressed(xTouch, yTouch)) {
                accelStartSpeedText.update(String(accelStartSpeedPreset2.text.toInt()));
            } else if (accelStartSpeedPreset3.checkIfPressed(xTouch, yTouch)) {
                accelStartSpeedText.update(String(accelStartSpeedPreset3.text.toInt()));
            } else if (accelStartSpeedPreset4.checkIfPressed(xTouch, yTouch)) {
                accelStartSpeedText.update(String(accelStartSpeedPreset4.text.toInt()));
            }
        } else if (accelEndSpeedUp.checkIfPressed(xTouch, yTouch)) {
            accelEndSpeedText.update(String(accelEndSpeedText.lastVal.toInt() + 1));
        } else if (accelEndSpeedDown.checkIfPressed(xTouch, yTouch)) {
            accelEndSpeedText.update(String(accelEndSpeedText.lastVal.toInt() - 1));
        } else if (accelEndSpeedPreset1.checkIfPressed(xTouch, yTouch)) {
            accelEndSpeedText.update(String(accelEndSpeedPreset1.text.toInt()));
        } else if (accelEndSpeedPreset2.checkIfPressed(xTouch, yTouch)) {
            accelEndSpeedText.update(String(accelEndSpeedPreset2.text.toInt()));
        } else if (accelEndSpeedPreset3.checkIfPressed(xTouch, yTouch)) {
            accelEndSpeedText.update(String(accelEndSpeedPreset3.text.toInt()));
        } else if (accelEndSpeedPreset4.checkIfPressed(xTouch, yTouch)) {
            accelEndSpeedText.update(String(accelEndSpeedPreset4.text.toInt()));
        }
    }
}