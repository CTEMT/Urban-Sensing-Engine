#include "dashboard.h"
#include "logging.h"
#include "use_defs.h"

namespace dashboard
{
    mqtt_callback::mqtt_callback(dashboard &engine) : engine(engine) {}

    void mqtt_callback::connected([[maybe_unused]] const std::string &cause)
    {
        LOG("MQTT client connected!");

        LOG_DEBUG("Subscribing to '" + engine.root + MESSAGE_TOPIC "' topic..");
        engine.mqtt_client.subscribe(engine.root + MESSAGE_TOPIC, 1);
    }
    void mqtt_callback::connection_lost([[maybe_unused]] const std::string &cause)
    {
        LOG_WARN("MQTT connection lost! trying to reconnect..");
        engine.mqtt_client.reconnect()->wait();
    }
    void mqtt_callback::message_arrived(mqtt::const_message_ptr msg)
    {
        LOG_DEBUG("Message arrived on topic " + msg->get_topic() + "..");

        if (msg->get_topic() == engine.root + MESSAGE_TOPIC)
            engine.broadcast(msg->get_payload());
    }

    dashboard::dashboard(const std::string &root, const std::string &dashboard_host, const unsigned short dashboard_port, const std::string &mqtt_server_uri, const std::string &mqtt_client_id) : root(root), dashboard_host(dashboard_host), dashboard_port(dashboard_port), mqtt_client(mqtt_server_uri, mqtt_client_id), msg_callback(*this)
    {
        app.loglevel(crow::LogLevel::Warning);

        CROW_ROUTE(app, "/")
        ([&root]()
         {
            crow::mustache::context ctx;
            ctx["title"] = root + " Dashboard";
            ctx["latitude"] = USE_DASHBOARD_LAT;
            ctx["longitude"] = USE_DASHBOARD_LNG;
            return crow::mustache::load("index.html").render(ctx); });

        CROW_ROUTE(app, "/use")
            .websocket()
            .onopen([&](crow::websocket::connection &conn)
                    { std::lock_guard<std::mutex> _(mtx);
                users.insert(&conn); })
            .onclose([&](crow::websocket::connection &conn, const std::string &)
                     { std::lock_guard<std::mutex> _(mtx); users.erase(&conn); })
            .onmessage([&](crow::websocket::connection &, const std::string &, bool) {});

        options.set_clean_session(false);
        options.set_keep_alive_interval(20);

        mqtt_client.set_callback(msg_callback);
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

    void dashboard::connect()
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
    void dashboard::disconnect()
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

    void dashboard::broadcast(const std::string &msg)
    {
        for (const auto &u : users)
            u->send_text(msg);
    }
} // namespace dashboard
