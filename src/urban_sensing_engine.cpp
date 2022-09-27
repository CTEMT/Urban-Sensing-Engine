#include "urban_sensing_engine.h"
#include "logging.h"
#include <sstream>
#include <unordered_set>
#include <chrono>

#define SENSORS_TOPIC "/sensors"
#define SENSOR_TOPIC "/sensor"
#define MESSAGE_TOPIC "/message"

namespace use
{
    void send_message([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        UDFValue engine;
        if (!UDFFirstArgument(udfc, NUMBER_BITS, &engine))
            return;

        UDFValue topic;
        if (!UDFNextArgument(udfc, SYMBOL_BIT, &topic))
            return;

        UDFValue message;
        if (!UDFNextArgument(udfc, STRING_BIT, &message))
            return;

        urban_sensing_engine &e = *reinterpret_cast<urban_sensing_engine *>(engine.integerValue->contents);

        auto msg = mqtt::make_message(e.root + MESSAGE_TOPIC + '/' + topic.lexemeValue->contents, message.lexemeValue->contents);
        e.mqtt_client.publish(msg);
    }

    mqtt_callback::mqtt_callback(urban_sensing_engine &engine) : engine(engine) {}

    void mqtt_callback::connected([[maybe_unused]] const std::string &cause)
    {
        LOG("MQTT client connected!");

        LOG_DEBUG("Subscribing to '" + engine.root + SENSORS_TOPIC "' topic..");
        engine.mqtt_client.subscribe(engine.root + SENSORS_TOPIC, 1);
    }
    void mqtt_callback::connection_lost([[maybe_unused]] const std::string &cause)
    {
        LOG_WARN("MQTT connection lost! trying to reconnect..");
        engine.mqtt_client.reconnect()->wait();
    }
    void mqtt_callback::message_arrived(mqtt::const_message_ptr msg)
    {
        LOG_DEBUG("Message arrived on topic " + msg->get_topic() + "..");
        if (msg->get_topic() == engine.root + SENSORS_TOPIC)
        { // The sensor network has been updated..
            LOG_DEBUG("The sensor network has been updated..");

            std::stringstream ss;
            ss << msg->get_payload();
            auto j_sensors = json::load(ss);
            json::array &sensors = j_sensors;
            std::unordered_set<std::string> sensors_set;
            for (size_t i = 0; i < sensors.size(); ++i)
            {
                json::json &j_sensor = sensors[i];
                json::string_val &j_id = j_sensor["id"];
                std::string sensor_id = j_id;
                sensors_set.insert(sensor_id);
                json::string_val &j_type = j_sensor["type"];
                std::string sensor_type = j_type;
                AssertString(engine.env, ("(sensor_type (name " + sensor_type + "))").c_str());
                if (!engine.sensors.count(sensor_id))
                {
                    LOG_DEBUG("Subscribing to '" + engine.root + SENSOR_TOPIC + '/' + sensor_id + "' topic..");
                    engine.mqtt_client.subscribe(engine.root + SENSOR_TOPIC + '/' + sensor_id, 1);

                    auto *s_fact = AssertString(engine.env, ("(sensor (id " + sensor_id + ") (sensor_type " + sensor_type + "))").c_str());
                    engine.sensors.emplace(sensor_id, std::make_unique<sensor>(sensor_id, sensor_type, s_fact));
                }
            }
            for (const auto &[sensor_id, sensor] : engine.sensors)
                if (!sensors_set.count(sensor_id))
                {
                    LOG_DEBUG("Unsubscribing from '" + engine.root + SENSOR_TOPIC + '/' + sensor_id + "' topic..");
                    engine.mqtt_client.unsubscribe(engine.root + SENSOR_TOPIC + '/' + sensor_id);

                    Retract(engine.sensors.at(sensor_id)->get_fact());
                    engine.sensors.erase(sensor_id);
                }
        }
        else if (msg->get_topic().rfind(engine.root + SENSOR_TOPIC + '/', 0) == 0)
        { // we have a new sensor value..
            std::string sensor_id = msg->get_topic();
            sensor_id.erase(0, (engine.root + SENSOR_TOPIC + '/').length());
            LOG_DEBUG("Sensor " + sensor_id + " has been updated..");
            std::stringstream ss;
            ss << msg->get_payload();
            auto j_value = json::load(ss);
            json::object &j_val = j_value;

            auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            std::string val_fact = "(sensor_value (sensor_id " + sensor_id + ") (local_time " + std::to_string(time) + ") (value ";
            for (const auto &[id, val] : j_val)
            {
                json::string_val &j_v = val;
                val_fact += j_v;
            }
            val_fact += "))";

            auto val = std::make_unique<json::json>(std::move(j_value));
            engine.sensors.at(sensor_id)->set_value(std::move(val));
        }

        Run(engine.env, -1);
    }

    sensor::sensor(const std::string &id, const std::string &type, Fact *fact) : id(id), type(type), fact(fact) {}

    urban_sensing_engine::urban_sensing_engine(const std::string &root, const std::string &server_uri, const std::string &client_id) : root(root), mqtt_client(server_uri, client_id), msg_callback(*this), env(CreateEnvironment())
    {
        options.set_clean_session(false);
        options.set_keep_alive_interval(20);

        mqtt_client.set_callback(msg_callback);

        AddUDF(env, "send_message", "v", 3, 3, "lss", send_message, "send_message", NULL);
        LOG("Loading policy rules..");
        Load(env, "rules/rules.clp");
        AssertString(env, ("(configuration (engine_ptr " + std::to_string(reinterpret_cast<uintptr_t>(this)) + "))").c_str());
        LOG("done..");
    }
    urban_sensing_engine::~urban_sensing_engine() { DestroyEnvironment(env); }

    void urban_sensing_engine::connect()
    {
        try
        {
            LOG("Connecting MQTT client to " + mqtt_client.get_server_uri() + "..");
            mqtt_client.connect(options)->wait();
        }
        catch (const mqtt::exception &e)
        {
            LOG_ERR(e.what());
        }
    }
    void urban_sensing_engine::disconnect()
    {
        try
        {
            LOG("Disconnecting MQTT client from " + mqtt_client.get_server_uri() + "..");
            mqtt_client.disconnect()->wait();
        }
        catch (const mqtt::exception &e)
        {
            LOG_ERR(e.what());
        }
    }
} // namespace use
