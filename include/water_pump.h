#ifndef WATER_PUMP_H
#define WATER_PUMP_H

#include <Arduino.h>
#include <defines.h>

typedef struct
{
  hw_timer_t *drain_water_timer;
  /// @brief Nivel minimo aceptable del agua.
  uint8_t min_water_level;
  /// @brief Nivel maximo aceptable del agua.
  uint8_t max_water_level;
} WaterPump;

void init_water_pump(uint8_t drain_water_time, uint8_t min_water_level,
                     uint8_t max_water_level);

/// @brief Analiza el nivel del agua para determinar si supero el minimo nivel permitido.
bool is_water_low();

/// @brief Analiza el nivel del agua para determinar si llego a un nivel de agua deseable
/// para poder continuar con las funcionalidades de la pecera.
bool is_water_high();

void start_water_pump();
void stop_water_pump();

#endif  // WATER_PUMP_H