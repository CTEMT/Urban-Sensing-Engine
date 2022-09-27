#include "urban_sensing_engine.h"
#include "logging.h"
#include <sstream>
#include <unordered_set>

#define SENSORS_TOPIC "/sensors"
#define SENSOR_TOPIC "/sensor"

namespace use
{
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
        engine.mqtt_client.reconnect();
    }
    void mqtt_callback::message_arrived(mqtt::const_message_ptr msg)
    {
        LOG_DEBUG("Message arrived on topic " + msg->get_topic() + "..");
        if (msg->get_topic() == engine.root + SENSORS_TOPIC)
        { // The sensor network has been updated..
            LOG_DEBUG("The sensor network has been updated..");

            std::stringstream ss;
            ss << msg->get_payload();
            auto c_sensors = json::load(ss);
            json::array &sensors = c_sensors;
            std::unordered_set<std::string> sensors_set;
            for (size_t i = 0; i < sensors.size(); ++i)
            {
                json::string_val &sensor = sensors[i];
                std::string sensor_id = sensor;
                sensors_set.insert(sensor_id);
                if (!engine.state.count(sensor_id))
                {
                    LOG_DEBUG("Subscribing to '" + engine.root + SENSOR_TOPIC + '/' + sensor_id + "' topic..");
                    engine.mqtt_client.subscribe(engine.root + SENSOR_TOPIC + '/' + sensor_id, 1);
                    engine.state.emplace(sensor_id, nullptr);
                }
            }
            for (const auto &[sensor_id, val] : engine.state)
                if (!sensors_set.count(sensor_id))
                {
                    LOG_DEBUG("Unsubscribing from '" + engine.root + SENSOR_TOPIC + '/' + sensor_id + "' topic..");
                    engine.mqtt_client.unsubscribe(engine.root + SENSOR_TOPIC + '/' + sensor_id);
                }
        }
        else if (msg->get_topic().rfind(engine.root + SENSOR_TOPIC + '/', 0) == 0)
        { // we have a new sensor value..
            std::string sensor_id = msg->get_topic();
            sensor_id.erase(0, (engine.root + SENSOR_TOPIC + '/').length());
            LOG_DEBUG("Sensor " + sensor_id + " has been updated..");
            std::stringstream ss;
            ss << msg->get_payload();
            auto value = json::load(ss);
            auto val = std::make_unique<json::json>(std::move(value));
            engine.state.at(sensor_id).swap(val);
        }
    }

    urban_sensing_engine::urban_sensing_engine(const std::string &root, const std::string &server_uri, const std::string &client_id) : root(root), mqtt_client(server_uri, client_id), msg_callback(*this), env(CreateEnvironment())
    {
        options.set_clean_session(false);
        options.set_keep_alive_interval(20);

        mqtt_client.set_callback(msg_callback);
    }

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
