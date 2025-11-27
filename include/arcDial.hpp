/*
File for arcDial class, contains all code relevant to the main dial and its enclosed text readouts.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"

class arcDial {
    private:
        int dialPercentage;
        int valAngle;
        int innerRadius;
        uint8_t redValue;
        uint8_t greenValue;
        uint16_t colour;
        uint8_t thickness = 20;
        int lastAngle;

        void drawGradientArc(float startAngle, float endAngle, uint16_t startColor, uint16_t endColor) { // Draws a full gradient arc to ensure it is all coloured correctly when switching screen
            float step = 0.5; // smaller = smoother but slower
            for (float a = startAngle; a <= endAngle; a += step) {
                float progress = (a - startAngle) / (endAngle - startAngle);

                // Interpolate colour
                uint8_t r1 = (startColor >> 11) & 0x1F;
                uint8_t g1 = (startColor >> 5) & 0x3F;
                uint8_t b1 = startColor & 0x1F;

                uint8_t r2 = (endColor >> 11) & 0x1F;
                uint8_t g2 = (endColor >> 5) & 0x3F;
                uint8_t b2 = endColor & 0x1F;

                uint8_t rGrad = r1 + progress * (r2 - r1);
                uint8_t gGrad = g1 + progress * (g2 - g1);
                uint8_t bGrad = b1 + progress * (b2 - b1);

                uint16_t colorGrad = (rGrad << 11) | (gGrad << 5) | bGrad;

                tft.drawArc(xPos, yPos, innerRadius, innerRadius - thickness, a, a + step, colorGrad, TFT_BLACK);
            }
        }


    public:
        int val1; //rpm for this project
        int val2; //speed for this project
        int xPos;
        int yPos;
        int radius;
        int upperBound;
        int lowerBound;

        void init() { // Draws various static elements of dial
            lastAngle = 30;
            innerRadius = radius - 3;
            dialPercentage = map(val1, lowerBound, upperBound, 0, 100);
            redValue = map(dialPercentage, 0, 100, 0, 255);
            greenValue = map(dialPercentage, 0, 100, 255, 0);
            colour = tft.color565(redValue, greenValue, 0);
            valAngle = map(dialPercentage, 0, 100, 30, 330); // scale 0-100 to arc 30-330deg

            tft.fillCircle(xPos, yPos, innerRadius, DARKER_GREY);
            tft.drawSmoothCircle(xPos, yPos, radius, TFT_SILVER, DARKER_GREY);

            tft.setTextDatum(MC_DATUM);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextSize(1);
            for (int i = 0; i <= 7; i++) { // Draw dial labels and tick marks
                float angle = radians((30 + i * (300.0 / 7)+90));
                int x0 = xPos + cos(angle) * (innerRadius - thickness-8); 
                int y0 = yPos + sin(angle) * (innerRadius - thickness-8);
                int x1 = xPos + cos(angle) * (innerRadius - thickness); 
                int y1 = yPos + sin(angle) * (innerRadius - thickness);
                tft.drawLine(x0, y0, x1, y1, TFT_WHITE);

                int labelRadius = radius + 9;
                int lx = xPos + cos(angle) * labelRadius;
                int ly = yPos + sin(angle) * labelRadius;
                tft.drawNumber(i, lx, ly);
            }

            drawGradientArc(30, 330, TFT_GREEN, TFT_RED);
            tft.drawArc(xPos, yPos, innerRadius, innerRadius - thickness, lastAngle, 330, TFT_BLACK, DARKER_GREY);
        }

        void update(int newVal1, int newVal2) { // In this project, newVal1 = rpm, newVal2 = speed // UPDATE DOES NOT SEEM TO BE ONLY DRAWING CHANGED PARTS, RPM TEXT FLASHES 0 AND GAUGE FLASHES BLACK/COLOUR
            dialPercentage = map(newVal1, lowerBound, upperBound, 0, 100);
            redValue = map(dialPercentage, 0, 100, 0, 255);
            greenValue = map(dialPercentage, 0, 100, 255, 0);
            colour = tft.color565(redValue, greenValue, 0);
            valAngle = map(dialPercentage, 0, 100, 30, 330);

            // Update arc if needed
            if (lastAngle != valAngle) {
                if (valAngle > lastAngle) {
                tft.drawArc(xPos, yPos, innerRadius, innerRadius - thickness, lastAngle, valAngle, colour, TFT_BLACK);
                } else {
                tft.drawArc(xPos, yPos, innerRadius, innerRadius - thickness, valAngle, lastAngle, TFT_BLACK, DARKER_GREY);
                }
                lastAngle = valAngle;
            }

            // Update central text
            if (newVal2 != val2) {
                int textW = 80;
                int textH = 40;
                int x0 = xPos - textW / 2;
                int y0 = yPos - textH / 2;
                tft.fillRect(x0, y0, textW, textH, DARKER_GREY);
                tft.setTextDatum(MC_DATUM);
                tft.setTextColor(TFT_WHITE, DARKER_GREY);
                tft.setTextSize(4);
                tft.drawNumber(newVal2, xPos, yPos);
            }

            // Update lower text
            if (newVal1 != val1) {
                int textW = 60;
                int textH = 20;
                int x0 = xPos - textW / 2;
                int y0 = yPos + 30 - textH / 2;
                tft.fillRect(x0, y0, textW, textH, DARKER_GREY);
                tft.setTextDatum(MC_DATUM);
                tft.setTextColor(TFT_WHITE, DARKER_GREY);
                tft.setTextSize(2);
                tft.drawNumber(newVal1, xPos, yPos + 30);
            }

            // Update values for other functions and next loop
            val1 = newVal1;
            val2 = newVal2;
        }
};