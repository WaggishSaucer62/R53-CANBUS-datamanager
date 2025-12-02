/*
File for SDCardReader class, simplifies SD card operations.
Usage:
sd.writeFile("/config.txt", "speed=300\n");
sd.appendFile("/log.txt", "Engine started\n");
sd.createDir("/logs");
sd.deleteFile("/old.txt");
String data = sd.readFile("/log.txt");


WaggishSaucer62, 02/12/25
*/

#pragma once
#include "functions.h"
#include "globals.h"


class SDCardReader {
private:
    fs::FS &fs;

public:
    SDCardReader(fs::FS &filesystem) : fs(filesystem) {}

    bool appendFile(const char *path, const char *text) {
        File file = fs.open(path, FILE_APPEND);
        if (!file) return false;

        bool saveStatus = file.print(text);
        file.close();
        return saveStatus;
    }

    bool writeFile(const char *path, const char *text) {
        File file = fs.open(path, FILE_WRITE);
        if (!file) return false;

        bool saveStatus = file.print(text);
        file.close();
        return saveStatus;
    }

    String readFile(const char *path) {
        File file = fs.open(path);
        if (!file) {
            return "";
        }

        String contents = "";
        while (file.available()) {
            contents += (char)file.read();
        }

        file.close();
        return contents;
    }

    bool deleteFile(const char *path) {
        return fs.remove(path);
    }

    bool createDir(const char *path) {
        return fs.mkdir(path);
    }
};