#pragma once

#include "mqtt/async_client.h"

namespace use
{
  class urban_sensing_engine;

  class mqtt_callback : public mqtt::callback
  {
  public:
    mqtt_callback(urban_sensing_engine &engine);

  private:
    void connected(const std::string &cause) override;
    void connection_lost(const std::string &cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;

  private:
    urban_sensing_engine &engine;
  };

  class urban_sensing_engine
  {
    friend class mqtt_callback;

  public:
    urban_sensing_engine(const std::string &root, const std::string &server_uri = "tcp://localhost:1883", const std::string &client_id = "urban_sensing_engine");

    void connect();
    void disconnect();

  private:
    const std::string root;
    mqtt::async_client mqtt_client;
    mqtt::connect_options options;
    mqtt_callback msg_callback;
  };
} // namespace use
