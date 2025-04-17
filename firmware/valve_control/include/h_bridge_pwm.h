#ifndef _H_BRIDGE_PWM
#define _H_BRIDGE_PWM

#include <driver/mcpwm.h>
#include <Arduino.h>

#define PWM_PIN_A 16
#define PWM_PIN_B 17
#define PWM_FREQUENCY_HZ 50
#define DEAD_TIME_US 300
#define H_BRIDGE_ENABLE_PIN 4

namespace HBridgePWM
{
  bool setup();

  void enable_output();

  void disable_output();
}

#endif