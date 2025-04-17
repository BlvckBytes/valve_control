#include "wifi_handler.h"

#define format_ip_addr(ip) ip.toString().c_str()

namespace WiFiHandler
{
  static long wfh_conn_last_check = millis();
  static long wfh_last_recon = millis();
  static bool is_initial_call = false;

  static uint8_t *locate_strongest_bssid(int32_t* strongest_channel_output)
  {
    uint8_t* strongest_bssid = NULL;
    int32_t strongest_rssi = 0;
    int32_t strongest_channel = 0;

    int16_t num_networks = WiFi.scanNetworks();
    for (int16_t i = 0; i < num_networks; i++)
    {
      if (!WiFi.SSID(i).equals(WFH_SSID))
        continue;

      int32_t current_rssi = WiFi.RSSI(i);

      if (!strongest_bssid || current_rssi > strongest_rssi)
      {
        strongest_bssid = WiFi.BSSID(i);
        strongest_rssi = current_rssi;
        strongest_channel = WiFi.channel(i);
      }
    }

    if (strongest_channel_output)
      *strongest_channel_output = strongest_channel;

    return strongest_bssid;
  }

  void debug_print_connection_info()
  {
    #ifdef DEBUG_PRINTS
    Serial.printf(
      "{\n"
      "  local_ip: %s\n"
      "  subnet_mask: %s\n"
      "  gateway: %s\n"
      "  dns_1: %s\n"
      "  dns_2: %s\n"
      "}\n",
      format_ip_addr(WiFi.localIP()),
      format_ip_addr(WiFi.subnetMask()),
      format_ip_addr(WiFi.gatewayIP()),
      format_ip_addr(WiFi.dnsIP(0)),
      format_ip_addr(WiFi.dnsIP(1))
    );
    #endif
  }

  bool check_sta_connection_status()
  {
    static bool result_cache = false;

    if (millis() - wfh_conn_last_check < WFH_CONN_STATUS_CACHE)
      return result_cache;

    result_cache = (
      WiFi.status() == WL_CONNECTED &&
      WiFi.localIP() != INADDR_NONE
    );

    if (result_cache)
    {
      if (!Ping.ping(WiFi.gatewayIP(), 1))
      {
        WiFi.disconnect();
        result_cache = false;
      }
    }

    wfh_conn_last_check = millis();
    return result_cache;
  }

  bool connect_strongest_bssid_sta()
  {
    #ifdef DEBUG_PRINTS
    Serial.printf("Attempting to connect to the STA \"%s\"...\n", WFH_SSID);
    #endif

    if (WiFi.getMode() != WIFI_AP_STA)
      WiFi.mode(WIFI_STA);

    uint8_t *bssid = NULL;
    int32_t channel = 0;
    while (!(bssid = locate_strongest_bssid(&channel)))
    {
      #ifdef DEBUG_PRINTS
      Serial.printf("Could not find a STA with the SSID \"%s\"!\n", WFH_SSID);
      #endif
      delay(500);
      continue;
    }

    WiFi.hostname(WFH_DNS_NAME);
    WiFi.begin(WFH_SSID, WFH_PASSWORD, channel, bssid);

    long started = millis();
    while (!check_sta_connection_status())
    {
      if (millis() - started > WFH_TIMEOUT)
      {
        #ifdef DEBUG_PRINTS
        Serial.printf("WiFi connection timed out!\n");
        #endif
        return false;
      }

      // TODO: status_led_update()
      yield();
    }

    #ifdef DEBUG_PRINTS
    // String-length will be: 6x2 hex-chars and n-1 (so 5) ":", thus 17
    char bssid_str[17 + 1];

    size_t bssid_str_ind = 0;
    for (int i = 0; i < 6; i++)
      bssid_str_ind += snprintf(&(bssid_str[bssid_str_ind]), sizeof(bssid_str) - bssid_str_ind, "%s%02X", i == 0 ? "" : ":", bssid[i]);
    bssid_str[bssid_str_ind] = 0;

    Serial.printf("Connected to STA \"%s\" (bssid: %s, channel: %d)\n", WFH_SSID, bssid_str, channel);
    Serial.printf("Received config from DHCP:\n");

    debug_print_connection_info();
    #endif

    return true;
  }

  bool ensure_sta_connected()
  {
    if (!check_sta_connection_status())
    {
      if (is_initial_call && millis() - wfh_last_recon < WFH_RECONN_COOLDOWN)
        return false;
      
      is_initial_call = true;

      #ifdef DEBUG_PRINTS
      Serial.printf("Trying to reconnect!\n");
      #endif

      bool success = connect_strongest_bssid_sta();

      wfh_last_recon = millis();

      if (success) {
        wfh_conn_last_check -= WFH_CONN_STATUS_CACHE;
        return true;
      }

      return false;
    }

    return true;
  }
}