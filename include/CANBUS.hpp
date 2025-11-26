/*
File for CANBUS class to parse CAN frames and hold data for other functions.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class CANBUS {
    public:
        int rpm;
        float spdAvg;
        int throttlePos;
        int fuelPercent;
        float externalTemp;

        void update() {
            if (CAN.checkReceive() != CAN_MSGAVAIL) return;

            long unsigned int rxId;
            unsigned char len;
            unsigned char rxBuf[8];
            CAN.readMsgBuf(&rxId, &len, rxBuf);

            switch (rxId) {
                case 0x316: handleRPM(rxBuf); break;
                case 0x01F0: handleSpeed(rxBuf); break;
                case 0x0329: handleThrottle(rxBuf); break;
                case 0x0613: handleFuel(rxBuf); break;
                case 0x0615: handleExternalTemp(rxBuf); break;
            }
        }

    private:
        void handleRPM(unsigned char* b) {
            float rpmRaw = (b[2]) | (b[3] << 8);
            rpm = rpmRaw * 0.15625;
        }

        void handleSpeed(unsigned char* b) {
            float rawFL = b[0] | (b[1] << 8);
            float rawFR = b[2] | (b[3] << 8);
            float rawRL = b[4] | (b[5] << 8);
            float rawRR = b[6] | (b[7] << 8);
            float scalingFactor = 14.9;

            float spd[4] = {
                (rawFL - 10) / scalingFactor,
                (rawFR - 10) / scalingFactor,
                (rawRL - 10) / scalingFactor,
                (rawRR - 10) / scalingFactor
            };

            float sum = 0;
            int count = 0;
            for (float v : spd) {
                if (v >= 0 && v <= 150) {
                    sum += v;
                    count++;
                }
            }
            spdAvg = (count ? sum / count : 0);
        }

        void handleThrottle(unsigned char* b) {
            throttlePos = b[5] * 0.392156863;
        }

        void handleFuel(unsigned char* b) {
            fuelPercent = b[2] * 2;
        }

        void handleExternalTemp(unsigned char* b) {
            externalTemp = b[3];
        }
};