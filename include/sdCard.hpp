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
#include "globals.h"


class SDCardReader {
private:
    fs::FS &fs;

public:
    SDCardReader(fs::FS &filesystem) : fs(filesystem) {}

    bool appendFile(const String &path, const String &text) {
        File file = fs.open(path, FILE_APPEND);
        if (!file) return false;

        bool saveStatus = file.print(text);
        file.close();
        return saveStatus;
    }

    bool writeFile(const String &path, const String &text) {
        File file = fs.open(path, FILE_WRITE);
        if (!file) return false;

        bool saveStatus = file.print(text);
        file.close();
        return saveStatus;
    }

    String readFile(const String &path) {
        File file = fs.open(path);
        if (!file) return "";

        String contents;
        contents.reserve(file.size());

        while (file.available()) {
            contents += (char)file.read();
        }

        file.close();
        return contents;
    }

    bool deleteFile(const String &path) {
        return fs.remove(path);
    }

    bool createDir(const String &path) {
        return fs.mkdir(path);
    }
};