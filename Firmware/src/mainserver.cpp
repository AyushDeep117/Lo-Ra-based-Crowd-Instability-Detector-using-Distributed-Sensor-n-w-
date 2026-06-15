#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "config.h"

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("[INIT] Booting Receiver Station Gateway...");

    SPI.begin(18, 19, 23, LORA_SS);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(LORA_BAND)) {
        Serial.println("[ERROR] LoRa Gateway Hardware Base Offline!");
        while (1);
    }
    Serial.println("[SUCCESS] Receiver Gateway Active. Monitoring RF channels...");
}

void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String payload = "";
        while (LoRa.available()) {
            payload += (char)LoRa.read();
        }

        Serial.println("\n--------------------------------------------");
        Serial.println("[RX] Raw CSV Incoming: " + payload);

        int idx1 = payload.indexOf(',');
        int idx2 = payload.indexOf(',', idx1 + 1);
        int idx3 = payload.indexOf(',', idx2 + 1);
        int idx4 = payload.indexOf(',', idx3 + 1);

        if (idx1 == -1 || idx2 == -1 || idx3 == -1 || idx4 == -1) {
            Serial.println("[WARN] Frame dropped. Corrupted payload parsing structure.");
            return;
        }

        float parsedCSI        = payload.substring(0, idx1).toFloat();
        float parsedNoise      = payload.substring(idx1 + 1, idx2).toFloat();
        float parsedWeight     = payload.substring(idx2 + 1, idx3).toFloat();
        float parsedVibration  = payload.substring(idx3 + 1, idx4).toFloat();
        float parsedAlertCode  = payload.substring(idx4 + 1).toFloat();

        if (parsedCSI < 0.25) {
            Serial.println("STATUS DISPLAY: [SAFE] LOW DENSITY POPULATION");
        } else if (parsedCSI < 0.50) {
            Serial.println("STATUS DISPLAY: [NORMAL] MODERATE DENSITY POPULATION");
        } else if (parsedCSI < 0.75) {
            Serial.println("STATUS DISPLAY: [WARNING] LOCALIZED CROWD SURGE INITIALIZED");
        } else {
            Serial.println("STATUS DISPLAY: [CRITICAL] HIGH ACCELERATION CRUSH STAMPEDE RISK");
        }

        if (parsedAlertCode > 0.5) {
            Serial.println(">>> BROADCAST SYSTEM ENGINE: !!! EMERGENCY TRIGGER ACTIVATED !!! <<<");
        }
    }
}