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


float dragCoeff = 0.39;
float airDensity = 1.225;
float frontalArea = 1.98;
float mass = 1215;
float powerInformation[4] = {0,0,0,0};

int brightnessPercentage = 100;

bool lastPressedState = false;


arcDial rpmDial;
verticalBar fuelGauge;
verticalBar throttleGauge;
shiftCueDots shiftDots;
textLabel tempText;
button loggingToggle;
verticalSlider brightnessSlider;
rollingGraph powerGraph(tft, 10, 150, 300, 80, 0, 200);
CANBUS canBus;