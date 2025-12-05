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
    rpmDial.init();
    fuelGauge.init();
    tempText.init();
    throttleGauge.init();
}

void settingsScreenInit() {
    brightnessSlider.init(brightnessPercentage);
    LEDbrightnessSlider.init(LEDbrightnessPercentage);
    loggingToggle.init(config.get("loggingToggle", "0").toInt());
}

void powerScreenInit() {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Power:  0", 10, 10);
    tft.drawString("Acceleration 0", 10, 40);
    tft.drawString("Weight: 0", 10, 70);
    tft.drawString("Drag: 0", 10, 100);
    tft.drawString("Speed: 0", 10, 130);
    }



void mainScreen() {
    rpmDial.update(canBus.rpm, canBus.spdAvg);
    fuelGauge.update(canBus.fuelPercent);
    throttleGauge.update(canBus.throttlePos);
    shiftDots.update(canBus.rpm);
    tempText.update(canBus.externalTemp);
}


void powerScreen() {
    static float powerInformation[4] = {0,0,0,0}; // prevSpeed, prevTime, currentSpeed, currentTime
    static uint32_t lastCalc = 0;
    static uint32_t lastUpdatedGraph = 0;
    static float acceleration = 0;
    static float lastPower = 0;
    static float smoothedPower = 0;
    const float alpha = 0.05;    // 0–1, lower is smoother

    if (millis() - lastCalc > 50) { // Every 50ms, calculate power and update related display items
        powerInformation[0] = powerInformation[2];
        powerInformation[1] = powerInformation[3];
        powerInformation[2] = canBus.spdAvg;
        powerInformation[3] = millis();

        float dragForce = 0.5*dragCoeff*frontalArea*airDensity*((powerInformation[2]/3.6)*(powerInformation[2]/3.6));
        float fullWeight = mass + ((50*0.75*canBus.fuelPercent)/100);
        float dt = (powerInformation[3] - powerInformation[1])/1000.0;
        if ((powerInformation[2] - powerInformation[0] != 0) && dt > 0.01f) {
            acceleration = ((powerInformation[2]-powerInformation[0])/3.6)/dt;
        }
        float power = (((fullWeight*acceleration)+dragForce)*(powerInformation[2]/3.6))/745.7;
        smoothedPower = smoothedPower + alpha * (power - smoothedPower);

        if (abs(smoothedPower - lastPower) > 0.1) { // CORRECT THIS - THIS IS A BUNCH OF PRINTS FOR DEBUGGING
            tft.fillRect(0, 0, 320, 145, TFT_BLACK);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextSize(3);
            tft.setTextDatum(TL_DATUM);
            tft.drawString("Power: " + String(smoothedPower, 1), 10, 10);
            tft.drawString("Acceleration: " + String(acceleration, 1), 10, 40);
            tft.drawString("Weight: " + String(fullWeight, 1), 10, 70);
            tft.drawString("Drag: " + String(dragForce, 1), 10, 100);
            tft.drawString("Speed: " + String(powerInformation[2], 1), 10, 130);

            lastPower = smoothedPower;
        }
        lastCalc = millis();
    }

    if (millis()-lastUpdatedGraph > 30) { // Still update graph separately, to keep control of scanrate
        powerGraph.update(smoothedPower);
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

        if (lastPressedState == false) {
            loggingToggle.checkIfPressed(xTouch, yTouch);
            bool state = loggingToggle.getState();
            config.set("loggingToggle", String(state));
            config.save("/config.cfg");
        }
        lastPressedState = true;
    }
}