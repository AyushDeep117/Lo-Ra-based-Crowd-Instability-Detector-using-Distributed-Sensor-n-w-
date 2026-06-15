#include "lora_comm.h"
#include "config.h"
#include <SPI.h>
#include <LoRa.h>

void initLoRa() {
    SPI.begin(18, 19, 23, LORA_SS);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(LORA_BAND)) {
        Serial.println("[CRITICAL] LoRa Tranceiver Radio init failure! Check wiring.");
        while (1);
    }
    Serial.println("[SUCCESS] Sub-GHz LoRa RF Front-End Initialized.");
}

void transmitTelemetry(float cii, float noise, float weight, float vibe, float alertStatus) {
    // Structured Tokenized Data String Output Payload Array for error-free parsing on the server
    String csvPacket = String(cii, 3) + "," + 
                       String(noise, 3) + "," + 
                       String(weight, 3) + "," + 
                       String(vibe, 3) + "," + 
                       String(alertStatus, 0);

    LoRa.beginPacket();
    LoRa.print(csvPacket);
    LoRa.endPacket();
    Serial.println("[TX LoRa] Packet Broadcasted -> " + csvPacket);
}