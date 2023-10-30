#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>
#include <defines.h>
#include <utils.h>

inline void init_light_sensor()
{
  pinMode(PIN_LDR_AO, INPUT);
  pinMode(PIN_LED, OUTPUT);
}

/// @brief Analiza el nivel de luz para determinar si es de noche o no.
inline bool is_night_time()
{
  // El valor analogico es inverso al nivel de lux, por lo tanto a mayor valor analogico
  // menor nivel de lux.
  uint16_t analogValue = analogRead(PIN_LDR_AO);
  return analogValue < DEFAULT_NIGHT_THRESHOLD;
}

inline void lights_on() { digitalWrite(PIN_LED, HIGH); }
inline void lights_off() { digitalWrite(PIN_LED, LOW); }

#endif  // LIGHT_SENSOR_H