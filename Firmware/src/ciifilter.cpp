#include "cii_filter.h"
#include "config.h"
#include <math.h>

CIIFilter::CIIFilter() : windowIndex(0), bufferFull(false) {
    for (int i = 0; i < WINDOW_SIZE; i++) ciiHistory[i] = 0.0;
}

// Unified multi-parameter CII Fusion Matrix
float CIIFilter::computeCII(float p, float v, float s, float dp, float dv) {
    float cii = (W_P * p) + (W_V * v) + (W_S * s) + (W_DP * dp) + (W_DV * dv);
    if (cii > 1.0) cii = 1.0;
    if (cii < 0.0) cii = 0.0;
    return cii;
}

// Computes µ (Mean)
float CIIFilter::getMovingAverage(float currentCII) {
    ciiHistory[windowIndex] = currentCII;
    int count = bufferFull ? WINDOW_SIZE : (windowIndex + 1);
    
    float sum = 0;
    for (int i = 0; i < count; i++) {
        sum += ciiHistory[i];
    }
    return sum / count;
}

// Computes σ (Standard Deviation)
float CIIFilter::getMovingStdDev(float currentCII, float avg) {
    int count = bufferFull ? WINDOW_SIZE : (windowIndex + 1);
    float varianceSum = 0;
    
    for (int i = 0; i < count; i++) {
        varianceSum += pow(ciiHistory[i] - avg, 2);
    }
    
    // Advance sliding rolling index pointer safely
    windowIndex++;
    if (windowIndex >= WINDOW_SIZE) {
        windowIndex = 0;
        bufferFull = true;
    }
    
    return sqrt(varianceSum / count);
}

// Statistical Adaptive Threshold Calculation: µ + k*σ
float CIIFilter::calculateAdaptiveThreshold(float avg, float stdDev) {
    float threshold = avg + (K_SCALE * stdDev);
    if (threshold < 0.20) threshold = 0.20; // Hard noise-floor limit to prevent false positives in quiet environments
    return threshold;
}