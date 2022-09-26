#include "urban_sensing_engine.h"
#include "logging.h"

namespace use
{
    connection_callback::connection_callback(urban_sensing_engine &engine) : engine(engine) {}

    void connection_callback::on_failure([[maybe_unused]] const mqtt::token &tkn)
    {
        LOG_WARN("MQTT connection failure..");
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        LOG("trying to reconnect..");
        engine.mqtt_client.reconnect();
    }
    void connection_callback::on_success([[maybe_unused]] const mqtt::token &tkn) { LOG_WARN("MQTT connection success!"); }

    mqtt_callback::mqtt_callback(urban_sensing_engine &engine) : engine(engine) {}

    void mqtt_callback::on_failure([[maybe_unused]] const mqtt::token &tkn) { LOG_WARN("MQTT failure" + (tkn.get_message_id() != 0 ? (" for token [" + std::to_string(tkn.get_message_id()) + "]") : "")); }
    void mqtt_callback::on_success([[maybe_unused]] const mqtt::token &tkn) { LOG_DEBUG("MQTT success" + (tkn.get_message_id() != 0 ? (" for token [" + std::to_string(tkn.get_message_id()) + "]") : "")); }

    void mqtt_callback::connected(const std::string &cause) {}
    void mqtt_callback::connection_lost(const std::string &cause) {}
    void mqtt_callback::message_arrived(mqtt::const_message_ptr msg) {}

    urban_sensing_engine::urban_sensing_engine(const std::string &server_uri, const std::string &client_id) : mqtt_client(server_uri, client_id), conn_callback(*this), msg_callback(*this)
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
            mqtt_client.connect(options, nullptr, conn_callback)->wait();
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
