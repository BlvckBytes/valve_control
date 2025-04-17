#include "h_bridge_pwm.h"

namespace HBridgePWM
{
  bool setup()
  {
    mcpwm_config_t mcpwm_config = {
      .frequency = 50,
      .cmpr_a = 50,
      .cmpr_b = 50,
      .duty_mode = MCPWM_DUTY_MODE_0,
      .counter_mode = MCPWM_UP_COUNTER
    };

    if (mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &mcpwm_config) != ESP_OK)
    {
      #ifdef DEBUG_PRINTS
      Serial.println("Could not configure PWM wave-form");
      #endif
      return false;
    }
    

    if (mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, DEAD_TIME_US * 10, DEAD_TIME_US * 10) != ESP_OK)
    {
      #ifdef DEBUG_PRINTS
      Serial.println("Could not configure PWM dead-time");
      #endif
      return false;
    }

    if (mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PWM_PIN_A) != ESP_OK)
    {
      #ifdef DEBUG_PRINTS
      Serial.println("Could not configure PWM pin A");
      #endif
      return false;
    }

    if (mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, PWM_PIN_B) != ESP_OK)
    {
      #ifdef DEBUG_PRINTS
      Serial.println("Could not configure PWM pin B");
      #endif
      return false;
    }

    if (mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0) != ESP_OK)
    {
      #ifdef DEBUG_PRINTS
      Serial.println("Could not enable PWM");
      #endif
      return false;
    }

    pinMode(H_BRIDGE_ENABLE_PIN, OUTPUT);
    digitalWrite(H_BRIDGE_ENABLE_PIN, LOW);

    #ifdef DEBUG_PRINTS
    Serial.println("Successfully configured PWM");
    #endif
    return true;
  }

  void enable_output()
  {
    digitalWrite(H_BRIDGE_ENABLE_PIN, HIGH);
  }

  void disable_output()
  {
    digitalWrite(H_BRIDGE_ENABLE_PIN, LOW);
  }
}