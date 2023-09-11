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

inline bool isNightTime()
{
  uint16_t analogValue = analogRead(PIN_LDR_AO);
  return analogValue > NIGHT_THRESHOLD;
}

inline void lightsOn() { digitalWrite(PIN_LED, HIGH); }
inline void lightsOff() { digitalWrite(PIN_LED, LOW); }

#endif  // LIGHT_SENSOR_H