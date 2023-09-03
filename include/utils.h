#ifndef UTILS_H
#define UTILS_H

#if SERIAL_DEBUG_ENABLED
#define DebugPrint(str) \
  {                     \
    Serial.print(str);  \
  }
#define DebugPrintln(str) \
  {                       \
    Serial.println(str);  \
  }
#define DebugPrintState(state, event)                              \
  {                                                                \
    String est = state;                                            \
    String evt = event;                                            \
    String str;                                                    \
    str = "-----------------------------------------------------"; \
    DebugPrintln(str);                                             \
    str = "State: [" + est + "] - " + "Event: [" + evt + "].";     \
    DebugPrintln(str);                                             \
    str = "-----------------------------------------------------"; \
    DebugPrintln(str);                                             \
  }
#else
#define DebugPrint(str)
#define DebugPrintln(str)
#define DebugPrintState(state, event)
#endif

#endif  // UTILS_H