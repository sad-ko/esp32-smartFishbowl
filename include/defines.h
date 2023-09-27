#ifndef DEFINES_H
#define DEFINES_H

// Debug macros
#define DEBUG_MODE 1
#define SERIAL_DEBUG_ENABLED 1

// Tiempo macros
#define MS_TO_SEC 1000
#define MIC_TO_SEC 1000000UL
#define MIC_TO_HR (MIC_TO_SEC * 3600UL)
#define CHECK_TIMEOUT (1 * MS_TO_SEC)
#define ULTRASOUND_DURATION 10  // En ms

// Tiempo timers
#define TIMER_FEEDER 0
#define TIMER_FEEDER_DURATION 1
#define TIMER_DRAIN_WATER 2
#define TIMER_CLOCK_FREQUENCY 80

// Distancias macros
#define PULSE_TO_CM(pulse) (float)(pulse / 58)

// Pines macros
#define PIN_LED 21
#define PIN_SERVO 5
#define PIN_RELAY 32
#define PIN_LDR_AO 34
#define PIN_ULTRASONIDO_TRIG 22
#define PIN_ULTRASONIDO_ECHO 23

// Defaults macro
#define DEFAULT_FEEDER_TIME 20         // En horas
#define DEFAULT_FEEDER_DURATION 5      // En segundos
#define DEFAULT_DIRTY_WATER_TIME 7     // En horas
#define DEFAULT_MIN_WATER_DISTANCE 50  // En cm
#define DEFAULT_MAX_WATER_DISTANCE 20  // En cm
#define DEFAULT_NIGHT_THRESHOLD 800  // Aprox. 200 lux (https://en.wikipedia.org/wiki/Lux#Illuminance)

#endif  // DEFINES_H