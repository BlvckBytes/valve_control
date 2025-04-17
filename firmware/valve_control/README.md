# valve_control

## Requirements

- Control a total of four individual solenoid-valves
  - They operate on 24VAC - drive them using a H-bridge on 12V
  - Four relays direct the square-wave to the target valve
  - Only one valve may be active at any given time
- Status-LEDs as a means to quickly debug
  - 3V3 power present
  - Valve 1/2/3/4 active
  - H-bridge enable
  - MCU status
    - fast blink: connecting to WiFi
    - slow blink: connected to WiFi
    - on: booting (pre-WiFi setup)
- Control and configure using a simple, locally hosted website
  - Serve static files from an SD-card
  - Supply data and receive commands via web-sockets
  - Enable a valve manually with a provided duration
  - Enable/disable the scheduler (disabling cancels the current task)
  - Configure schedule slots - weekday, start-time, duration
  - Request current valve status-array
  - Request schedule for a valve
  - Update schedule-slot of a valve (clear, set) - each valve has six slots
- Pull information regarding current weekday and time from NTP

## WebSocket Protocol

As to keep parsing as simple as possible, a concise binary protocol will suffice; the first byte always dictates the message's command.

### Enable valve by index

Direction: Client->Server\
Command: `0`\
Arguments:
  - Valve-index byte

### Disable active valve

Direction: Client->Server\
Command: `1`\
Arguments: none

### Valves state change

Direction: Server->Client\
Command: `2`\
Arguments:
  - Valve-count byte
  - Bit-packed state bytes, index increasing left to right

## Reminder-Notes

- 12VDC over the H-bridge doesn't suffice to trigger all kinds of valves I own; `24VAC * sqrt(2) ~= 34V P2P` => add a boost-converter onto the prototype.