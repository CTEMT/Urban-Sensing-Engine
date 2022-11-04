#pragma once

#include "json.h"
#include <mongocxx/client.hpp>

namespace use
{
  struct sensor_type
  {
    std::string id, name, description;
  };

  class mongo_client
  {
  public:
    mongo_client(const std::string &db_uri = "mongodb://localhost:27017", const std::string &root = "urban_sensing_engine");

  private:
    static json::json to_json(const sensor_type &st) noexcept;

  private:
    mongocxx::client conn;
    mongocxx::v_noabi::database db;
    mongocxx::v_noabi::collection sensor_types;
    mongocxx::v_noabi::collection sensor_network;
    mongocxx::v_noabi::collection sensor_data;
    std::unordered_map<std::string, sensor_type> sensor_types_ids;
  };
} // namespace use
