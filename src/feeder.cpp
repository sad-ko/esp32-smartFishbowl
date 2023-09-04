#include <defines.h>
#include <feeder.h>
#include <utils.h>

// Variable global, es la unica forma de hacerlo funcionar con interrupciones.
Feeder g_feeder;

void IRAM_ATTR feedStartIR() { feedStart(); }

void IRAM_ATTR feedStopIR() { feedStop(); }

void initFeeder(FeederCfg cfg)
{
  // Servo init
  g_feeder.servo.attach(cfg.pin);
  g_feeder.servo.write(CLOSE_DISPENSER);

  // Timer init
  g_feeder.timer = timerBegin(TIMER_FEEDER, TIMER_CLOCK_FREQUENCY, true);
  timerAttachInterrupt(g_feeder.timer, &feedStartIR, true);
// Para probar dura segundos, pero debe pasarse a horas en el producto final.
#if SERIAL_DEBUG_ENABLED
  timerAlarmWrite(g_feeder.timer, cfg.hours * MIC_TO_SEC, true);
#else
  timerAlarmWrite(g_feeder.timer, cfg.hours * MIC_TO_HR, true);
#endif
  timerAlarmEnable(g_feeder.timer);

  // Timer duration init
  g_feeder.duration = timerBegin(TIMER_FEEDER_DURATION, TIMER_CLOCK_FREQUENCY, true);
  timerAttachInterrupt(g_feeder.duration, &feedStopIR, true);
  timerAlarmWrite(g_feeder.duration, cfg.duration * MIC_TO_SEC, true);
  timerAlarmEnable(g_feeder.duration);
  // Se lo detiene hasta que coman los peces.
  timerStop(g_feeder.duration);
  timerRestart(g_feeder.duration);
}

void feedStart()
{
  timerStop(g_feeder.timer);
  // Reiniciamos el timer o el rendimiento comienza a deteriorarse.
  timerRestart(g_feeder.timer);

  DebugPrintln("Time to feed the fishes...");
  g_feeder.servo.write(OPEN_DISPENSER);

  timerStart(g_feeder.duration);
}

void feedStop()
{
  timerStop(g_feeder.duration);
  // Reiniciamos el timer o el rendimiento comienza a deteriorarse.
  timerRestart(g_feeder.duration);

  DebugPrintln("Stop feeding the fishes...");
  g_feeder.servo.write(CLOSE_DISPENSER);

  timerStart(g_feeder.timer);
}
