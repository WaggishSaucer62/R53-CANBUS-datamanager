/*
File for configManager class, handles reading, writing, and editing config (json) files.
WaggishSaucer62, 05/12/25
*/

#pragma once
#include "functions.h"
#include "globals.h"

class configManager {
private:
    SDCardReader& sd;
    std::map<String, String> data;

    void processLine(String line) {
        line.trim();
        if (line.startsWith("#")) return;
        if (line.length() == 0) return;

        int delimiterIndex = line.indexOf('=');
        if (delimiterIndex == -1) return; // skip when no = found

        String key = line.substring(0, delimiterIndex);
        String value = line.substring(delimiterIndex + 1);

        key.trim();
        value.trim();

        data[key] = value;
    }

public:
    configManager(SDCardReader& reader) : sd(reader) {}

    void load(String filepath) {
        String fileContents = sd.readFile(filepath);
        data.clear();

        String line = "";
        int i = 0;
        while (i < fileContents.length()) {
            char c = fileContents.charAt(i);
            if (c == '\n') {
                processLine(line);
                line = "";
            } else {
                line = line + c;
            }
            i++;
        }
    }

    // Save all key value pairs back to the file
    bool save(String filepath) {
        File f = SD.open(filepath, FILE_WRITE);
        if (!f) return false;

        for (auto &kv : data) {
            f.print(kv.first);
            f.print("=");
            f.println(kv.second);
        }

        f.close();
        return true;
    }

    String get(const String &key, const String &defaultValue = "") {
        if (data.count(key)) return data[key];
        return defaultValue;
    }

    void set(const String &key, const String &value) {
        data[key] = value;
    }

    void removeKey(const String &key) {
        data.erase(key);
    }
};