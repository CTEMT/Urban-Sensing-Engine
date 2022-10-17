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

    void connect();
    void disconnect();

  private:
    void broadcast(const std::string &msg);

    class solver
    {
    public:
      solver(uintptr_t id) : id(id){}

      uintptr_t get_id() const { return id; }

      json::object &get_state() { return state; }
      void set_state(json::json s) { state = std::move(s); }

      json::object &get_graph() { return graph; }
      void set_graph(json::json g) { graph = std::move(g); }

    private:
      uintptr_t id;
      json::json state;
      json::json graph;
    };

  private:
    const std::string root;
    const std::string dashboard_host;
    const unsigned short dashboard_port;
    mqtt::async_client mqtt_client;
    mqtt::connect_options options;
    mqtt_callback msg_callback;
    crow::SimpleApp app;
    std::unordered_set<crow::websocket::connection *> users;
    std::unordered_map<uintptr_t, solver> solvers;
    std::mutex mtx;
  };
} // namespace dashboard
