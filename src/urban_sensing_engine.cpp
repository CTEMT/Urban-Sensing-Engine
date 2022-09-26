#include "urban_sensing_engine.h"
#include "logging.h"

namespace use
{
    connection_callback::connection_callback(urban_sensing_engine &engine) : engine(engine) {}

    void connection_callback::on_failure([[maybe_unused]] const mqtt::token &tkn)
    {
        LOG_WARN("connection failure..");
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        LOG("trying to reconnect..");
        engine.mqtt_client.reconnect();
    }
    void connection_callback::on_success([[maybe_unused]] const mqtt::token &tkn) { LOG_WARN("connection success!"); }

    mqtt_callback::mqtt_callback(urban_sensing_engine &engine) : engine(engine) {}

    void mqtt_callback::on_failure([[maybe_unused]] const mqtt::token &tkn) { LOG_WARN("failure" + (tkn.get_message_id() != 0 ? (" for token [" + std::to_string(tkn.get_message_id()) + "]") : "")); }
    void mqtt_callback::on_success([[maybe_unused]] const mqtt::token &tkn) { LOG_DEBUG("success" + (tkn.get_message_id() != 0 ? (" for token [" + std::to_string(tkn.get_message_id()) + "]") : "")); }

    urban_sensing_engine::urban_sensing_engine(const std::string &server_uri, const std::string &client_id) : mqtt_client(server_uri, client_id), conn_callback(*this), msg_callback(*this)
    {
        options.set_clean_session(false);
        options.set_keep_alive_interval(20);

        mqtt_client.set_callback(msg_callback);
        try
        {
            LOG("connecting to " + server_uri + "..");
            mqtt_client.connect(options, nullptr, conn_callback);
        }
        catch (const mqtt::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
} // namespace use
