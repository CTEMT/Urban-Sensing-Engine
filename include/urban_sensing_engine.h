#pragma once

#include "mqtt/async_client.h"

namespace use
{
  class urban_sensing_engine
  {
  public:
    urban_sensing_engine(const std::string &server_uri, const std::string &client_id);
    ~urban_sensing_engine();

  private:
    mqtt::async_client mqtt_client;
  };
} // namespace use
