#include <Arduino.h>
#include <defines.h>
#include <state_machine.h>
#include <utils.h>

unsigned long last_time = 0;
unsigned long current_time = 0;

void setup()
{
  Serial.begin(115200);
  g_state_table[g_current_state][g_new_event]();
}

void loop()
{
  delay(10);  // this speeds up the simulation

  current_time = millis();

  if (current_time - last_time > CHECK_TIMEOUT)
  {
    last_time = current_time;
    g_new_event = get_event();

    if (g_new_event != EV_CAPTURED)
    {
      g_state_table[g_current_state][g_new_event]();
      DebugPrintState(g_states_str[g_current_state], g_events_str[g_new_event]);
    }

    g_new_event = EV_CAPTURED;
  }
}
