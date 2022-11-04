#pragma once

#include <mongocxx/client.hpp>

namespace use
{
  class mongo_client
  {
  public:
    mongo_client(const std::string &db_uri = "mongodb://localhost:27017", const std::string &root = "urban_sensing_engine");

  private:
    mongocxx::client conn;
    mongocxx::v_noabi::database db;
    mongocxx::v_noabi::collection sensor_types;
    mongocxx::v_noabi::collection sensor_network;
    mongocxx::v_noabi::collection sensor_data;
  };
} // namespace use
