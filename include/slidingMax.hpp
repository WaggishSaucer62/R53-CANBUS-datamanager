/*
File for slidingMax class, manages the previous time window and data related to finding the max HP in the last minute.
WaggishSaucer62, 17/12/25
*/

#pragma once
#include "functions.h"
#include "globals.h"

struct Sample {
    unsigned long timeMS;
    float value;
};

class slidingMax {
public:
    const unsigned long window = 60000; // 1 minute in MS

    void add(unsigned long currentTimeMS, float value) {
        // Remove smaller values from the back
        while (!deque.empty() && deque.back().value <= value) {
            deque.pop_back();
        }

        deque.push_back({currentTimeMS, value});

        // Remove values older than 60 seconds
        while (!deque.empty() && deque.front().timeMS + window < currentTimeMS) {
            deque.pop_front();
        }
    }

    float max() const {
        return deque.empty() ? 0.0f : deque.front().value;
    }

private:
    std::deque<Sample> deque;
};