#pragma once

#include "mqtt/async_client.h"

namespace use
{
  class urban_sensing_engine;

  class connection_callback : public virtual mqtt::iaction_listener
  {
  public:
    connection_callback(urban_sensing_engine &engine);

  private:
    void on_failure(const mqtt::token &tkn) override;
    void on_success(const mqtt::token &tkn) override;

  private:
    urban_sensing_engine &engine;
  };

  class mqtt_callback : public mqtt::callback, public virtual mqtt::iaction_listener
  {
  public:
    mqtt_callback(urban_sensing_engine &engine);

  private:
    void on_failure(const mqtt::token &tkn) override;
    void on_success(const mqtt::token &tkn) override;

    void connected(const std::string &cause) override;
    void connection_lost(const std::string &cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;

  private:
    urban_sensing_engine &engine;
  };

  class urban_sensing_engine
  {
    friend class connection_callback;

  public:
    urban_sensing_engine(const std::string &server_uri, const std::string &client_id = "urban_sensing_engine");

    void connect();
    void disconnect();

  private:
    mqtt::async_client mqtt_client;
    mqtt::connect_options options;
    connection_callback conn_callback;
    mqtt_callback msg_callback;
  };
} // namespace use
