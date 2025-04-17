#include "selection_relays.h"

namespace SelectionRelays
{

  static uint8_t RELAY_PINS[] = { RELAY_1_PIN, RELAY_2_PIN, RELAY_3_PIN, RELAY_4_PIN };
  static uint8_t RELAY_COUNT = sizeof(RELAY_PINS) / sizeof(uint8_t);
  static state_change_callback_t state_change_callback;

  void setup()
  {
    for (uint8_t relay_pin : RELAY_PINS)
    {
      pinMode(relay_pin, OUTPUT);
      digitalWrite(relay_pin, LOW);
    }

    #ifdef DEBUG_PRINTS
    Serial.println("Successfully configured section-relay outputs");
    #endif
  }

  void deactivate_all()
  {
    bool caused_change = false;

    for (uint8_t relay_pin : RELAY_PINS)
    {
      if (digitalRead(relay_pin) == LOW)
        continue;

      digitalWrite(relay_pin, LOW);
      caused_change = true;
    }

    if (caused_change && state_change_callback)
      state_change_callback();
  }

  bool activate_by_index(uint8_t index)
  {
    if (index >= RELAY_COUNT)
      return false;

    for (uint8_t relay_index = 0; relay_index < RELAY_COUNT; ++relay_index)
    {
      digitalWrite(RELAY_PINS[relay_index], relay_index == index);
    }

    if (state_change_callback)
      state_change_callback();

    return true;
  }

  uint8_t get_relay_count()
  {
    return RELAY_COUNT;
  }

  int get_relay_state(uint8_t index)
  {
    if (index >= RELAY_COUNT)
      return -1;

    return digitalRead(RELAY_PINS[index]);
  }
  
  void set_state_change_callback(state_change_callback_t callback)
  {
    state_change_callback = callback;
  }
}