#include "json.h"
#include "mqtt/async_client.h"
#include <thread>
#include <chrono>

int main(int argc, char const *argv[])
{
    std::string root = USE_ROOT;
    mqtt::async_client mqtt_client(USE_MQTT_HOST, USE_MQTT_PORT);
    mqtt::connect_options options;
    options.set_clean_session(true);
    options.set_keep_alive_interval(20);
    mqtt_client.connect(options)->wait();

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    json::array sensor_network;

    json::json s0;
    s0["id"] = "s0";
    s0["type"] = "temperature";
    json::object s0_loc;
    s0_loc["lat"] = 37.5078;
    s0_loc["lng"] = 15.083;
    s0["location"] = std::move(s0_loc);
    sensor_network.push_back(std::move(s0));

    json::json s1;
    s1["id"] = "s1";
    s1["type"] = "gate";
    json::object s1_loc;
    s1_loc["lat"] = 37.4978;
    s1_loc["lng"] = 15.073;
    s1["location"] = std::move(s1_loc);
    sensor_network.push_back(std::move(s1));

    json::json bus_1;
    bus_1["id"] = "bus_1";
    bus_1["type"] = "bus";
    sensor_network.push_back(std::move(bus_1));

    json::json bus_2;
    bus_2["id"] = "bus_2";
    bus_2["type"] = "bus";
    sensor_network.push_back(std::move(bus_2));

    json::json bus_3;
    bus_3["id"] = "bus_3";
    bus_3["type"] = "bus";
    sensor_network.push_back(std::move(bus_3));

    json::json s3;
    s3["id"] = "s3";
    s3["type"] = "air_monitoring";
    json::object s3_loc;
    s3_loc["lat"] = 37.5078772;
    s3_loc["lng"] = 15.0970304;
    s3["location"] = std::move(s3_loc);
    sensor_network.push_back(std::move(s3));

    json::json set_sn = std::move(sensor_network);
    std::cout << "Updating sensor network:\n";
    std::cout << set_sn.dump() << "\n"
              << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    mqtt_client.publish(mqtt::make_message(root + "/sensors", set_sn.dump(), 2, true));

    json::json s0_temp;
    json::json s3_air_quality;
    json::json bus_1_bus;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    bus_1_bus["lat"] = 37.508;
    bus_1_bus["lng"] = 15.083;
    std::cout << "Updating position for bus bus_1:\n";
    std::cout << bus_1_bus.dump() << "\n"
              << std::endl;
    mqtt_client.publish(mqtt::make_message(root + "/sensor/bus_1", bus_1_bus.dump()));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    s3_air_quality["pm10"] = 6.2;
    s3_air_quality["pm2.5"] = 5.4;
    std::cout << "Updating air quality for sensor s3:\n";
    std::cout << s3_air_quality.dump() << "\n"
              << std::endl;
    mqtt_client.publish(mqtt::make_message(root + "/sensor/s3", s3_air_quality.dump()));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    bus_1_bus["lat"] = 37.5075;
    bus_1_bus["lng"] = 15.084;
    std::cout << "Updating position for bus bus_1:\n";
    std::cout << bus_1_bus.dump() << "\n"
              << std::endl;
    mqtt_client.publish(mqtt::make_message(root + "/sensor/bus_1", bus_1_bus.dump()));

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    s0_temp["temperature"] = 39.5;
    std::cout << "Updating temperature for sensor s0:\n";
    std::cout << s0_temp.dump() << "\n"
              << std::endl;
    mqtt_client.publish(mqtt::make_message(root + "/sensor/s0", s0_temp.dump()));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    bus_1_bus["lat"] = 37.5075;
    bus_1_bus["lng"] = 15.085;
    std::cout << "Updating position for bus bus_1:\n";
    std::cout << bus_1_bus.dump() << "\n"
              << std::endl;
    mqtt_client.publish(mqtt::make_message(root + "/sensor/bus_1", bus_1_bus.dump()));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    s3_air_quality["pm10"] = 26.2;
    s3_air_quality["pm2.5"] = 25.4;
    std::cout << "Updating air quality for sensor s3:\n";
    std::cout << s3_air_quality.dump() << "\n"
              << std::endl;
    mqtt_client.publish(mqtt::make_message(root + "/sensor/s3", s3_air_quality.dump()));

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    s0_temp["temperature"] = 40.3;
    std::cout << "Updating temperature for sensor s0:\n";
    std::cout << s0_temp.dump() << "\n"
              << std::endl;
    mqtt_client.publish(mqtt::make_message(root + "/sensor/s0", s0_temp.dump()));

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    s0_temp["temperature"] = 39.3;
    std::cout << "Updating temperature for sensor s0:\n";
    std::cout << s0_temp.dump() << "\n"
              << std::endl;
    mqtt_client.publish(mqtt::make_message(root + "/sensor/s0", s0_temp.dump()));

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    s0_temp["temperature"] = 40.3;
    std::cout << "Updating temperature for sensor s0:\n";
    std::cout << s0_temp.dump() << "\n"
              << std::endl;
    mqtt_client.publish(mqtt::make_message(root + "/sensor/s0", s0_temp.dump()));

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    json::json part_0;
    part_0["type"] = "road_failure";
    json::json part_0_val;
    part_0_val["val"] = 5.0;
    part_0["value"] = std::move(part_0_val);
    std::cout << "Updating participatory data:\n";
    std::cout << part_0.dump() << "\n"
              << std::endl;
    mqtt_client.publish(mqtt::make_message(root + "/participatory/u0", part_0.dump()));

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return 0;
}
