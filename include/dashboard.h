#pragma once

#include "core_listener.h"
#include "solver_listener.h"
#include "executor_listener.h"
#include "crow_all.h"
#include <mutex>

namespace dashboard
{
  class dashboard
  {
  public:
    dashboard(const std::string &host = "127.0.0.1", const unsigned short port = 8080);

    void start();
    void wait_for_server_start();
    void stop();

  private:
    const std::string host;
    const unsigned short port;
    crow::SimpleApp app;
    std::unordered_set<crow::websocket::connection *> users;
    std::mutex mtx;
  };
} // namespace dashboard
