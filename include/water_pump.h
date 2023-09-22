#ifndef WATER_PUMP_H
#define WATER_PUMP_H

#include <Arduino.h>
#include <defines.h>

void initWaterPump();
bool isWaterLow();
void startWaterPump();
void stopWaterPump();

#endif  // WATER_PUMP_H