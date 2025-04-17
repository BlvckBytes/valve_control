#ifndef _H_WEB_HANDLER
#define _H_WEB_HANDLER

#include <Arduino.h>
#include <SD.h>
#include <ESPAsyncWebServer.h>

#include "selection_relays.h"
#include "h_bridge_pwm.h"

// Web Protocol Commands
#define WPC_ENABLE_VALVE_BY_INDEX 0
#define WPC_DISABLE_ACTIVE_VALVE  1
#define WPC_VALVES_STATE_CHANGE   2

namespace WebHandler
{
  bool setup();
}

#endif