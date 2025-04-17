#include "web_handler.h"

namespace WebHandler
{
  // TODO: Implement web-socket endpoint on /ws
  AsyncWebServer server(80);
  AsyncWebSocket web_socket("/ws");

  void build_and_provide_state_message(void* arg, void message_handler(uint8_t*, size_t, void*))
  {
    int relay_count = SelectionRelays::get_relay_count();
    int bitpacked_bytes_count = (relay_count + 7) / 8;
    size_t message_length = bitpacked_bytes_count + 2;
    uint8_t message[message_length];

    message[0] = WPC_VALVES_STATE_CHANGE;
    message[1] = relay_count;

    uint8_t relay_index = 0;

    for (int byte_index = 0; byte_index < bitpacked_bytes_count; ++byte_index)
    {
      uint8_t current_byte = 0;

      for (uint8_t bit_index = 7; bit_index >= 0; --bit_index)
      {
        if (relay_index >= relay_count)
          break;

        if (SelectionRelays::get_relay_state(relay_index))
          current_byte |= 1 << bit_index;

        ++relay_index;
      }

      message[2 + byte_index] = current_byte;
    }

    message_handler(message, message_length, arg);
  }

  bool on_web_socket_data(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsFrameInfo* info, uint8_t* data, size_t len)
  {
    // This protocol does not require multi-frame messages
    if (!info->final || info->index != 0 || info->len != len)
      return false;

    if (info->opcode != WS_BINARY || info->len == 0)
      return false;

    uint8_t command = data[0];

    if (command == WPC_ENABLE_VALVE_BY_INDEX)
    {
      if (info->len != 2)
        return false;

      uint8_t valve_index = data[1];

      if (!SelectionRelays::activate_by_index(valve_index))
        return false;

      HBridgePWM::enable_output();
      return true;
    }

    if (command == WPC_DISABLE_ACTIVE_VALVE)
    {
      if (info->len != 1)
        return false;

      HBridgePWM::disable_output();
      SelectionRelays::deactivate_all();
      return true;
    }

    // Unknown command
    return false;
  }

  void on_web_socket_event(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len)
  {
    switch (type) {
      case WS_EVT_CONNECT:
        #ifdef DEBUG_PRINTS
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        #endif

        build_and_provide_state_message(client, [](uint8_t* message, size_t message_length, void* arg) -> void {
          ((AsyncWebSocketClient*) arg)->binary(message, message_length);
        });

        break;

      case WS_EVT_DISCONNECT:
        #ifdef DEBUG_PRINTS
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        #endif
        break;

      case WS_EVT_DATA:
        if (!on_web_socket_data(server, client, (AwsFrameInfo*) arg, data, len))
        {
          #ifdef DEBUG_PRINTS
          Serial.printf("WebSocket client #%u sent invalid command [", client->id());
          for (size_t data_index = 0; data_index < len; ++data_index)
          {
            if (data_index != 0)
              Serial.print(", ");

            Serial.printf("%02X", data[data_index]);
          }
          Serial.println("]; disconnecting!");
          #endif

          client->close();
        }
        break;

      case WS_EVT_PONG:
        break;

      case WS_EVT_ERROR:
        #ifdef DEBUG_PRINTS
        Serial.printf("WebSocket client #%u caused a ws-error; disconnecting!", client->id());
        #endif
        client->close();
        break;
    }
  }

  void on_relay_state_change()
  {
    build_and_provide_state_message(nullptr, [](uint8_t* message, size_t message_length, void* arg) -> void {
      for (AsyncWebSocketClient* client : web_socket.getClients())
      {
        client->binary(message, message_length);
      }
    });
  }

  bool setup()
  {
    // TODO: Does this support hot-plug once booted up? Likely not - add!
    if (!SD.begin())
    {
      #ifdef DEBUG_PRINTS
      Serial.println("Could not initialize the SD module");
      #endif
      return false;
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SD, "/index.html", "text/html");
    });

    web_socket.onEvent(on_web_socket_event);
    server.addHandler(&web_socket);

    server.serveStatic("/", SD, "/");
    server.begin();

    SelectionRelays::set_state_change_callback(on_relay_state_change);
    return true;
  }
}