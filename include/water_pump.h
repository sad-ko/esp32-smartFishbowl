#ifndef WATER_PUMP_H
#define WATER_PUMP_H

#include <Arduino.h>
#include <defines.h>

void initWaterPump();
bool isWaterLow();
inline void startWaterPump() { digitalWrite(PIN_RELAY, HIGH); }
inline void stopWaterPump() { digitalWrite(PIN_RELAY, LOW); }

#endif  // WATER_PUMP_H