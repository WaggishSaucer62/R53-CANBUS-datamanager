/*
File for loggingManager class, wraps all logging logic, all that is needed is to call loggingManager.log(), frequency is managed internally.
WaggishSaucer62, 06/12/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class loggingManager {
    private:
    String fileName;
    using logPointer = std::variant< // Needed to hold pointers to different types of variables in canBus
        int*,
        float*,
        double*,
        bool*,
        String*
    >;

    SDCardReader& sd;
    unsigned long lastLogTime = 0;

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

        void init() {
            fileName = getNextLogFileName();
            String headerLine = "";
            for (auto &keyValue : data) {
                if (headerLine != "") {
                    headerLine += ",";
                }
                headerLine += keyValue.first;
            }
            headerLine += "\n";
            sd.writeFile(fileName, headerLine);
        }

        void log() {
            if ((millis() - lastLogTime < logIntervalMs) || (loggingActive == false)) {
                return;
            }

            String newLine = "";
            for (auto &keyValue : data) {
                if (newLine != "") {
                    newLine += ",";
                }
                std::visit([&newLine](auto pointer){
                    newLine += *pointer;
                }, keyValue.second);
            }

            newLine += "\n";
            sd.appendFile(fileName, newLine);
            lastLogTime = millis();
        }
};