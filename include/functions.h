/*
Header file for functions forward definitions, allowing for global use in other files.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <mcp_can.h>
#include <SPI.h>

#include "globals.h"


void touchCalibrate();
void checkScreenSwitch(uint16_t xTouch, uint16_t yTouch);
void mainScreenInit();
void settingsScreenInit();
void powerScreenInit();
void mainScreen();
void settingsScreen();
void powerScreen();
void fullscreenWarning(String text);

void calibrateAndPrintTouchData();

screenID parseScreenID(const String &name);