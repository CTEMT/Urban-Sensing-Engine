#pragma once

#include "json.h"
#include "mqtt/async_client.h"
#include "clips.h"
#include "timer.h"
#include <unordered_set>

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

  private:
    urban_sensing_engine &engine;
  };

  class sensor
  {
  public:
    sensor(const std::string &id, const std::string &type, Fact *fact);

    const std::string &get_id() const { return id; }
    const std::string &get_type() const { return type; }
    Fact *get_fact() const { return fact; }

    json::json &get_value() const { return *value; }
    void set_value(std::unique_ptr<json::json> val) { value.swap(val); }

  private:
    const std::string id;
    const std::string type;
    Fact *fact;
    std::unique_ptr<json::json> value;
  };

  class urban_sensing_engine
  {
    friend class mqtt_callback;
    friend class use_executor;

  public:
    urban_sensing_engine(const std::string &root, const std::string &server_uri = "tcp://localhost:1883", const std::string &client_id = "urban_sensing_engine");
    ~urban_sensing_engine();

    void connect();
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
    ratio::time::timer use_timer;
    std::unordered_set<std::string> sensor_types;
    std::unordered_map<std::string, std::unique_ptr<sensor>> sensors;
    std::list<std::unique_ptr<use_executor>> executors;
    Environment *env;
  };
} // namespace use
