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
        float lastSpeed = 0;
        int throttlePos = 0;
        int fuelPercent = 100;
        String externalTemp = "0";
        String engineTemp = "0";

        void update() {
            while (CAN.checkReceive() == CAN_MSGAVAIL) {
                long unsigned int rxId;
                unsigned char len;
                unsigned char rxBuf[8];
                CAN.readMsgBuf(&rxId, &len, rxBuf);

                switch (rxId) {
                    case 0x316: processRPM(rxBuf); break;
                    case 0x01F0: processSpeed(rxBuf); break;
                    case 0x0329: processThrottleEngineTemp(rxBuf); break;
                    case 0x0613: processFuel(rxBuf); break;
                    case 0x0615: processExternalTemp(rxBuf); break;
                    case 0x61a: processIgnitionToggle(rxBuf); break;
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
            float scalingFactor = 15.6;

            float spd[4] = {
                (rawFL - 10) / scalingFactor,
                (rawFR - 10) / scalingFactor,
                (rawRL - 10) / scalingFactor,
                (rawRR - 10) / scalingFactor
            };

            float sum = 0;
            int count = 0;
            for (float v : spd) {
                if (v >= 0 && v <= 250) {
                    sum += v;
                    count++;
                }
            }

            spdAvg = (count ? sum / count : lastSpeed);
            lastSpeed = spdAvg;
        }

        void processThrottleEngineTemp(unsigned char* b) {
            throttlePos = b[5] * 0.392156863;
            engineTemp = String((b[1] * 0.75) - 48.373, 1);
        }

        void processFuel(unsigned char* b) {
            fuelPercent = b[2] * 2;
        }

        void processExternalTemp(unsigned char* b) {
            externalTemp = b[3];
        }

        void processIgnitionToggle(unsigned char* b) {
            if (b[6] > 30 && hibernate == false) {
                analogWrite(22, 0);
                hibernate = true;
            } else if (b[6] <= 30 && hibernate == true) {
                ESP.restart(); // Restarting sets hibernate back to false
            }
        }
};

// notes on decoding CAN IDs:
// Completed IDs: 0x000, 0x153, 0x1f0, 0x1f3, 0x1f5, 0x1f8, 0x316, 0x329, 0x336, 0x545, 0x565, 0x613, 0x618, 0x61a, 0x61f, 0x630
// 0x153 byte 0: brake pressure from 0-255 or so? bytes 1 and 2 are vehicle speed? (data = (b[1]) | (b[2] << 8))
// 0x1f3 has something on byte 3, some others toggle up and down once or twice
// 0x1f5 just randomness?
// 0x1f8 definitely has something on byte 0, others nothing
// 0x329 byte 0 random data? byte 1 is coolant temp?
// 0x336 has data on bytes 0 and 1 together maybe intake pressure?, (data = (b[0]) | (b[1] << 8)), very similar shaped data on (data = (b[4]) | (b[5] << 8)) and (data = (b[6]) | (b[7] << 8))
// 0x545 Fuel consumption is the rate of change of (data = (b[1]) | (b[2] << 8))
// 0x565 byte 7 spends most time at minimum value of 99, but spikes semi-frequently to at least 170
// 0x613 byte 5 and 6 have data, maybe they're combined?
// 0x615 no changing data other than temp
// 0x618 no changing data
// 0x61a bit 1 changes slightly between ~215, 220, bit 1 does a similar thing between ~5, 12, byte 2 moves the same a 1 (at least in this data) but by less and starting at 0, 
// bytes 3 maybe a temp? slowly increases a little over time, though bugs out a little at the start (maybe just until engine start?), byte 4 has same bugged (?) pattern at the
// start as byte 3, but is constant afterwards and is shifted down. byte 5 has data on it, locked at 255 until start (?), then moves between 0 and 255 regularly, but doesn't 
// seem to come off 0 until car moves, but doesn't behave like speed exactly, though follows a very similar shape to the first byte of speed data,
// (for example byte 0 for FL wheel), but no other bytes that look like they would combine, byte 6 seems to be high when engine off (127), and low when running (moves between 0 and 2)
// 0x61f byte 0 goes between 0 and 255 somewhat often, byte 1 hovers between 255 and ~170, load maybe? drops to 0 at start (I think?)