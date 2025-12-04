/*
File for shiftCueDotsLED class, contains all code relevant to updating and drawing them, based on non-led version.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class shiftCueDotsLED {
    private:
        const int thresholds[3] = {3000, 4000, 5000};
        bool shiftDots[3] = {false,false,false};
        bool previousDotsState[3] = {false,false,false};
        int colors[3][3] = {{0, 255, 0}, {255, 255, 0}, {255, 0, 0}}; // G, Y, R
        bool flashState = false;
        bool flashing = false;
        int lastToggleMillis = 0;

    public:
        int flashingRPM;
        
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
                if (millis() - lastToggleMillis >= 60) {
                    lastToggleMillis = millis();
                    for (int i = 0; i < NUM_LEDS; i++) {
                        if (flashState == false) {
                            leds[i].setRGB(0, 0, 0);
                        } else {
                            leds[i].setRGB(255, 0, 0);
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
                            leds[i].setRGB(colors[i][0], colors[i][1], colors[i][2]);
                            leds[NUM_LEDS-1-i].setRGB(colors[i][0], colors[i][1], colors[i][2]);
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