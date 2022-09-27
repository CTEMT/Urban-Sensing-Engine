#pragma once

#include "core_listener.h"
#include "solver_listener.h"
#include "executor_listener.h"
#include "crow_all.h"
#include <mutex>
#include "mqtt/async_client.h"

namespace dashboard
{
  class dashboard;

  class mqtt_callback : public mqtt::callback
  {
  public:
    mqtt_callback(dashboard &engine);

  private:
    void connected(const std::string &cause) override;
    void connection_lost(const std::string &cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;

  private:
    dashboard &engine;
  };

  class dashboard
  {
    friend class mqtt_callback;

  public:
    dashboard(const std::string &root, const std::string &dashboard_host = "127.0.0.1", const unsigned short dashboard_port = 8080, const std::string &mqtt_server_uri = "tcp://localhost:1883", const std::string &mqtt_client_id = "dashboard");

    void start();
    void wait_for_server_start();
    void stop();

  private:
    const std::string root;
    const std::string dashboard_host;
    const unsigned short dashboard_port;
    mqtt::async_client mqtt_client;
    crow::SimpleApp app;
    std::unordered_set<crow::websocket::connection *> users;
    std::mutex mtx;
  };
} // namespace dashboard
