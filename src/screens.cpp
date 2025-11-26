/*
File for functions related to the screens, mainly including initialisation functions to be run when switching screens, and main functions to be run every frame.
WaggishSaucer62, 27/11/25
*/

#include "functions.h"
#include "globals.h"


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
    loggingToggle.init();
}

void powerScreenInit() {
    // Empty for now
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

    if (millis() - lastCalc < 50) { // Every 50ms, calculate power and update related display items
        float dragForce = 0.5*dragCoeff*frontalArea*airDensity*((powerInformation[2]/3.6)*(powerInformation[2]/3.6));
        float fullWeight = mass + ((50*0.75*canBus.fuelPercent)/100);
        float dt = (powerInformation[3] - powerInformation[1])/1000.0;
        if ((powerInformation[2] - powerInformation[0] != 0) && dt > 0.0001f) {
            acceleration = ((powerInformation[2]-powerInformation[0])/3.6)/dt;
        }
        float power = (((fullWeight*acceleration)+dragForce)*(powerInformation[2]/3.6))/745.7;
        smoothedPower = smoothedPower + alpha * (power - smoothedPower);

        if (abs(smoothedPower - lastPower) > 0.1) {
        int textW = 250;
        int textH = 60;
        int x = tft.width() / 2;
        int y = 100;

        tft.fillRect(x - textW/2, y - textH/2, textW, textH, TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(6);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(String(smoothedPower, 1), x, y);

        lastPower = smoothedPower;
        powerInformation[0] = powerInformation[2];
        powerInformation[1] = powerInformation[3];
        powerInformation[2] = canBus.spdAvg;
        powerInformation[3] = millis();
        }
    }
    lastCalc = millis();

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
        if (yTouch < brightnessSlider.yPos) {
            brightnessPercentage = 0;
        } else if (yTouch > (brightnessSlider.yPos+brightnessSlider.height)) {
            brightnessPercentage = 100;
        } else {
        brightnessPercentage = ((yTouch-brightnessSlider.yPos)*100)/brightnessSlider.height;
        }
        brightnessSlider.update(brightnessPercentage);
        analogWrite(22, map(brightnessPercentage, 100, 0, 0, 255));
        }

        if (lastPressedState == false) {
            loggingToggle.checkIfPressed(xTouch, yTouch);
        }
        lastPressedState = true;
    }
};