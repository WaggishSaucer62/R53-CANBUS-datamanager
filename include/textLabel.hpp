/*
File for textLabel class, used for random bits of text like outside temp, contains all code relevant to updataing and drawing the text.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class textLabel {
  private:
    int lastVal = 0;

  public:
    int xPos;
    int yPos;

    void init() {
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(3);
        tft.drawNumber(lastVal, xPos, yPos);
    }

    void update(int val) { // FIX THIS AND MAKE IT ACTUALLY BLANK A BOX THE RIGHT SIZE NOT AN ARBITRARY CIRCLE
        if (val != lastVal) {
            tft.fillCircle(xPos, yPos, 20, TFT_BLACK);
            tft.setTextDatum(MC_DATUM);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextSize(3);
            tft.drawNumber(val, xPos, yPos);
            lastVal = val;
  }
    }
};