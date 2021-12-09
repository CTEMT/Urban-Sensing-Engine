#include "urban_sensing_engine.h"
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>

constexpr auto sensors_database = "sensors";

namespace use
{
    urban_sensing_engine::urban_sensing_engine(const std::string &rest_uri, const std::string &mqtt_uri, const std::string &mqtt_client_id) : rest_client(U(rest_uri)), cb(*this, mqtt_uri, mqtt_client_id)
    {
        std::cout << "loading rules.." << std::endl;
        std::string path = std::filesystem::current_path();
        for (const auto &entry : std::filesystem::directory_iterator(path + "/rules"))
        {
            std::cout << ".." << entry.path() << std::endl;
            std::ifstream rule_file(entry.path());
            rules.push_back(new rule(smt::json::from_json(rule_file)));
        }
    }
    urban_sensing_engine::~urban_sensing_engine() {}

    void urban_sensing_engine::start()
    {
        std::cout << "starting sensing.." << std::endl;
        std::thread t([this]()
                      {
                          std::chrono::steady_clock::time_point tick_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(tick_duration);
                          while (true)
                          {
                              check();
                              std::this_thread::sleep_until(tick_time += std::chrono::milliseconds(tick_duration));
                          }
                      });
        t.join();
    }

    std::vector<std::string> urban_sensing_engine::get_sensors()
    {
        std::cout << "retrieving sensors.." << std::endl;
        std::vector<std::string> c_sens;
        web::http::uri_builder builder(U("/list_collections"));
        builder.append_query(U("database"), U(sensors_database));
        web::http::http_request req(web::http::methods::GET);
        req.headers().set_content_type(U("application/json"));
        req.set_request_uri(builder.to_string());
        auto res = rest_client.request(req).get();
        std::istringstream istr(res.extract_string().get());
        const auto j_res = smt::json::from_json(istr);
        const auto j_arr_res = static_cast<smt::array_val &>(*j_res);
        for (size_t i = 0; i < j_arr_res.size(); ++i)
            c_sens.emplace_back(static_cast<smt::string_val &>(*j_arr_res.get(i)).get());
        return c_sens;
    }

    void urban_sensing_engine::check()
    {
        for (const auto &r : rules)
            if (r->applicable(state))
            {
            }
    }

    mqtt_callback::mqtt_callback(urban_sensing_engine &use, const std::string &mqtt_uri, const std::string &mqtt_client_id) : use(use), mqtt_client(mqtt_uri, mqtt_client_id)
    {
        mqtt::connect_options opts;
        opts.set_keep_alive_interval(20);
        opts.set_clean_session(true);
        opts.set_user_name("data");
        opts.set_password("h8^J3@");

        mqtt_client.set_callback(*this);

        try
        {
            std::cout << "connecting to the MQTT server.." << std::endl;
            mqtt_client.connect(opts);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    mqtt_callback::~mqtt_callback() {}

    void mqtt_callback::connected(const std::string &cause)
    {
        std::cout << "connected.." << std::endl;

        // we retrieve all the sensors..
        const auto sensors = use.get_sensors();
        std::cout << "subscribing to sensors.." << std::endl;
        for (const auto &s : sensors)
            mqtt_client.subscribe(s, 1);

        // we to sensor changes..
        std::cout << "subscribing to sensor changes.." << std::endl;
        mqtt_client.subscribe(sensors_database, 1);
    }
    void mqtt_callback::connection_lost(const std::string &cause)
    {
        std::cout << "connection lost.." << std::endl;
    }
    void mqtt_callback::message_arrived(mqtt::const_message_ptr msg)
    {
        std::cout << "message arrived" << std::endl;
        std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
        std::cout << "\tpayload: '" << msg->to_string() << "'\n"
                  << std::endl;
        if (msg->get_topic() == sensors_database)
        { // the sensor network has changed..
            for (const auto &s : use.state)
                mqtt_client.unsubscribe(s.first);

            std::istringstream istr(msg->to_string());
            const auto j_res = smt::json::from_json(istr);
            const auto j_arr_res = static_cast<smt::array_val &>(*j_res);
            for (size_t i = 0; i < j_arr_res.size(); ++i)
            {
                const auto sens = static_cast<smt::string_val &>(*j_arr_res.get(i)).get();
                mqtt_client.subscribe(sens, 1);
            }
        }
        else
        { // we update the state..
            try
            {
                std::istringstream istr(msg->to_string());
                use.state[msg->get_topic()] = smt::json::from_json(istr);
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
                std::cerr << msg->to_string() << '\n';
            }
        }
    }
} // namespace use
