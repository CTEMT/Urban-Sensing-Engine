#include "urban_sensing_engine.h"
#include "logging.h"

#define SENSORS_TOPIC "/sensors"

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
        }
    }

    urban_sensing_engine::urban_sensing_engine(const std::string &root, const std::string &server_uri, const std::string &client_id) : root(root), mqtt_client(server_uri, client_id), msg_callback(*this)
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
