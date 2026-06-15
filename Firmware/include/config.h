#ifndef CONFIG_H
#define CONFIG_H

// --- SPI & LoRa Transceiver Hardware Pins ---
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  26
#define LORA_BAND  433E6 // 433 MHz frequency band

// --- Sensor Peripheral Pin Hardware Mappings ---
#define MIC_PIN    34
#define HX711_DT   4
#define HX711_SCK  15

// --- Adaptive Thresholding Configurations ---
#define WINDOW_SIZE 10     // Sliding window size for rolling statistical metrics
const float K_SCALE = 1.5; // Dynamic scaling factor (k) for µ + k*σ

// --- CII Sensor Fusion Coefficients ---
// NOTE: Exact calibrated weights are redacted due to pending patent. 
// Dummy values are provided below for structural compilation purposes.
const float W_P  = 0.20;  
const float W_V  = 0.20;  
const float W_S  = 0.20;  
const float W_DP = 0.20;  
const float W_DV = 0.20;

#endif