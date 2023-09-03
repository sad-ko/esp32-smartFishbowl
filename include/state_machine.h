#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include <defines.h>
#include <feeder.h>
#include <utils.h>
#include <water_pump.h>

#define MAX_STATES 3
#define MAX_EVENTS 3

enum states
{
  ST_INIT,
  ST_IDLE,
  ST_FILLING_TANK,
} g_current_state;

String g_states_str[] = {
    "ST_INIT",
    "ST_IDLE",
    "ST_FILLING_TANK",
};

enum events
{
  EV_CAPTURED,
  EV_LOW_WATER,
  EV_HIGH_WATER,
} g_new_event;

String g_events_str[] = {
    "EV_CAPTURED",
    "EV_LOW_WATER",
    "EV_HIGH_WATER",
};

typedef void (*transition)();

void action_init()
{
  initFeeder(FeederCfg{.pin = PIN_SERVO, .hours = 5, .duration = 2});
  initWaterPump();
  DebugPrintState(g_states_str[g_current_state], g_events_str[g_new_event]);
  g_current_state = ST_IDLE;
}

void action_error() { DebugPrintln("[Error] - No deberias estar aqui..."); }

void action_none() {}

void action_pump()
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

transition g_state_table[MAX_STATES][MAX_EVENTS] = {
    {action_init, action_error, action_error},     // ST_INIT
    {action_none, action_pump, action_none},       // ST_IDLE
    {action_none, action_none, action_pump_stop},  // ST_FILLING_TANK

    // EV_CAPTURED, EV_LOW_WATER, EV_HIGH_WATER
};

enum events get_event()
{
  if (checkWaterLevels())
  {
    return (g_current_state != ST_FILLING_TANK) ? EV_LOW_WATER : EV_CAPTURED;
  }

  return (g_current_state != ST_FILLING_TANK) ? EV_CAPTURED : EV_HIGH_WATER;
}

#endif  // STATE_MACHINE_H