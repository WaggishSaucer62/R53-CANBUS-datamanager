/*
File for verticalSlider class, contains all code relevant to updataing and drawing the slider, but does not manage if it was touched, it must be passed percentage when called.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"

class verticalSlider {
    private:
        int percentage;
        int lastPercent;

    public:
        int xPos;
        int yPos;
        int height;
        int width;
        int radius;
        

        void init(int initPercent) {
            percentage = initPercent;
            lastPercent = initPercent;
            tft.fillRect(xPos - width/2, yPos, width, height, TFT_WHITE);
            tft.fillCircle(xPos, yPos + height - ((height*percentage)/100), radius, TFT_WHITE);
        }

        void update(int newPercent) {
            percentage = newPercent;
            if (lastPercent != percentage) {
                tft.fillCircle(xPos, yPos + height - ((height*lastPercent)/100), radius, TFT_BLACK);
                tft.fillRect(xPos - width/2, yPos, width, height, TFT_WHITE);
                tft.fillCircle(xPos, yPos + height - ((height*percentage)/100), radius, TFT_WHITE);
                lastPercent = percentage;
            }
        }

        int getPercentage() {
            return percentage;
        }
};