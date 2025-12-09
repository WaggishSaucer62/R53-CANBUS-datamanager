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
        unsigned long lastLogTime = 0;

        File logFile;
        size_t bytesWritten = 0;
        size_t lastFlushedSector = 0;

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

        int logIntervalMs;
        bool loggingActive = false;

        String fileName;

        void init() {
            fileName = getNextLogFileName();
            logFile = SD.open(fileName, FILE_WRITE);

            if (!logFile) {
                // ADD ERROR MESSAGES HERE
                return;
            }

            String headerLine = "timestamp";
            for (auto &keyValue : data) {
                headerLine += ",";
                headerLine += keyValue.first;
            }
            headerLine += "\n";
            bytesWritten += logFile.print(headerLine);
            logFile.flush();
            lastLogTime = millis();
        }

        void log() {
            if ((millis() - lastLogTime < logIntervalMs) || (loggingActive == false)) {
                return;
            }

            String newLine;
            newLine.reserve(96); // Basic calculations plus a bunch of overhead for my bad maths.
            newLine = String(millis()) + ",";

            for (auto &keyValue : data) {
                if (newLine != "") {
                    newLine += ",";
                }
                std::visit([&newLine](auto pointer){
                    newLine += String(*pointer);
                }, keyValue.second);
            }

            newLine += "\n";
            bytesWritten += logFile.print(newLine);

            size_t currentSector = bytesWritten / 512;

            if (currentSector != lastFlushedSector) {
                logFile.flush();
                lastFlushedSector = currentSector;
            }
            lastLogTime = millis();
        }

        void close() {
            if (logFile) {
                logFile.flush();
                logFile.close();
                bytesWritten = 0;
                lastFlushedSector = 0;
                loggingActive = false;
            }
        }
};