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
    tempText.init();
    throttleGauge.init();
}

void settingsScreenInit() {
    tft.fillScreen(TFT_BLACK);
    brightnessSlider.init(brightnessPercentage);
    LEDbrightnessSlider.init(LEDbrightnessPercentage);
    loggingToggle.init();
}

void powerScreenInit() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Power:  0", 10, 10);
    tft.drawString("Acceleration 0", 10, 40);
    tft.drawString("Weight: 0", 10, 70);
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
    static float lastPower = 0;

    if (millis() - lastCalc > 50) { // Every 50ms, calculate power and update related display items
        if (abs(powerCalc.smoothedPower - lastPower) > 0.1) { // CORRECT THIS - THIS IS A BUNCH OF PRINTS FOR DEBUGGING
            tft.fillRect(0, 0, 320, 145, TFT_BLACK);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextSize(3);
            tft.setTextDatum(TL_DATUM);
            tft.drawString("Power: " + String(powerCalc.smoothedPower, 1), 10, 10);
            tft.drawString("Acceleration: " + String(powerCalc.acceleration, 1), 10, 40);
            tft.drawString("Weight: " + String(powerCalc.fullWeight, 1), 10, 70);

            lastPower = powerCalc.smoothedPower;
        }
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

        if (lastPressedState == false) { // button only updates once per press.
                if (loggingToggle.checkIfPressed(xTouch, yTouch)) {
                bool state = loggingToggle.state;
                logger.loggingActive = state;
                if (state == true) {
                    logger.init();
                } else {
                    logger.close();
                    tft.fillScreen(TFT_BLACK);
                    tft.setTextColor(TFT_WHITE, TFT_BLACK);
                    tft.setTextSize(2);
                    tft.setTextDatum(MC_DATUM);
                    tft.drawString("Log saved to " + logger.fileName, tft.width()/2, tft.height()/2);
                    delay(2000);

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
                    }
                }
            }
        }
        lastPressedState = true;
    }
}