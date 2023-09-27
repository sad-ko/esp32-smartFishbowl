#include <defines.h>
#include <feeder.h>
#include <light_sensor.h>
#include <state_machine.h>
#include <water_pump.h>

enum states g_current_state = ST_INIT;
enum events g_new_event = EV_CAPTURED;

const String g_states_str[] = {
    "ST_INIT",         "ST_IDLE",          "ST_IDLE_NIGHT",
    "ST_LOW_ON_WATER", "ST_DRAWING_WATER", "ST_FEEDING_FISHES",
};

const String g_events_str[] = {
    "EV_CAPTURED",     "EV_FISHES_FED",    "EV_DIRTY_WATER",   "EV_TANK_FILLED",
    "EV_TIME_CHANGED", "EV_MINIMUN_WATER", "EV_HUNGRY_FISHES",
};

// clang-format off

///@brief Tabla de estados
const transition g_state_table[MAX_STATES][MAX_EVENTS] = {
    {action_init,        action_error,      action_error,      action_error,      action_error,     action_error,         action_error}, // ST_INIT
    {action_none,         action_none, action_draw_start,       action_none, action_night_time, action_tank_stop, action_feeding_start}, // ST_IDLE
    {action_none,         action_none,       action_none,       action_none,   action_day_time, action_tank_stop,          action_none}, // ST_IDLE_NIGHT
    {action_none,         action_none,       action_none, action_tank_start,       action_none, action_tank_stop,          action_none}, // ST_LOW_ON_WATER
    {action_none,         action_none,       action_none,       action_none,       action_none, action_draw_stop,          action_none}, // ST_DRAWING_WATER
    {action_none, action_feeding_stop,       action_none,       action_none,       action_none, action_tank_stop,          action_none}, // ST_FEEDING_FISHES

  // EV_CAPTURED,       EV_FISHES_FED,    EV_DIRTY_WATER,    EV_TANK_FILLED,   EV_TIME_CHANGED, EV_MINIMUN_WATER,     EV_HUNGRY_FISHES
};
// clang-format on

// --- Events --- //
#define MAX_EVENTS_TO_CHECK 2

typedef bool (*checkEvent)();

bool check_water();
bool check_time();

checkEvent events_to_check[MAX_EVENTS_TO_CHECK] = {check_water, check_time};

unsigned long current_time = 0;
unsigned long last_time = 0;
short current_index = 0;
short last_index = 0;

bool check_water()
{
  // Verificamos si esta o no en el estado ST_LOW_ON_WATER para evitar el constante
  // estimulo de estos eventos.

  if (is_water_low() && g_current_state != ST_LOW_ON_WATER)
  {
    g_new_event = EV_MINIMUN_WATER;
    return true;
  }

  if (is_water_high() && g_current_state == ST_LOW_ON_WATER)
  {
    g_new_event = EV_TANK_FILLED;
    return true;
  }

  return false;
}

bool check_time()
{
  bool time = is_night_time();
  if ((time && g_current_state == ST_IDLE) || (!time && g_current_state == ST_IDLE_NIGHT))
  {
    g_new_event = EV_TIME_CHANGED;
    return true;
  }

  return false;
}

void get_event()
{
  current_time = millis();

  if (current_time - last_time > CHECK_TIMEOUT)
  {
    last_time = current_time;
    current_index = last_index % MAX_EVENTS_TO_CHECK;
    last_index++;

    events_to_check[current_index]();
  }
}

// -- Actions -- //
void action_init()
{
  DebugPrintState(g_states_str[g_current_state], g_events_str[g_new_event]);
  init_light_sensor();
  init_water_pump(DEFAULT_DIRTY_WATER_TIME, DEFAULT_MIN_WATER_DISTANCE,
                  DEFAULT_MAX_WATER_DISTANCE);
  init_feeder(FeederCfg{.pin = PIN_SERVO,
                        .hours = DEFAULT_FEEDER_TIME,
                        .duration = DEFAULT_FEEDER_DURATION});
  g_current_state = ST_IDLE;
}

void action_draw_start()
{
  start_water_pump();
  g_current_state = ST_DRAWING_WATER;
}

void action_draw_stop()
{
  stop_water_pump();
  g_current_state = ST_IDLE;
}

void action_day_time()
{
  lights_off();
  g_current_state = ST_IDLE;
}

void action_night_time()
{
  lights_on();
  g_current_state = ST_IDLE_NIGHT;
}

void action_feeding_start()
{
  feed_start();
  g_current_state = ST_FEEDING_FISHES;
}

void action_feeding_stop()
{
  feed_stop();
  g_current_state = ST_IDLE;
}

void action_tank_start() { g_current_state = ST_IDLE; }

void action_tank_stop()
{
  DebugPrintln("[Warning] - Niveles de agua muy bajo, funcionalidades detenidas.");
  g_current_state = ST_LOW_ON_WATER;
}

// -- State Machine -- //
void state_machine()
{
  get_event();

  if (g_new_event != EV_CAPTURED)
  {
    DebugPrintState(g_states_str[g_current_state], g_events_str[g_new_event]);
    g_state_table[g_current_state][g_new_event]();
    g_new_event = EV_CAPTURED;
    DebugPrintState(g_states_str[g_current_state], g_events_str[g_new_event]);
  }
}