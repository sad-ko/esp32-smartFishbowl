#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include <utils.h>

#define MAX_STATES 6
#define MAX_EVENTS 7

/// @brief Estados de la pecera.
enum states
{
  ST_INIT,
  ST_IDLE,
  ST_IDLE_NIGHT,
  ST_LOW_ON_WATER,
  ST_DRAWING_WATER,
  ST_FEEDING_FISHES,
};

/// @brief Eventos de la pecera.
enum events
{
  EV_CAPTURED,
  EV_FISHES_FED,
  EV_DIRTY_WATER,
  EV_TANK_FILLED,
  EV_TIME_CHANGED,
  EV_MINIMUN_WATER,
  EV_HUNGRY_FISHES,
};

typedef void (*transition)();

inline void action_none() {}
inline void action_error() { DebugPrintln("[Error] - No deberias estar aqui..."); }

void action_init();
void action_draw_start();
void action_draw_stop();
void action_day_time();
void action_night_time();
void action_feeding_start();
void action_feeding_stop();
void action_tank_start();
void action_tank_stop();

void state_machine();

extern enum states g_current_state;
extern enum events g_new_event;
extern const String g_states_str[];
extern const String g_events_str[];
extern const transition g_state_table[MAX_STATES][MAX_EVENTS];

#endif  // STATE_MACHINE_H