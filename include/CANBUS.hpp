/*
File for CANBUS class to parse CAN frames and hold data for other functions.
WaggishSaucer62, 27/11/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class CANBUS {
    public:
        int rpm = 0;
        float spdAvg = 0;
        int throttlePos = 0;
        int fuelPercent = 100;
        String externalTemp = "0";

        void update() {
            while (CAN.checkReceive() == CAN_MSGAVAIL) {
                long unsigned int rxId;
                unsigned char len;
                unsigned char rxBuf[8];
                CAN.readMsgBuf(&rxId, &len, rxBuf);

                switch (rxId) {
                    case 0x316: processRPM(rxBuf); break;
                    case 0x01F0: processSpeed(rxBuf); break;
                    case 0x0329: processThrottle(rxBuf); break;
                    case 0x0613: processFuel(rxBuf); break;
                    case 0x0615: processExternalTemp(rxBuf); break;
                }
            }
        }

    private:
        void processRPM(unsigned char* b) {
            float rpmRaw = (b[2]) | (b[3] << 8);
            rpm = rpmRaw * 0.15625;
        }

        void processSpeed(unsigned char* b) {
            float rawFL = b[0] | (b[1] << 8);
            float rawFR = b[2] | (b[3] << 8);
            float rawRL = b[4] | (b[5] << 8);
            float rawRR = b[6] | (b[7] << 8);
            float scalingFactor = 15.2;

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

        void processThrottle(unsigned char* b) {
            throttlePos = b[5] * 0.392156863;
        }

        void processFuel(unsigned char* b) {
            fuelPercent = b[2] * 2;
        }

        void processExternalTemp(unsigned char* b) {
            externalTemp = b[3];
        }
};