#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include <defines.h>
#include <feeder.h>
#include <light_sensor.h>
#include <utils.h>
#include <water_pump.h>

#define MAX_STATES 4
#define MAX_EVENTS 4

enum states
{
  ST_INIT,
  ST_IDLE,
  ST_IDLE_NIGHT,
  ST_FILLING_TANK,
} g_current_state;

String g_states_str[] = {
    "ST_INIT",
    "ST_IDLE",
    "ST_IDLE_NIGHT",
    "ST_FILLING_TANK",
};

enum events
{
  EV_CAPTURED,
  EV_LOW_WATER,
  EV_HIGH_WATER,
  EV_TIME_CHANGED,
} g_new_event;

String g_events_str[] = {
    "EV_CAPTURED",
    "EV_LOW_WATER",
    "EV_HIGH_WATER",
    "EV_TIME_CHANGED",
};

typedef void (*transition)();

void action_init()
{
  initFeeder(FeederCfg{.pin = PIN_SERVO, .hours = 5, .duration = 2});
  initWaterPump();
  initLightSensor();
  DebugPrintState(g_states_str[g_current_state], g_events_str[g_new_event]);
  g_current_state = ST_IDLE;
}

void action_none() {}

void action_error() { DebugPrintln("[Error] - No deberias estar aqui..."); }

void action_pump_start()
{
  DebugPrintState(g_states_str[g_current_state], g_events_str[g_new_event]);
  g_current_state = ST_FILLING_TANK;
  startWaterPump();
}

void action_pump_stop()
{
  DebugPrintState(g_states_str[g_current_state], g_events_str[g_new_event]);
  g_current_state = ST_IDLE;
  stopWaterPump();
}

void action_day_time()
{
  DebugPrintln("Day time!");
  digitalWrite(PIN_LED, LOW);
  g_current_state = ST_IDLE;
}

void action_night_time()
{
  DebugPrintln("Night time!");
  digitalWrite(PIN_LED, HIGH);
  g_current_state = ST_IDLE_NIGHT;
}

transition g_state_table[MAX_STATES][MAX_EVENTS] = {
    {action_init,      action_error,     action_error,      action_error}, // ST_INIT
    {action_none, action_pump_start,      action_none, action_night_time}, // ST_IDLE
    {action_none, action_pump_start,      action_none,   action_day_time}, // ST_IDLE_NIGHT
    {action_none,       action_none, action_pump_stop,       action_none}, // ST_FILLING_TANK

  // EV_CAPTURED,      EV_LOW_WATER,    EV_HIGH_WATER,   EV_TIME_CHANGED
};

enum events get_event()
{
  bool is_low_water = checkWaterLevels();
  if (is_low_water)
  {
    return EV_LOW_WATER;
  }
  else if (!is_low_water && g_current_state == ST_FILLING_TANK)
  {
    return EV_HIGH_WATER;
  }

  bool is_night_time = checkLightLevels();
  if ((is_night_time && g_current_state == ST_IDLE) ||
      (!is_night_time && g_current_state == ST_IDLE_NIGHT))
  {
    return EV_TIME_CHANGED;
  }

  return EV_CAPTURED;
}

#endif  // STATE_MACHINE_H