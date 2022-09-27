#include "dashboard.h"
#include "logging.h"
#include "use_defs.h"

namespace dashboard
{
    mqtt_callback::mqtt_callback(dashboard &engine) : engine(engine) {}

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
    }

    dashboard::dashboard(const std::string &root, const std::string &dashboard_host, const unsigned short dashboard_port, const std::string &mqtt_server_uri, const std::string &mqtt_client_id) : root(root), dashboard_host(dashboard_host), dashboard_port(dashboard_port), mqtt_client(mqtt_server_uri, mqtt_client_id)
    {
        app.loglevel(crow::LogLevel::Warning);

        CROW_ROUTE(app, "/")
        ([&root]()
         {
            crow::mustache::context ctx;
            ctx["title"] = root + " Dashboard";
            return crow::mustache::load("index.html").render(ctx); });
    }

    void dashboard::start()
    {
        LOG("Starting " + root + " Dashboard Web Server on http://" + dashboard_host + ':' + std::to_string(dashboard_port) + "..");
        app.bindaddr(dashboard_host).port(dashboard_port).run();
    }
    void dashboard::wait_for_server_start()
    {
        app.wait_for_server_start();
        LOG(root + " Dashboard Web Server started..");
    }
    void dashboard::stop()
    {
        LOG("Stopping " + root + " Dashboard Web Server..");
        app.stop();
    }
} // namespace dashboard
