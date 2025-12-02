/*
Header file for defining global variables and constants for use in other files.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include <iostream>
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <mcp_can.h>
#include <SPI.h>
#include <string>
#include <FastLED.h>
#include <SD.h>


// Macros – do NOT use extern
#define MCP_CS 27
#define TFT_CS 15
#define TOUCH_CS 21
#define SD_CS 25
#define DARKER_GREY 0x18E3

#define NUM_LEDS 9
#define LED_DATA_PIN 26

extern CRGB leds[NUM_LEDS];

extern MCP_CAN CAN;
extern TFT_eSPI tft;

enum screenID {
    MAIN_SCREEN,
    SETTINGS_SCREEN,
    POWER_SCREEN
};
extern screenID currentScreen;

extern uint16_t xTouch;
extern uint16_t yTouch;
extern bool pressed;


extern float dragCoeff;
extern float airDensity;
extern float frontalArea;
extern float mass;
extern float powerInformation[4];

extern int brightnessPercentage;

extern bool lastPressedState;


#include "arcDial.hpp"
#include "verticalBarGauge.hpp"
#include "shiftCueDots.hpp"
#include "textLabel.hpp"
#include "button.hpp"
#include "verticalSlider.hpp"
#include "rollingGraph.hpp"
#include "CANBUS.hpp"
#include "sdCard.hpp"


extern arcDial rpmDial;
extern verticalBar fuelGauge;
extern verticalBar throttleGauge;
extern shiftCueDots shiftDots;
extern textLabel tempText;
extern button loggingToggle;
extern verticalSlider brightnessSlider;
extern rollingGraph powerGraph;
extern CANBUS canBus;
extern SDCardReader sd;