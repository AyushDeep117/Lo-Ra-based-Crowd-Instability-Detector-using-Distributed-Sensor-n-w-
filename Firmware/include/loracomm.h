#ifndef LORA_COMM_H
#define LORA_COMM_H

#include <Arduino.h>

void initLoRa();
void transmitTelemetry(float cii, float noise, float weight, float vibe, float alertStatus);

#endif