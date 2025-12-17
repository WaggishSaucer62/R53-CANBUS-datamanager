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



void mainScreen() {
    rpmDial.update(canBus.rpm, canBus.spdAvg);
    fuelGauge.update(canBus.fuelPercent);
    throttleGauge.update(canBus.throttlePos);
    shiftDots.update(canBus.rpm);
    tempText.update(canBus.externalTemp);
}


void powerScreen() {
    static uint32_t lastCalc = 0;
    static uint32_t lastUpdatedGraph = 0;

    if (millis() - lastCalc > 50) { // Every 50ms, calculate power and update related display items
        float currentPower = powerCalc.smoothedPower;

        hpMax.add(millis(), currentPower);
        powerPeakText.update(String(hpMax.max(), 1));
        powerText.update(String(currentPower, 1));

        lastCalc = millis();
    }

    if (millis()-lastUpdatedGraph > 30) { // Still update graph separately, to keep control of scanrate
        powerGraph.update(powerCalc.smoothedPower);
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