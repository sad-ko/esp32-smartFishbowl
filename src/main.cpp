#include <Arduino.h>
#include <defines.h>
#include <state_machine.h>

void setup()
{
  Serial.begin(9600);
  g_state_table[g_current_state][g_new_event]();
}

void loop()
{
#if DEBUG_MODE
  delay(10);  // Acelera la simulacion
#endif

  state_machine();
}
