#include "urban_sensing_engine.h"
#include "mongo_db.h"
#include "mqtt/async_client.h"
#include <thread>
#include <chrono>
#include <future>
#include <random>

#define MQTT_URI(host, port) host ":" port

void create_sensor_network(coco::mongo_db &db)
{
    db.drop(); // Warning!! We are deleting all the current data!!

    // we create the sensor types..
    auto temp_type_id = db.create_sensor_type("temperature", "A type of sensor for measuring temperature");
    auto bus_type_id = db.create_sensor_type("bus", "A smart bus");
    auto gate_type_id = db.create_sensor_type("gate", "A smart gate for detecting vehicles' passages");
    auto air_monitoring_type_id = db.create_sensor_type("air_monitoring", "A smart air monitoring station");

    // we create the sensors..
    auto temp0_loc = std::make_unique<coco::location>();
    temp0_loc->x = 40.666;
    temp0_loc->y = 16.604;
    auto temp0_id = db.create_sensor("Temp0", db.get_sensor_type(temp_type_id), std::move(temp0_loc));

    auto bus0_id = db.create_sensor("Bus0", db.get_sensor_type(bus_type_id));
    auto bus1_id = db.create_sensor("Bus1", db.get_sensor_type(bus_type_id));

    auto gate0_loc = std::make_unique<coco::location>();
    gate0_loc->x = 40.676;
    gate0_loc->y = 16.614;
    auto gate0_id = db.create_sensor("Gate0", db.get_sensor_type(gate_type_id), std::move(gate0_loc));

    auto air_monitoring0_loc = std::make_unique<coco::location>();
    air_monitoring0_loc->x = 40.686;
    air_monitoring0_loc->y = 16.594;
    auto air_monitoring0_id = db.create_sensor("AirMonitoring0", db.get_sensor_type(air_monitoring_type_id), std::move(air_monitoring0_loc));
}

void set_sensor_values(coco::mongo_db &db)
{
    auto sensors = db.get_all_sensors();

    std::string temp0_id;
    std::string bus0_id;
    std::string bus1_id;
    std::string gate0_id;
    std::string air_monitoring0_id;

    for (const auto &sensor : sensors)
    {
        if (sensor.get().get_type().get_name() == "temperature")
            temp0_id = sensor.get().get_id();
        else if (sensor.get().get_type().get_name() == "gate")
            gate0_id = sensor.get().get_id();
        else if (sensor.get().get_type().get_name() == "bus")
        {
            if (sensor.get().get_name() == "Bus0")
                bus0_id = sensor.get().get_id();
            else if (sensor.get().get_name() == "Bus1")
                bus1_id = sensor.get().get_id();
        }
        else if (sensor.get().get_type().get_name() == "air_monitoring")
            air_monitoring0_id = sensor.get().get_id();
    }

    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    json::json temp0_val;
    temp0_val["temperature"] = 20.0;
    db.set_sensor_value(temp0_id, time, temp0_val);

    json::json bus0_val;
    bus0_val["lat"] = 40.669;
    bus0_val["lng"] = 16.609;
    bus0_val["passengers"] = 10.0;
    db.set_sensor_value(bus0_id, time, bus0_val);

    json::json bus1_val;
    bus1_val["lat"] = 40.659;
    bus1_val["lng"] = 16.599;
    bus1_val["passengers"] = 15.0;
    db.set_sensor_value(bus1_id, time, bus1_val);

    json::json gate0_val;
    gate0_val["passes"] = 15.0;
    db.set_sensor_value(gate0_id, time, gate0_val);

    json::json air_monitoring0_val;
    air_monitoring0_val["pm10"] = 10.0;
    air_monitoring0_val["pm2.5"] = 5.0;
    db.set_sensor_value(air_monitoring0_id, time, air_monitoring0_val);
}

void update_temperature(mqtt::async_client &mqtt_client, const std::string &root, const std::string &temp_id, double temp)
{
    std::random_device rnd_dev;
    std::mt19937 generator(rnd_dev());
    std::uniform_real_distribution<double> distr(-1.0, 1.0);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        temp += distr(generator);

        json::json temp_val;
        temp_val["temperature"] = temp;

        mqtt_client.publish(mqtt::make_message(root + "/sensor/" + temp_id, temp_val.dump(), 1, true));
    }
}

void update_bus(mqtt::async_client &mqtt_client, const std::string &root, const std::string &bus_id, double lat, double lng, long passengers)
{
    std::random_device rnd_dev;
    std::mt19937 generator(rnd_dev());
    std::uniform_real_distribution<double> real_distr(-0.01, 0.01);
    std::uniform_int_distribution<int> int_distr(-1, 1);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        lat += real_distr(generator);
        lng += real_distr(generator);
        do
        {
            passengers += int_distr(generator);
        } while (passengers < 0);

        json::json bus_val;
        bus_val["lat"] = lat;
        bus_val["lng"] = lng;
        bus_val["passengers"] = passengers;

        mqtt_client.publish(mqtt::make_message(root + "/sensor/" + bus_id, bus_val.dump(), 1, true));
    }
}

void update_gate(mqtt::async_client &mqtt_client, const std::string &root, const std::string &gate_id, long passes)
{
    std::random_device rnd_dev;
    std::mt19937 generator(rnd_dev());
    std::uniform_int_distribution<int> distr(-1, 1);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        do
        {
            passes += distr(generator);
        } while (passes < 0);

        json::json gate_val;
        gate_val["passes"] = passes;

        mqtt_client.publish(mqtt::make_message(root + "/sensor/" + gate_id, gate_val.dump(), 1, true));
    }
}

void update_air_monitoring(mqtt::async_client &mqtt_client, const std::string &root, const std::string &air_monitoring_id, double pm10, double pm2_5)
{
    std::random_device rnd_dev;
    std::mt19937 generator(rnd_dev());
    std::uniform_real_distribution<double> distr(-1.0, 1.0);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        pm10 += distr(generator);
        pm2_5 += distr(generator);

        json::json air_monitoring_val;
        air_monitoring_val["pm10"] = pm10;
        air_monitoring_val["pm2.5"] = pm2_5;

        mqtt_client.publish(mqtt::make_message(root + "/sensor/" + air_monitoring_id, air_monitoring_val.dump(), 1, true));
    }
}

void dynamic_set_sensor_values(coco::mongo_db &db)
{
    auto sensors = db.get_all_sensors();

    std::string temp0_id;
    std::string bus0_id;
    std::string bus1_id;
    std::string gate0_id;
    std::string air_monitoring0_id;

    for (const auto &sensor : sensors)
    {
        if (sensor.get().get_type().get_name() == "temperature")
            temp0_id = sensor.get().get_id();
        else if (sensor.get().get_type().get_name() == "gate")
            gate0_id = sensor.get().get_id();
        else if (sensor.get().get_type().get_name() == "bus")
        {
            if (sensor.get().get_name() == "Bus0")
                bus0_id = sensor.get().get_id();
            else if (sensor.get().get_name() == "Bus1")
                bus1_id = sensor.get().get_id();
        }
        else if (sensor.get().get_type().get_name() == "air_monitoring")
            air_monitoring0_id = sensor.get().get_id();
    }

    std::string root = COCO_ROOT;
    mqtt::async_client mqtt_client(MQTT_URI(MQTT_HOST, MQTT_PORT), root + "-test-client");
    mqtt::connect_options options;
    options.set_clean_session(true);
    options.set_keep_alive_interval(20);
    mqtt_client.connect(options)->wait();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    auto temp0_ftr = std::async(std::launch::async, update_temperature, std::ref(mqtt_client), std::ref(root), std::ref(temp0_id), 20.0);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    auto bus0_ftr = std::async(std::launch::async, update_bus, std::ref(mqtt_client), std::ref(root), std::ref(bus0_id), 40.669, 16.609, 10);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    auto bus1_ftr = std::async(std::launch::async, update_bus, std::ref(mqtt_client), std::ref(root), std::ref(bus1_id), 40.659, 16.599, 15);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    auto gate0_ftr = std::async(std::launch::async, update_gate, std::ref(mqtt_client), std::ref(root), std::ref(gate0_id), 15);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    auto air_monitoring0_ftr = std::async(std::launch::async, update_air_monitoring, std::ref(mqtt_client), std::ref(root), std::ref(air_monitoring0_id), 10.0, 5.0);
}

int main(int argc, char const *argv[])
{
    mongocxx::instance inst{}; // This should be done only once.
    coco::mongo_db db;

    bool init = false;
    if (init)
    {
        create_sensor_network(db);
        set_sensor_values(db);
    }
    else
        db.init();

    // we dynamically set the values of the sensors..
    dynamic_set_sensor_values(db);

    return 0;
}
