/*
File for textLabel class, used for random bits of text like outside temp, contains all code relevant to updataing and drawing the text.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class textLabel {
  private:
  String lastVal = "";

  public:
    int xPos;
    int yPos;
    int textSize;

    void init(String initVal) {
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(textSize);
        tft.drawString(initVal, xPos, yPos);
        lastVal = initVal;
    }

    void update(String val) {
        if (val != lastVal) {
            tft.setTextDatum(MC_DATUM);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextSize(textSize);
            int16_t width = tft.textWidth(lastVal);
            int16_t height = tft.fontHeight();
            tft.fillRect(xPos - (width / 2), yPos - (height / 2), width, height, DARKER_GREY);
            tft.drawString(val, xPos, yPos);
            lastVal = val;
        }
    }
};