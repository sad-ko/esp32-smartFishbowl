#ifndef FEEDER_H
#define FEEDER_H

#include <ESP32Servo.h>

#define OPEN_DISPENSER 90
#define CLOSE_DISPENSER 0

/// @brief Alimentador para peces, es un servo que gira la lata de comida
/// cada vez que el timer genera una interrupcion por hardware
/// (Se genera una segunda interrupcion para volver a su lugar la lata despues
/// X segundos).
typedef struct
{
  Servo servo;
  /// @brief Tiempo para darle de comer a los peces.
  hw_timer_t *timer;
  /// @brief Tiempo que se les da de comer a los peces.
  hw_timer_t *duration;
} Feeder;

/// @brief Configuracion del alimentador para peces.
typedef struct
{
  /// @brief Pin del Servo conectado.
  int pin;
  /// @brief Cada cuantas horas comen los peces.
  uint8_t hours;
  /// @brief Cuantos segundos debe estar abierto el dispenser.
  uint8_t duration;
} FeederCfg;

/// @brief Inicializa el servo en 0 grados y los timers para comenzar/detenerse
/// de darle de comer a los peces.
/// @param cfg: Las configuraciones del alimentador para peces.
void initFeeder(FeederCfg cfg);

/// @brief Comienza a darle de comer a los peces.
void feedStart();

/// @brief Detiene de darle de comer a los peces.
void feedStop();

#endif  // FEEDER_H