/* NOTE: DISPLAY AND TOUCH ORIGIN IS TOP LEFT CORNER
This is the main file, mainly contains just basic logic, most actual code is within methods and functions.
WaggishSaucer62, 27/11/25
*/

#include "functions.h"
#include "globals.h"


void setup(void) {
    Serial.begin(115200);
    // Disable TFT/Touch while MCP initializes to prevent clashes
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
    pinMode(TOUCH_CS, OUTPUT);
    digitalWrite(TOUCH_CS, HIGH);

    SPI.begin(18, 19, 23, MCP_CS);
    if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
        Serial.println("MCP2515 Init OK!");
        CAN.setMode(MCP_NORMAL);
    } else {
        Serial.println("MCP2515 Init FAIL!");
        while (1) delay(1000);
    }

    // Now initialize TFT
    tft.init();
    analogWrite(22, 255);
    tft.setRotation(3);
    touchCalibrate();
    tft.fillScreen(TFT_BLACK);


    // Initialise object parameters, these are global objects defined in globals.h/.cpp
    rpmDial.xPos = tft.width()/2;
    rpmDial.yPos = (tft.height()/2) + 20;
    rpmDial.radius = 80;
    rpmDial.lowerBound = 0;
    rpmDial.upperBound = 8000;
    rpmDial.val1 = 0;
    rpmDial.val2 = 0;

    fuelGauge.xPos = 10;
    fuelGauge.yPos = tft.height()-10-100;
    fuelGauge.width = 20;
    fuelGauge.height = 100;
    fuelGauge.percentage = 0;

    throttleGauge.xPos = tft.width()-10-20;
    throttleGauge.yPos = tft.height()-10-100-30;
    throttleGauge.width = 20;
    throttleGauge.height = 100;
    throttleGauge.percentage = 0;

    shiftDots.xPos = tft.width()/2;
    shiftDots.yPos = 20;
    shiftDots.spacing = 30;
    shiftDots.radius = 10;

    tempText.xPos = tft.width() - 20;
    tempText.yPos = tft.height() - 20;

    loggingToggle.xPos = 200;
    loggingToggle.yPos = 100;
    loggingToggle.radius = 20;
    loggingToggle.colourOn = TFT_GREEN;
    loggingToggle.colourOff = TFT_RED;
    loggingToggle.text = "Logging";
    loggingToggle.state = false;

    brightnessSlider.xPos = 30;
    brightnessSlider.yPos = 30;
    brightnessSlider.height = 180;
    brightnessSlider.width = 6;
    brightnessSlider.radius = 20;

    // Opens main screen by default
    mainScreenInit();
    // calibrateAndPrintTouchData(); // UNCOMMENT, COMMENT OUT LOOP TO CALIBRATE TOUCH, prints calibration data to serial
}


void loop() {
    static unsigned long lastDraw = 0;
    canBus.update();
    if (millis() - lastDraw > 20) { // Only draw and check touch at 50fps
        pressed = tft.getTouch(&xTouch, &yTouch);
        checkScreenSwitch(xTouch, yTouch); // Checks if screen should be switched
        
        switch(currentScreen) {
        case MAIN_SCREEN:
            mainScreen();
            break;
        case SETTINGS_SCREEN:
            settingsScreen();
            break;
        case POWER_SCREEN:
            powerScreen();
            break;
        }
        lastDraw = millis();
    }
}