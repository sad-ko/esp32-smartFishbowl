#include <defines.h>
#include <feeder.h>
#include <light_sensor.h>
#include <state_machine.h>
#include <water_pump.h>

enum states g_current_state = ST_INIT;
enum events g_new_event = EV_CAPTURED;

const String g_states_str[] = {
    "ST_INIT",         "ST_IDLE",          "ST_IDLE_NIGHT",
    "ST_FILLING_TANK", "ST_DRAWING_WATER", "ST_FEEDING_FISHES",
};

const String g_events_str[] = {
    "EV_CAPTURED",    "EV_TANK_FULL",    "EV_FISHES_FED",    "EV_DIRTY_WATER",
    "EV_CLEAN_WATER", "EV_TIME_CHANGED", "EV_MINIMUN_WATER", "EV_HUNGRY_FISHES",
};

// Desactivamos el formatter temporalmente para mantener el formato de la tabla legible.
// clang-format off
const transition g_state_table[MAX_STATES][MAX_EVENTS] = {
    {action_init,     action_error,        action_error,      action_error,      action_error,      action_error,     action_error,         action_error}, // ST_INIT
    {action_none,      action_none,         action_none, action_draw_start, action_pump_start, action_night_time,      action_none, action_feeding_start}, // ST_IDLE
    {action_none,      action_none,         action_none,       action_none,       action_none,   action_day_time,      action_none,          action_none}, // ST_IDLE_NIGHT
    {action_none, action_pump_stop,         action_none,       action_none,       action_none,       action_none,      action_none,          action_none}, // ST_FILLING_TANK
    {action_none,      action_none,         action_none,       action_none,       action_none,       action_none, action_draw_stop,          action_none}, // ST_DRAWING_WATER
    {action_none,      action_none, action_feeding_stop,       action_none,       action_none,       action_none,      action_none,          action_none}, // ST_FEEDING_FISHES

  // EV_CAPTURED,     EV_TANK_FULL,       EV_FISHES_FED,    EV_DIRTY_WATER,    EV_CLEAN_WATER,   EV_TIME_CHANGED, EV_MINIMUN_WATER,     EV_HUNGRY_FISHES
};
// clang-format on

void action_init()
{
  DebugPrintState(g_states_str[g_current_state], g_events_str[g_new_event]);
  initFeeder(FeederCfg{.pin = PIN_SERVO,
                       .hours = DEFAULT_FEEDER_TIME,
                       .duration = DEFAULT_FEEDER_DURATION});
  initWaterPump();
  initLightSensor();
  g_current_state = ST_IDLE;
}

void action_pump_start()
{
  startWaterPump();
  g_current_state = ST_FILLING_TANK;
}

void action_pump_stop()
{
  stopWaterPump();
  g_current_state = ST_IDLE;
}

void action_draw_start() {}
void action_draw_stop() {}

void action_day_time()
{
  lightsOff();
  g_current_state = ST_IDLE;
}

void action_night_time()
{
  lightsOn();
  g_current_state = ST_IDLE_NIGHT;
}

void action_feeding_start()
{
  feedStart();
  g_current_state = ST_FEEDING_FISHES;
}

void action_feeding_stop()
{
  feedStop();
  g_current_state = ST_IDLE;
}

void get_event()
{
  switch (g_current_state)
  {
    case ST_IDLE:
      if (isNightTime())
      {
        g_new_event = EV_TIME_CHANGED;
      }
      break;

    case ST_IDLE_NIGHT:
      if (!isNightTime())
      {
        g_new_event = EV_TIME_CHANGED;
      }
      break;

    case ST_DRAWING_WATER:
      if (isWaterLow())
      {
        g_new_event = EV_MINIMUN_WATER;
      }
      break;

    case ST_FILLING_TANK:
      if (!isWaterLow())
      {
        g_new_event = EV_TANK_FULL;
      }
      break;

    default:
      break;
  }
}
