#include <state_machine.h>
#include <utils.h>
#include <water_pump.h>

WaterPump g_water_pump;

// Se podria mejorar poniendo el evento en una cola para evitar que lo pise otro evento.
void IRAM_ATTR pumpStartIR() { g_new_event = EV_DIRTY_WATER; }

void init_water_pump(uint8_t drain_water_time, uint8_t min_water_level,
                     uint8_t max_water_level)
{
  pinMode(PIN_ULTRASONIDO_ECHO, INPUT);
  pinMode(PIN_ULTRASONIDO_TRIG, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);

  g_water_pump.drain_water_timer =
      timerBegin(TIMER_DRAIN_WATER, TIMER_CLOCK_FREQUENCY, true);
  timerAttachInterrupt(g_water_pump.drain_water_timer, &pumpStartIR, true);
#if DEBUG_MODE
  timerAlarmWrite(g_water_pump.drain_water_timer, drain_water_time * MIC_TO_MIN, true);
#else
  timerAlarmWrite(g_water_pump.drain_water_timer, drain_water_time * MIC_TO_HR, true);
#endif
  timerAlarmEnable(g_water_pump.drain_water_timer);

  g_water_pump.min_water_level = min_water_level;
  g_water_pump.max_water_level = max_water_level;
}

bool is_water_low()
{
  digitalWrite(PIN_ULTRASONIDO_TRIG, HIGH);
  delayMicroseconds(ULTRASOUND_DURATION);
  digitalWrite(PIN_ULTRASONIDO_TRIG, LOW);

  size_t duration = pulseIn(PIN_ULTRASONIDO_ECHO, HIGH);
  float distance = PULSE_TO_CM(duration);

  return distance > g_water_pump.min_water_level;
}

bool is_water_high()
{
  digitalWrite(PIN_ULTRASONIDO_TRIG, HIGH);
  delayMicroseconds(ULTRASOUND_DURATION);
  digitalWrite(PIN_ULTRASONIDO_TRIG, LOW);

  size_t duration = pulseIn(PIN_ULTRASONIDO_ECHO, HIGH);
  float distance = PULSE_TO_CM(duration);

  return distance < g_water_pump.max_water_level;
}

void start_water_pump()
{
  timerStop(g_water_pump.drain_water_timer);
  timerRestart(g_water_pump.drain_water_timer);
  digitalWrite(PIN_RELAY, HIGH);
}

void stop_water_pump()
{
  digitalWrite(PIN_RELAY, LOW);
  timerStart(g_water_pump.drain_water_timer);
}