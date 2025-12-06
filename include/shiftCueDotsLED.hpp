/*
File for shiftCueDotsLED class, contains all code relevant to updating and drawing them, based on non-led version.
WaggishSaucer62, 03/12/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class shiftCueDotsLED {
    private:
        const int thresholds[3] = {3000, 4000, 5000};
        bool shiftDots[3] = {false,false,false};
        bool previousDotsState[3] = {false,false,false};
        int colors[3][3] = {{0, 255, 0}, {255, 255, 0}, {255, 0, 0}};
        bool flashState = false;
        bool flashing = false;
        int lastToggleMillis = 0;

    public:
        int flashingRPM = 6000;
        int flashSpeed = 60;
        
        void update(int rpm) {
            if (rpm >= flashingRPM && flashing == false) {
                flashing = true;
                for (int i = 0; i < NUM_LEDS; i++) {
                    leds[i].setRGB(0, 0, 0);
                }
                FastLED.show();
                shiftDots[0]=false;
                shiftDots[1]=false;
                shiftDots[2]=false;
            } else if (rpm < flashingRPM && flashing == true) {
                flashing = false;
                for (int i = 0; i < NUM_LEDS; i++) {
                    leds[i].setRGB(0, 0, 0);
                }
                FastLED.show();
                shiftDots[0]=false;
                shiftDots[1]=false;
                shiftDots[2]=false;
            }

            if (flashing == true) {
                if (millis() - lastToggleMillis >= flashSpeed) {
                    lastToggleMillis = millis();
                    for (int i = 0; i < NUM_LEDS; i++) {
                        if (flashState == false) {
                            leds[i].setRGB(0, 0, 0);
                        } else {
                            leds[i].setRGB(round((255*LEDbrightnessPercentage)/100), 0, 0);
                        }
                    }
                    flashState = !flashState;
                    FastLED.show();
                }
                
            } else {
                for (int i = 0; i < 3; i++) { // Updates each dot state based on relevant thresholds
                    shiftDots[i] = (rpm >= thresholds[i]);
                }

                for (int i = 0; i < 3; i++) {
                    if (shiftDots[i] != previousDotsState[i]) {
                        if (shiftDots[i] == false) {
                            leds[i].setRGB(0, 0, 0);
                            leds[NUM_LEDS-1-i].setRGB(0, 0, 0);
                        } else {
                            leds[i].setRGB(round((colors[i][0]*LEDbrightnessPercentage)/100), round((colors[i][1]*LEDbrightnessPercentage)/100), round((colors[i][2]*LEDbrightnessPercentage)/100));
                            leds[NUM_LEDS-1-i].setRGB(round((colors[i][0]*LEDbrightnessPercentage)/100), round((colors[i][1]*LEDbrightnessPercentage)/100), round((colors[i][2]*LEDbrightnessPercentage)/100));
                        }
                        FastLED.show();
                    }
                }
                for (int i = 0; i < 3; i++) {
                    previousDotsState[i] = shiftDots[i];
                }
            }
        }
};