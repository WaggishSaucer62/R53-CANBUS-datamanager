/*
File for shiftCueDotsLED class, contains all code relevant to updating and drawing them, based on non-led version.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class shiftCueDotsLED {
    private:
        bool shiftDots[5] = {false,false,false,false,false};
        bool previousDotsState[5] = {false,false,false,false,false};
        int colors[5][3] = {{0, 255, 0}, {0, 255, 0}, {255, 255, 0}, {255, 255, 0}, {255, 0, 0}}; // G, G, Y, Y, R
        bool flashState = false;
        bool flashing = false;
        int lastToggleMillis = 0;

    public:
        int xPos;
        int yPos;
        int flashingRPM;
        
        void update(int rpm) {
            if (rpm >= 6000) { // I'm sorry for making you read this... but it works and I'm lazy...
                shiftDots[0]=true;
                shiftDots[1]=true;
                shiftDots[2]=true;
                shiftDots[3]=true;
                shiftDots[4]=true;
            } else if (rpm >= 5000) {
                shiftDots[0]=true;
                shiftDots[1]=true;
                shiftDots[2]=true;
                shiftDots[3]=true;
                shiftDots[4]=false;
            } else if (rpm >= 4000) {
                shiftDots[0]=true;
                shiftDots[1]=true;
                shiftDots[2]=true;
                shiftDots[3]=false;
                shiftDots[4]=false;
            } else if (rpm >= 3000) {
                shiftDots[0]=true;
                shiftDots[1]=true;
                shiftDots[2]=false;
                shiftDots[3]=false;
                shiftDots[4]=false;
            } else if (rpm >= 2000) {
                shiftDots[0]=true;
                shiftDots[1]=false;
                shiftDots[2]=false;
                shiftDots[3]=false;
                shiftDots[4]=false;
            } else if (rpm >= 1000) {
                shiftDots[0]=false;
                shiftDots[1]=false;
                shiftDots[2]=false;
                shiftDots[3]=false;
                shiftDots[4]=false;
            }

            if (rpm >= flashingRPM) {
                flashing = true;
            } else {
                flashing = false;
            }

            if (flashing == true) {
                if (millis() - lastToggleMillis >= 500) {
                    lastToggleMillis = millis();
                    for (int i = 0; i < NUM_LEDS; i++) {
                        if (flashState == false) {
                            leds[i].setRGB(0, 0, 0);
                            flashState = true;
                        } else {
                            leds[i].setRGB(255, 0, 0);
                            flashState = false;
                        }
                    }
                }
                
            } else {
                for (int i = 0; i < 5; i++) {
                        if (shiftDots[i] != previousDotsState[i]) {
                            if (shiftDots[i] == false) {
                                leds[i].setRGB(0, 0, 0);
                                leds[NUM_LEDS-1-i].setRGB(0, 0, 0);
                            } else {
                                leds[i].setRGB(colors[i][0], colors[i][1], colors[i][2]);
                                leds[NUM_LEDS-1-i].setRGB(colors[i][0], colors[i][1], colors[i][2]);
                            }
                        }
                }
            }
            memcpy(previousDotsState, shiftDots, sizeof(shiftDots));
        }
};