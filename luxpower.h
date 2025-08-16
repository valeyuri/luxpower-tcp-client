#include "esphome.h"
#include <WiFi.h>

class LuxPowerTCPClient : public esphome::Component {
 public:
  WiFiClient client;
  const char* host = "10.10.10.1"; // IP dongle
  const uint16_t port = 8000;      // Porta TCP

  sensor::Sensor *lux_pv_total;
  sensor::Sensor *lux_battery_soc;
  sensor::Sensor *lux_house_consumption;
  sensor::Sensor *lux_grid_import;

  void setup() override {
    if (!client.connect(host, port)) {
      ESP_LOGE("tcp", "Impossibile connettersi al dongle");
    } else {
      ESP_LOGI("tcp", "Connesso al dongle LuxPower");
    }
  }

  void loop() override {
    if (client.connected() && client.available()) {
      uint8_t buffer[64];
      int len = client.read(buffer, sizeof(buffer));

      if (len > 6) {
        uint16_t fv_total = (buffer[0] << 8) | buffer[1];
        uint8_t soc       = buffer[2];
        uint16_t casa     = (buffer[3] << 8) | buffer[4];
        uint16_t enel     = (buffer[5] << 8) | buffer[6];

        lux_pv_total->publish_state(fv_total);
        lux_battery_soc->publish_state(soc);
        lux_house_consumption->publish_state(casa);
        lux_grid_import->publish_state(enel);

        ESP_LOGI("tcp", "FV:%d W SOC:%d%% Casa:%d W Enel:%d W", fv_total, soc, casa, enel);
      }
    } else if (!client.connected()) {
      ESP_LOGW("tcp", "Connessione persa, riprovo...");
      client.connect(host, port);
    }
  }
};
