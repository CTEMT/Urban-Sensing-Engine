#include "urban_sensing_engine.h"
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>

namespace use
{
    urban_sensing_engine::urban_sensing_engine(const std::string &mqtt_uri, const std::string &mqtt_client_id) : cb(mqtt_uri, mqtt_client_id)
    {
        std::cout << "Loading rules.." << std::endl;
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
        std::thread t([this]()
                      {
                          std::chrono::steady_clock::time_point tick_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(tick_duration);
                          while (true)
                          {
                              for (const auto &r : rules)
                                  if (r->applicable(state))
                                  {
                                  }

                              std::this_thread::sleep_until(tick_time += std::chrono::milliseconds(tick_duration));
                          }
                      });
        t.join();
    }

    mqtt_callback::mqtt_callback(const std::string &mqtt_uri, const std::string &mqtt_client_id) : mqtt_client(mqtt_uri, mqtt_client_id)
    {
        mqtt::connect_options opts;
        opts.set_keep_alive_interval(20);
        opts.set_clean_session(true);
        opts.set_user_name("urban_sensing_engine");
        opts.set_password("password");

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
        mqtt_client.subscribe("#", 1);
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
    }
} // namespace use
