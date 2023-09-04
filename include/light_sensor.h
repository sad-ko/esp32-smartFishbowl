#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>
#include <defines.h>
#include <utils.h>

inline void initLightSensor()
{
  pinMode(PIN_LDR_AO, INPUT);
  pinMode(PIN_LED, OUTPUT);
}

inline bool checkLightLevels()
{
  uint16_t analogValue = analogRead(PIN_LDR_AO);
  DebugPrintln(analogValue);
  return analogValue > NIGHT_THRESHOLD;
}

#endif  // LIGHT_SENSOR_H