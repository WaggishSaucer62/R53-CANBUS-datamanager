/*
File for verticalBarGauge class, contains all code relevant to updataing and drawing the gauge.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class verticalBar {
    private:
        int lastPercentage;

    public:
        int percentage;
        int xPos;
        int yPos;
        int width;
        int height;

        void init() {
            lastPercentage = 0;
            tft.fillRect(xPos, yPos, width, height, DARKER_GREY);
        }

        void update(int percentage) {
            if (lastPercentage != percentage) {
                int lastHeight = (height * lastPercentage) / 100;
                int newHeight  = (height * percentage) / 100;

                if (percentage > lastPercentage) {
                tft.fillRect(xPos, yPos + height - newHeight, width, newHeight - lastHeight, TFT_WHITE);
                } else {
                tft.fillRect(xPos, yPos + height - lastHeight, width, lastHeight - newHeight, DARKER_GREY);
                }

                lastPercentage = percentage;
            }
        }
};