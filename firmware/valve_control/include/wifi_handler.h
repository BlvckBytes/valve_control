#ifndef _H_WIFI_HANDLER
#define _H_WIFI_HANDLER

#include <Arduino.h>
#include <IPAddress.h>
#include <ESP32Ping.h>
#include <WiFi.h>
#include <mdns.h>

#define WFH_TIMEOUT 15000
#define WFH_CONN_STATUS_CACHE 2000
#define WFH_RECONN_COOLDOWN 2000

namespace WiFiHandler
{
  bool ensure_sta_connected();
}

#endif