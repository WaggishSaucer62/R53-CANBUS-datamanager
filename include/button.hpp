/*
File for button class, self manages if it is pressed when given touch coordinates.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class button {
    private:

    public:
        int xPos;
        int yPos;
        int radius;
        uint16_t colourOn;
        uint16_t colourOff;
        String text;
        bool state;

        void init(bool initialState) {
            if (initialState == true) {
                tft.fillCircle(xPos, yPos, radius, colourOn);
            } else {
                tft.fillCircle(xPos, yPos, radius, colourOff);
            }
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextSize(1);
            tft.setTextDatum(MC_DATUM);
            tft.drawString(text, xPos, yPos+radius+5);

            state = initialState;
        }

        void update(bool newState) {
            if (newState == state) return;
            if (newState == true) {
                tft.fillCircle(xPos, yPos, radius, colourOn);
            } else {
                tft.fillCircle(xPos, yPos, radius, colourOff);
            }
            state = newState;
        }

        void checkIfPressed(uint16_t touchX, uint16_t touchY) {
            int dx = touchX - xPos;
            int dy = touchY - yPos;
            if (dx*dx + dy*dy <= radius*radius) {
                update(!state);
            }
        }

        bool getState() {
            return state;
        }
};