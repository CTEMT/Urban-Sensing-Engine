#pragma once

#include "sensor_network.h"
#include "timer.h"
#include <unordered_set>
#include "mqtt/async_client.h"
#include <mongocxx/client.hpp>

namespace use
{
  class urban_sensing_engine;
  class use_executor;

  class mqtt_callback : public mqtt::callback
  {
  public:
    mqtt_callback(urban_sensing_engine &engine);

  private:
    void connected(const std::string &cause) override;
    void connection_lost(const std::string &cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;

    void update_sensor_network(json::json msg);

  private:
    urban_sensing_engine &engine;
  };

  class urban_sensing_engine
  {
    friend class mqtt_callback;
    friend class use_executor;

  public:
    urban_sensing_engine(const std::string &root, const std::string &server_uri = "tcp://localhost:1883", const std::string &db_uri = "mongodb://localhost:27017", const std::string &client_id = "urban_sensing_engine");
    ~urban_sensing_engine();

    void init();
    void disconnect();

  private:
    void tick();

    friend void send_message(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void send_map_message(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void send_bus_message(Environment *env, UDFContext *udfc, UDFValue *out);

    friend void new_solver(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void read_script(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void read_files(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void delete_solver(Environment *env, UDFContext *udfc, UDFValue *out);

  private:
    const std::string root;
    mqtt::async_client mqtt_client;
    mqtt::connect_options options;
    mqtt_callback msg_callback;
    mongocxx::client conn;
    mongocxx::v_noabi::database db;
    mongocxx::v_noabi::collection sensor_types_collection;
    mongocxx::v_noabi::collection sensors_collection;
    mongocxx::v_noabi::collection sensor_data_collection;
    std::unordered_map<std::string, std::unique_ptr<sensor_type>> sensor_types;
    std::unordered_map<std::string, std::unique_ptr<sensor>> sensors;
    ratio::time::timer use_timer;
    std::list<std::unique_ptr<use_executor>> executors;
    Environment *env;
  };
} // namespace use
