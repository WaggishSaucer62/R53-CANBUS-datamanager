/*
File for rollingGraph class, contains all code relevant to updataing and drawing the graph.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class rollingGraph {
private:
    TFT_eSPI &tft;
    int x, y;
    int width, height;
    float minVal, maxVal;
    uint16_t *buffer;
    int index;
    uint16_t bgColor;

public:
    rollingGraph(TFT_eSPI &tftRef, int posX, int posY, int w, int h, float minV, float maxV, uint16_t background = 0x18E3)
        : tft(tftRef), x(posX), y(posY), width(w), height(h), minVal(minV), maxVal(maxV), bgColor(background)
    {
        buffer = new uint16_t[width];
        for (int i = 0; i < width; i++) buffer[i] = y + height;
        index = 0;
    }

    void init() {
        tft.fillRect(x, y, width, height, bgColor);
        index = 0;
    }

    void update(float value, uint16_t color = 0x07E0) {
        if (value > maxVal) value = maxVal;
        if (value < minVal) value = minVal;

        int newY = y + height - map(value * 1000, minVal * 1000, maxVal * 1000, 0, height);
        tft.drawLine(x + index, y, x + index, y + height, bgColor);

        if (index != 0) {
            int prevY = buffer[index - 1];
            tft.drawLine(x + index - 1, prevY, x + index, newY, color);
        }

        buffer[index] = newY;

        index++;
        if (index >= width) {
            index = 0;
        }
    }

    ~rollingGraph() {
        delete[] buffer;
    }
};