#include <state_machine.h>
#include <utils.h>
#include <water_pump.h>

hw_timer_t *g_dirty_water_t;

// No me parece que sea la mejor forma de pasar a estos eventos, podrian ser pisados
// por otro evento en cuanto vuelva de la interrupcion.
void IRAM_ATTR pumpStartIR() { g_new_event = EV_DIRTY_WATER; }

void initWaterPump()
{
  pinMode(PIN_ULTRASONIDO_ECHO, INPUT);
  pinMode(PIN_ULTRASONIDO_TRIG, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);

  // Timer duration init
  g_dirty_water_t = timerBegin(TIMER_WATER_CLEAN, TIMER_CLOCK_FREQUENCY, true);
  timerAttachInterrupt(g_dirty_water_t, &pumpStartIR, true);

// Para probar dura segundos, pero debe pasarse a horas en el producto final.
#if DEBUG_MODE
  timerAlarmWrite(g_dirty_water_t, DEFAULT_DIRTY_WATER_TIME * MIC_TO_SEC, true);
#else
  timerAlarmWrite(g_dirty_water_t, DEFAULT_DIRTY_WATER_TIME * MIC_TO_HR, true);
#endif

  timerAlarmEnable(g_dirty_water_t);
}

bool isWaterLow()
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

void startWaterPump()
{
  timerStop(g_dirty_water_t);
  timerRestart(g_dirty_water_t);
  digitalWrite(PIN_RELAY, HIGH);
}

void stopWaterPump()
{
  digitalWrite(PIN_RELAY, LOW);
  timerStart(g_dirty_water_t);
}