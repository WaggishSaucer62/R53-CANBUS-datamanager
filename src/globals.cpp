/*
File for defining all global variables
WaggishSaucer62, 27/11/25
*/

#include "globals.h"
#include "functions.h"


MCP_CAN CAN(MCP_CS);
TFT_eSPI tft = TFT_eSPI();

CRGB leds[NUM_LEDS];


screenID currentScreen = MAIN_SCREEN;

uint16_t xTouch;
uint16_t yTouch;
bool pressed = false;


int brightnessPercentage = 100;
int LEDbrightnessPercentage = 100;

bool lastPressedState = false;


arcDial rpmDial;
verticalBar fuelGauge;
verticalBar throttleGauge;
shiftCueDots shiftDots;
textLabel tempText;
button loggingToggle;
verticalSlider brightnessSlider;
verticalSlider LEDbrightnessSlider;
rollingGraph powerGraph(tft, 10, 150, 300, 80, 0, 200);
CANBUS canBus;
SDCardReader sd(SD);
shiftCueDotsLED shiftDotsLED;
configManager config(sd);
loggingManager logger(sd);
powerCalculator powerCalc(canBus);