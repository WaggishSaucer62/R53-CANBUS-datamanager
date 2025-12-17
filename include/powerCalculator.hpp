/*
File for powerCalculator class, wrapper to handle calculation and values for power calculations.
WaggishSaucer62, 06/12/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class powerCalculator {
    private:
        const float alpha = 0.05;
        float powerInformation[4] = {0,0,0,0}; // prevSpeed, prevTime, currentSpeed, currentTime

        CANBUS& canBus;

    public:
        powerCalculator(CANBUS& canbus) : canBus(canbus) {}

        float acceleration = 0;
        float smoothedPower = 0;

        float dragCoeff = 0.39;
        float airDensity = 1.225;
        float frontalArea = 1.98;
        float mass = 1215.0;

        float fullWeight = 0;

        float calculatePower() {
            powerInformation[0] = powerInformation[2];
            powerInformation[1] = powerInformation[3];
            powerInformation[2] = canBus.spdAvg;
            powerInformation[3] = millis();

            float dragForce = 0.5*dragCoeff*frontalArea*airDensity*((powerInformation[2]/3.6)*(powerInformation[2]/3.6));
            fullWeight = mass + ((50*0.75*canBus.fuelPercent)/100);
            float dt = (powerInformation[3] - powerInformation[1])/1000.0;
            if ((powerInformation[2] - powerInformation[0] != 0) && dt > 0.01f) {
                acceleration = ((powerInformation[2]-powerInformation[0])/3.6)/dt;
            }
            float power = (((fullWeight*acceleration)+dragForce)*(powerInformation[2]/3.6))/745.7;

            smoothedPower = smoothedPower + alpha * (power - smoothedPower);
            return smoothedPower;
        }
};