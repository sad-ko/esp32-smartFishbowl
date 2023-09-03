#ifndef DEFINES_H
#define DEFINES_H

#define SERIAL_DEBUG_ENABLED 1

// Tiempo macros
#define CHECK_TIMEOUT 2000
#define MIC_TO_SEC 1000000UL
#define MIC_TO_HR (MIC_TO_SEC * 3600UL)
#define TIMER_FEEDER 0
#define TIMER_FEEDER_DURATION 1
#define TIMER_CLOCK_FREQUENCY 80
#define ULTRASOUND_DURATION 10

// Para probar ponemos el timeout igual al delay del simulador.
#if SERIAL_DEBUG_ENABLED
#define ULTRASOUND_TIMEOUT 100000
#else
#define ULTRASOUND_TIMEOUT (ULTRASOUND_DURATION * 2)
#endif

// Distancias macros
#define PULSE_TO_CM(pulse) (float)(pulse / 58)
#define MAX_WATER_DISTANCE 50  // A testear

// Pines macros
#define PIN_LED 21
#define PIN_SERVO 5
#define PIN_RELAY 32
#define PIN_LDR_AO 13
#define PIN_ULTRASONIDO_TRIG 22
#define PIN_ULTRASONIDO_ECHO 23

#endif  // DEFINES_H