/*
File for shiftCueDots class, contains all code relevant to updataing and drawing them.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class shiftCueDots {
    private:
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
            memcpy(previousDotsState, shiftDots, sizeof(shiftDots));
        }
};