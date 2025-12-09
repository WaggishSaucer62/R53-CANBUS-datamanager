/*
File for random functions that do not fit into other files
WaggishSaucer62, 27/11/25
*/

#include "functions.h"
#include "globals.h"

#define TOUCH_SAMPLES 5

void fullscreenWarning(String text) {
    tft.fillScreen(TFT_RED);
    tft.setTextSize(4);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(text, tft.width() / 2, tft.height() / 2);
    delay(10000);
    tft.fillScreen(TFT_BLACK);
    switch(currentScreen) {
    case MAIN_SCREEN:
        mainScreenInit();
        break;
    case SETTINGS_SCREEN:
        settingsScreenInit();
        break;
    case POWER_SCREEN:
        powerScreenInit();
        break;
    }
}


void touchCalibrate() {
    uint16_t calData[5] = {344, 3430, 393, 3245, 1};
    tft.setTouch(calData);
}




void calibrateAndPrintTouchData() {
  uint16_t calData[5];  // Array to store calibration values

  // Clear screen
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("Touch the corners as indicated");

  // Run built-in calibration
  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

  // Apply calibration immediately
  tft.setTouch(calData);

  // Print the calibration data to Serial
  Serial.println("Calibration complete! Use this array in your code:");
  Serial.print("uint16_t calData[5] = { ");
  for (int i = 0; i < 5; i++) {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }
  Serial.println(" };");

  // Optional: show on screen
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 50);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.println("Calibration done!");
}