#ifndef _H_SELECTION_RELAYS
#define _H_SELECTION_RELAYS

#include <Arduino.h>

#define RELAY_1_PIN 33
#define RELAY_2_PIN 25
#define RELAY_3_PIN 26
#define RELAY_4_PIN 27

namespace SelectionRelays
{

  typedef void (*state_change_callback_t)(void);

  void setup();

  void deactivate_all();

  bool activate_by_index(uint8_t index);

  int get_relay_state(uint8_t index);

  uint8_t get_relay_count();

  void set_state_change_callback(state_change_callback_t callback);
}

#endif