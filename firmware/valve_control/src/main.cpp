#include "web_handler.h"
#include "wifi_handler.h"

// TODO: Implement: booting - on; connecting: fast flash; connected - slow flash
#define STATUS_LED_PIN 32

void setup()
{
  #ifdef DEBUG_PRINTS
  Serial.begin(115200);
  #endif

  if (!HBridgePWM::setup())
    ESP.restart();

  SelectionRelays::setup();

  if (!WiFiHandler::ensure_sta_connected())
    ESP.restart();

  if (!WebHandler::setup())
    ESP.restart();
}

void loop()
{
  if (!WiFiHandler::ensure_sta_connected())
    return;
}