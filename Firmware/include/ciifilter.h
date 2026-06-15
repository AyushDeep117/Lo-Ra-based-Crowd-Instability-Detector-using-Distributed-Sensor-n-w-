#ifndef CII_FILTER_H
#define CII_FILTER_H

#include <Arduino.h>

class CIIFilter {
private:
    float ciiHistory[WINDOW_SIZE];
    int windowIndex;
    bool bufferFull;

public:
    CIIFilter();
    float computeCII(float p, float v, float s, float dp, float dv);
    float getMovingAverage(float currentCII);
    float getMovingStdDev(float currentCII, float avg);
    float calculateAdaptiveThreshold(float avg, float stdDev);
};

#endif