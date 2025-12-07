/*
File for shiftCueDots class, contains all code relevant to updating and drawing them.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class shiftCueDots {
    private:
        const int thresholds[5] = {2000, 3000, 4000, 5000, 6000};
        bool shiftDots[5] = {false,false,false,false,false};
        bool previousDotsState[5] = {false,false,false,false,false};
        uint16_t colors[5] = {TFT_GREEN, TFT_GREEN, TFT_YELLOW, TFT_YELLOW, TFT_RED};

    public:
        int xPos;
        int yPos;
        int spacing;
        int radius;
        
        void update(int rpm) {
            static int positions[9]; 
            for (int i = 0; i < 9; i++) {
                positions[i] = xPos + ((i-4)*spacing);
            }

            for (int i = 0; i < 5; i++) { // Updates each dot state based on relevant thresholds
                shiftDots[i] = (rpm >= thresholds[i]);
            }

            for (int i = 0; i < 5; i++) {
                if (shiftDots[i] != previousDotsState[i]) {
                if (shiftDots[i] == false) {
                    tft.fillCircle(positions[i], yPos, radius, TFT_BLACK);
                    tft.fillCircle(positions[8-i], yPos, radius, TFT_BLACK);
                } else {
                tft.fillCircle(positions[i], yPos, radius, colors[i]);
                tft.fillCircle(positions[8-i], yPos, radius, colors[i]);
                }
                }
            }
            for (int i = 0; i < 5; i++) {
                previousDotsState[i] = shiftDots[i];
            }
        }
};