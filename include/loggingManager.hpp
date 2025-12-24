/*
File for loggingManager class, wraps all logging logic, all that is needed is to call loggingManager.log(), frequency is managed internally.
WaggishSaucer62, 06/12/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class loggingManager {
    private:
        using logPointer = std::variant< // Needed to hold pointers to different types of variables in canBus
            int*,
            float*,
            double*,
            bool*,
            String*
        >;

        SDCardReader& sd;
        File logFile;

        unsigned long lastLogTime = 0;
        unsigned long lastFlushTime = 0;

        String getNextLogFileName() {
            int i = 0;
            while (true) {
                String name = "/log_" + String(i) + ".csv";
                if (!SD.exists(name)) {
                    return name;
                } else if (i >= 9999) {
                    return "/log_overflow.csv";
                }
                i++;
            }
        }

    public:
        loggingManager(SDCardReader& reader) : sd(reader) {}
        std::map<String, logPointer> data; // stores pair of data (actually a pointer to the canbus object var) and name, eg. speed -> 50

        int logIntervalMs = 100; // Default
        bool loggingEnabled = false;
        bool loggingActive = false;

        String fileName;

        void init() {
            fileName = getNextLogFileName();
            logFile = SD.open(fileName, FILE_WRITE);

            if (!logFile) {
                // ADD ERROR MESSAGES HERE
                return;
            }

            logFile.print("timestamp");
            for (auto &kv : data) {
                logFile.print(",");
                logFile.print(kv.first);
            }
            logFile.print("\n");
            logFile.flush();

            lastLogTime = millis();
            lastFlushTime = millis();
            lastReopenTime = millis();
            loggingActive = true;
        }

        void log() {
            unsigned long now = millis();
            if (now - lastLogTime < logIntervalMs) {
                return;
            }

            logFile.print(now / 1000.0f, 3);

            for (auto &keyValue : data) {
                logFile.print(",");
                std::visit([this](auto pointer) {
                    logFile.print(*pointer);
                }, keyValue.second);
            }
            logFile.print("\n");

            if (now - lastFlushTime >= 5000) {
                logFile.flush();
                lastFlushTime = now;
            }

            lastLogTime = millis();
        }

        void close() {
            if (logFile) {
                logFile.flush();
                logFile.close();
            }
            loggingActive = false;
        }
};