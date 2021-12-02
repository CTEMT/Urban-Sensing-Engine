#pragma once

#include "rule.h"
#include "mqtt/async_client.h"

namespace use
{
  // the duration of each tick in milliseconds..
  constexpr size_t tick_duration = 1000;

  class mqtt_callback : public virtual mqtt::callback
  {
  public:
    mqtt_callback(const std::string &mqtt_uri, const std::string &mqtt_client_id = "urban_sensing_engine");
    ~mqtt_callback();

  private:
    void connected(const std::string &cause) override;
    void connection_lost(const std::string &cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;

  private:
    mqtt::async_client mqtt_client;
  };

  class urban_sensing_engine
  {
  public:
    urban_sensing_engine(const std::string &mqtt_uri, const std::string &mqtt_client_id = "urban_sensing_engine");
    ~urban_sensing_engine();

    void start();

  private:
    mqtt_callback cb;
    std::vector<rule *> rules;
    std::unordered_map<std::string, std::string> state;
  };
} // namespace use
