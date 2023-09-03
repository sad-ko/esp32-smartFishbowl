#include <Arduino.h>
#include <defines.h>
#include <utils.h>
#include <water_pump.h>

void initWaterPump()
{
  pinMode(PIN_ULTRASONIDO_ECHO, INPUT);
  pinMode(PIN_ULTRASONIDO_TRIG, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
}

bool checkWaterLevels()
{
  digitalWrite(PIN_ULTRASONIDO_TRIG, HIGH);
  delayMicroseconds(ULTRASOUND_DURATION);
  digitalWrite(PIN_ULTRASONIDO_TRIG, LOW);

  size_t duration = pulseIn(PIN_ULTRASONIDO_ECHO, HIGH, ULTRASOUND_TIMEOUT);
  float distance = PULSE_TO_CM(duration);

  DebugPrint("Water is ");
  DebugPrint(distance);
  DebugPrintln(" centimetes away.");

  return distance > MAX_WATER_DISTANCE;
}

void startWaterPump() { digitalWrite(PIN_RELAY, HIGH); }

void stopWaterPump() { digitalWrite(PIN_RELAY, LOW); }
