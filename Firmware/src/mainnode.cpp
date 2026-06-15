#include <Arduino.h>
#include <Wire.h>
#include "HX711.h"
#include "config.h"
#include "ciifilter.h"
#include "loracomm.h"

// ADXL345 Registers via I2C Bus Configuration 
#define ADXL345_ADDR          0x53
#define ADXL345_DATA_FORMAT   0x31
#define ADXL345_POWER_CTL     0x2D
#define ADXL345_DATAX0        0x32

HX711 scale;
CIIFilter filterStack;
// State Tracking Parameters
float prevP = 0.0, prevV = 0.0;
unsigned long lastTickTime = 0;
unsigned long simStartTime = 0;
void writeI2C(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(ADXL345_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

void initADXL345() {
    Wire.begin(21, 22); // SDA pin 21, SCL pin 22
    writeI2C(ADXL345_POWER_CTL, 0x08);   // Measurement mode active
    writeI2C(ADXL345_DATA_FORMAT, 0x0B); // Full resolution, +/-16g dynamic range
    Serial.println("[SUCCESS] ADXL345 Accelerometer Bus Synced.");
}

void readAccelerometer(float &vibeMetric) {
    Wire.beginTransmission(ADXL345_ADDR);
    Wire.write(ADXL345_DATAX0);
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345_ADDR, 6, true);

    int16_t x = (Wire.read() | (Wire.read() << 8));
    int16_t y = (Wire.read() | (Wire.read() << 8));
    int16_t z = (Wire.read() | (Wire.read() << 8));

    // Convert raw reading to G force magnitude
    float ax = x * 0.0039;
    float ay = y * 0.0039;
    float az = z * 0.0039;
    float totalG = sqrt(ax*ax + ay*ay + az*az);

    // Dynamic noise isolation filtering
    float AC_Component = abs(totalG - 1.0); // Remove 1G earth gravity bias
    vibeMetric = AC_Component * 2.0;       // Scale sensitivity
    if (vibeMetric > 1.0) vibeMetric = 1.0;
}

void setup() {
    Serial.begin(115200);
    simStartTime = millis();
    lastTickTime = millis();

    initADXL345();
    scale.begin(HX711_DT, HX711_SCK);
    initLoRa();
    
    randomSeed(analogRead(35));
    Serial.println("[SYSTEM RUNNING] Edge Compute Node Active.");
}

void loop() {
    unsigned long currentTick = millis();
    
    // Execute exactly once per second non-blockingly
    if (currentTick - lastTickTime >= 1000) {
        float dt = (currentTick - lastTickTime) / 1000.0;
        float vibrationLevel = 0.0;
        readAccelerometer(vibrationLevel);
        float noiseLevel = analogRead(MIC_PIN) / 4095.0;
        long rawWeight = scale.is_ready() ? scale.read() : 0;
        float weightLevel = abs(rawWeight) / 10000000.0;
        if (weightLevel > 1.0) weightLevel = 1.0;

        // Dynamic Time Delta Derivatives Calculation
        float deltaP = (weightLevel - prevP) / dt;
        float deltaV = (vibrationLevel - prevV) / dt;
        prevP = weightLevel;
        prevV = vibrationLevel;

        // Compute Multi-Sensor Fusion Framework Strategy
        float currentCII = filterStack.computeCII(weightLevel, vibrationLevel, noiseLevel, deltaP, deltaV);

        // Calculate Statistical Tracking Threshold Matrix Limits
        float mu = filterStack.getMovingAverage(currentCII);
        float sigma = filterStack.getMovingStdDev(currentCII, mu);
        float adaptiveThreshold = filterStack.calculateAdaptiveThreshold(mu, sigma);

        float alertCode = 0.0; 
        
        Serial.println("\n=== REAL-TIME NODE METRICS ===");
        Serial.printf("P: %.2f | V: %.2f | S: %.2f | dP/dt: %.2f | dV/dt: %.2f\n", weightLevel, vibrationLevel, noiseLevel, deltaP, deltaV);
        Serial.printf("CII Value: %.4f | Dynamic Limit (µ+kσ): %.4f\n", currentCII, adaptiveThreshold);
        if (currentCII > adaptiveThreshold || currentCII > 0.70) { 
            alertCode = 1.0;
            Serial.println("[!!! RUNTIME THRESHOLD ALERT CRITICAL RISK !!!]");
        } else {
            Serial.println("System Security Frame State: SECURE / MINIMAL DISTURBANCE");
        }
        transmitTelemetry(currentCII, noiseLevel, weightLevel, vibrationLevel, alertCode);
        lastTickTime = currentTick;
    }
}