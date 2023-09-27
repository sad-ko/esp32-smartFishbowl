#include <defines.h>
#include <feeder.h>
#include <state_machine.h>
#include <utils.h>

Feeder g_feeder;

// Se podria mejorar poniendo el evento en una cola para evitar que lo pise otro evento.
void IRAM_ATTR feedStartIR() { g_new_event = EV_HUNGRY_FISHES; }
void IRAM_ATTR feedStopIR() { g_new_event = EV_FISHES_FED; }

void init_feeder(FeederCfg cfg)
{
  g_feeder.servo.attach(cfg.pin);
  g_feeder.servo.write(CLOSE_DISPENSER);

  g_feeder.timer = timerBegin(TIMER_FEEDER, TIMER_CLOCK_FREQUENCY, true);
  timerAttachInterrupt(g_feeder.timer, &feedStartIR, true);
#if DEBUG_MODE
  timerAlarmWrite(g_feeder.timer, cfg.hours * MIC_TO_SEC, true);
#else
  timerAlarmWrite(g_feeder.timer, cfg.hours * MIC_TO_HR, true);
#endif
  timerAlarmEnable(g_feeder.timer);

  g_feeder.duration = timerBegin(TIMER_FEEDER_DURATION, TIMER_CLOCK_FREQUENCY, true);
  timerAttachInterrupt(g_feeder.duration, &feedStopIR, true);
  timerAlarmWrite(g_feeder.duration, cfg.duration * MIC_TO_SEC, true);
  timerAlarmEnable(g_feeder.duration);

  // Se lo detiene hasta que coman los peces.
  timerStop(g_feeder.duration);
  timerRestart(g_feeder.duration);
}

void feed_start()
{
  timerStop(g_feeder.timer);
  // Reiniciamos el timer o el rendimiento comienza a deteriorarse.
  timerRestart(g_feeder.timer);
  g_feeder.servo.write(OPEN_DISPENSER);
  timerStart(g_feeder.duration);
}

void feed_stop()
{
  timerStop(g_feeder.duration);
  // Reiniciamos el timer o el rendimiento comienza a deteriorarse.
  timerRestart(g_feeder.duration);
  g_feeder.servo.write(CLOSE_DISPENSER);
  timerStart(g_feeder.timer);
}
