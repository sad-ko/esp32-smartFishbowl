#include <defines.h>
#include <feeder.h>
#include <light_sensor.h>
#include <mqtt.h>
#include <state_machine.h>
#include <water_pump.h>

enum states g_current_state = ST_INIT;
enum events g_new_event = EV_CAPTURED;
enum states g_current_idle = ST_IDLE;
bool lights_status = false;

const String g_states_str[] = {
    "ST_INIT",         "ST_IDLE",          "ST_IDLE_NIGHT",     "ST_IDLE_MANUAL",
    "ST_LOW_ON_WATER", "ST_DRAWING_WATER", "ST_FEEDING_FISHES",
};

const String g_events_str[] = {"EV_CAPTURED",         "EV_FISHES_FED",
                               "EV_DIRTY_WATER",      "EV_TANK_FILLED",
                               "EV_TIME_CHANGED",     "EV_MINIMUN_WATER",
                               "EV_HUNGRY_FISHES",    "EV_MQTT_DRAW_WATER",
                               "EV_MQTT_FEED_FISHES", "EV_MQTT_SWITCH_LIGHTS"};

// clang-format off

///@brief Tabla de estados
const transition g_state_table[MAX_STATES][MAX_EVENTS] = {
  {action_init,        action_error,      action_error,      action_error,      action_error,     action_error,         action_error,       action_error,          action_error,         action_error}, // ST_INIT
  {action_none,         action_none, action_draw_start,       action_none, action_night_time, action_tank_stop, action_feeding_start,  action_draw_start,  action_feeding_start,     action_lights_on}, // ST_IDLE
  {action_none,         action_none,       action_none,       action_none,   action_day_time, action_tank_stop,          action_none,  action_draw_start,  action_feeding_start,    action_lights_off}, // ST_IDLE_NIGHT
  {action_none,         action_none, action_draw_start,       action_none, action_night_time, action_tank_stop, action_feeding_start,  action_draw_start,  action_feeding_start,    action_lights_off}, // ST_IDLE_MANUAL
  {action_none,         action_none,       action_none, action_tank_start,       action_none, action_tank_stop,          action_none,  action_draw_start,  action_feeding_start, action_lights_switch}, // ST_LOW_ON_WATER
  {action_none,         action_none,       action_none,       action_none,       action_none, action_draw_stop,          action_none,  action_draw_start,  action_feeding_start, action_lights_switch}, // ST_DRAWING_WATER
  {action_none, action_feeding_stop,       action_none,       action_none,       action_none, action_tank_stop,          action_none,  action_draw_start,  action_feeding_start, action_lights_switch}, // ST_FEEDING_FISHES

  // EV_CAPTURED,     EV_FISHES_FED,    EV_DIRTY_WATER,    EV_TANK_FILLED,   EV_TIME_CHANGED, EV_MINIMUN_WATER,     EV_HUNGRY_FISHES, EV_MQTT_DRAW_WATER,   EV_MQTT_FEED_FISHES, EV_MQTT_SWITCH_LIGHTS
};
// clang-format on

// -- Events -- //
#define MAX_EVENTS_TO_CHECK 3

typedef bool (*checkEvent)();

bool check_water();
bool check_time();
bool check_mqtt();

checkEvent events_to_check[MAX_EVENTS_TO_CHECK] = {check_water, check_time, check_mqtt};

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

bool check_mqtt()
{
  return xQueueReceive(queue_to_esp, &(g_new_event), (TickType_t)QUEUE_NO_WAIT);
}

void get_event()
{
  current_time = millis();

  if (current_time - last_time > CHECK_TIMEOUT)
  {
    if (g_current_state == ST_DRAWING_WATER)
    {
      check_water();
      return;
    }

    last_time = current_time;
    current_index = last_index % MAX_EVENTS_TO_CHECK;
    last_index++;

    events_to_check[current_index]();
  }
}

void change_state(enum states new_state)
{
  g_current_state = new_state;
  xQueueSend(queue_to_mqtt, (void *)&new_state, (TickType_t)QUEUE_NO_WAIT);
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
  init_mqtt_communication();
  change_state(g_current_idle);
}

void action_draw_start()
{
  start_water_pump();
  change_state(ST_DRAWING_WATER);
}

void action_draw_stop()
{
  stop_water_pump();
  change_state(ST_IDLE);
}

void action_day_time()
{
  lights_off();
  change_state(g_current_idle);
}

void action_night_time()
{
  lights_on();
  change_state(ST_IDLE_NIGHT);
}

void action_feeding_start()
{
  feed_start();
  change_state(ST_FEEDING_FISHES);
}

void action_feeding_stop()
{
  feed_stop();
  change_state(g_current_idle);
}

void action_tank_start() { change_state(g_current_idle); }

void action_tank_stop()
{
  DebugPrintln("[Warning] - Niveles de agua muy bajo, funcionalidades detenidas.");
  change_state(ST_LOW_ON_WATER);
}

void action_lights_on()
{
  lights_on();
  lights_status = true;
  g_current_idle = ST_IDLE_MANUAL;
  if (g_current_state == ST_IDLE)
  {
    change_state(ST_IDLE_MANUAL);
  }
}

void action_lights_off()
{
  lights_off();
  lights_status = false;
  g_current_idle = ST_IDLE;
  if (g_current_state == ST_IDLE_MANUAL)
  {
    change_state(ST_IDLE);
  }
}

void action_lights_switch()
{
  if (lights_status)
  {
    action_lights_off();
  }
  else
  {
    action_lights_on();
  }
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
