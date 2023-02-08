#include "urban_sensing_engine.h"
#include "urban_sensing_engine_db.h"
#include "mqtt/async_client.h"
#include <thread>
#include <chrono>
#include <future>
#include <random>

#define MQTT_URI(host, port) host ":" port

void create_sensor_network(coco::mongo_db &db)
{
    // we create the sensor types..
    auto temp_type_id = db.create_sensor_type("temperature", "A type of sensor for measuring temperature", {{"temperature", coco::parameter_type::Float}});
    auto bus_type_id = db.create_sensor_type("bus", "A smart bus", {{"lat", coco::parameter_type::Float}, {"lng", coco::parameter_type::Float}, {"passengers", coco::parameter_type::Integer}});
    auto gate_type_id = db.create_sensor_type("gate", "A smart gate for detecting vehicles' passages", {{"passes", coco::parameter_type::Integer}});
    auto air_monitoring_type_id = db.create_sensor_type("air_monitoring", "A smart air monitoring station", {{"pm10", coco::parameter_type::Float}, {"pm25", coco::parameter_type::Float}, {"co2", coco::parameter_type::Float}, {"co", coco::parameter_type::Float}, {"no2", coco::parameter_type::Float}, {"o3", coco::parameter_type::Float}, {"so2", coco::parameter_type::Float}});
    auto weather_station_type_id = db.create_sensor_type("weather_station", "A smart weather station", {{"temperature", coco::parameter_type::Float}, {"humidity", coco::parameter_type::Float}, {"pressure", coco::parameter_type::Float}, {"wind_speed", coco::parameter_type::Float}, {"wind_direction", coco::parameter_type::Float}, {"rain", coco::parameter_type::Float}});
    auto participatory_sensing_type_id = db.create_sensor_type("participatory_sensing", "A participatory sensing device", {{"status", coco::parameter_type::Float}, {"subject_id", coco::parameter_type::Symbol}});
    auto occupancy_prediction_type_id = db.create_sensor_type("occupancy_prediction", "A smart occupancy prediction device", {{"occupancy", coco::parameter_type::Integer}});
    auto message_sender_type_id = db.create_sensor_type("message_sender", "A message sender device", {{"message", coco::parameter_type::String}});

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

    auto weather_station0_loc = std::make_unique<coco::location>();
    weather_station0_loc->x = 40.656;
    weather_station0_loc->y = 16.624;
    auto weather_station0_id = db.create_sensor("WeatherStation0", db.get_sensor_type(weather_station_type_id), std::move(weather_station0_loc));

    auto participatory_sensing0_loc = std::make_unique<coco::location>();
    participatory_sensing0_loc->x = 40.646;
    participatory_sensing0_loc->y = 16.644;
    auto participatory_sensing0_id = db.create_sensor("ParticipatorySensing0", db.get_sensor_type(participatory_sensing_type_id), std::move(participatory_sensing0_loc));

    auto message_sender0_id = db.create_sensor("MessageSender0", db.get_sensor_type(message_sender_type_id));
}

void create_roads(use::urban_sensing_engine_db &db)
{
    // we create the roads..
    if (COCO_ROOT == "CTE-MT")
    {
        auto road0_loc = std::make_unique<coco::location>();
        road0_loc->x = 40.66886987440025;
        road0_loc->y = 16.606201033431592;
        auto road0_id = db.create_road("Via XX Settembre", std::move(road0_loc));

        auto road1_loc = std::make_unique<coco::location>();
        road1_loc->x = 40.66184045053739;
        road1_loc->y = 16.610004133290108;
        auto road1_id = db.create_road("Via Lucana", std::move(road1_loc));

        auto road2_loc = std::make_unique<coco::location>();
        road2_loc->x = 40.667417126550916;
        road2_loc->y = 16.604441555519582;
        auto road2_id = db.create_road("Via Roma", std::move(road2_loc));

        auto road3_loc = std::make_unique<coco::location>();
        road3_loc->x = 40.667882003624854;
        road3_loc->y = 16.608006069013957;
        auto road3_id = db.create_road("Via San Biagio", std::move(road3_loc));

        auto road4_loc = std::make_unique<coco::location>();
        road4_loc->x = 40.6669187521497;
        road4_loc->y = 16.606513082508386;
        auto road4_id = db.create_road("Piazza Vittorio Veneto", std::move(road4_loc));

        auto road5_loc = std::make_unique<coco::location>();
        road5_loc->x = 40.668147065962714;
        road5_loc->y = 16.611608001257064;
        auto road5_id = db.create_road("Vico Conservatorio", std::move(road5_loc));

        auto road6_loc = std::make_unique<coco::location>();
        road6_loc->x = 40.66624791599131;
        road6_loc->y = 16.61030855551953;
        auto road6_id = db.create_road("Via Duomo", std::move(road6_loc));

        auto road7_loc = std::make_unique<coco::location>();
        road7_loc->x = 40.66679155634107;
        road7_loc->y = 16.611154282508345;
        auto road7_id = db.create_road("Piazza Duomo", std::move(road7_loc));

        auto road8_loc = std::make_unique<coco::location>();
        road8_loc->x = 40.663425300341075;
        road8_loc->y = 16.610178297849036;
        auto road8_id = db.create_road("Piazzetta Pascoli", std::move(road8_loc));

        auto road9_loc = std::make_unique<coco::location>();
        road9_loc->x = 40.6660429407916;
        road9_loc->y = 16.60974561148296;
        auto road9_id = db.create_road("Piazza del Sedile", std::move(road9_loc));

        auto road10_loc = std::make_unique<coco::location>();
        road10_loc->x = 40.666770004486914;
        road10_loc->y = 16.61219574017874;
        auto road10_id = db.create_road("Via San Potito", std::move(road10_loc));

        auto road11_loc = std::make_unique<coco::location>();
        road11_loc->x = 40.662020781572956;
        road11_loc->y = 16.60025755551943;
        auto road11_id = db.create_road("Via del Castello", std::move(road11_loc));

        auto road12_loc = std::make_unique<coco::location>();
        road12_loc->x = 40.669571057618185;
        road12_loc->y = 16.60734906901401;
        auto road12_id = db.create_road("Via T. Stigliani", std::move(road12_loc));
    }
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
    air_monitoring0_val["co2"] = 100.0;
    air_monitoring0_val["co"] = 10.0;
    air_monitoring0_val["no2"] = 5.0;
    air_monitoring0_val["o3"] = 5.0;
    air_monitoring0_val["so2"] = 5.0;
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

void update_air_monitoring(mqtt::async_client &mqtt_client, const std::string &root, const std::string &air_monitoring_id, double pm10, double pm2_5, double co2, double co, double no2, double o3, double so2)
{
    std::random_device rnd_dev;
    std::mt19937 generator(rnd_dev());
    std::uniform_real_distribution<double> distr(-1.0, 1.0);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        pm10 += distr(generator);
        pm2_5 += distr(generator);
        co2 += distr(generator);
        co += distr(generator);
        no2 += distr(generator);
        o3 += distr(generator);
        so2 += distr(generator);

        json::json air_monitoring_val;
        air_monitoring_val["pm10"] = pm10;
        air_monitoring_val["pm2.5"] = pm2_5;
        air_monitoring_val["co2"] = co2;
        air_monitoring_val["co"] = co;
        air_monitoring_val["no2"] = no2;
        air_monitoring_val["o3"] = o3;
        air_monitoring_val["so2"] = so2;

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
    auto air_monitoring0_ftr = std::async(std::launch::async, update_air_monitoring, std::ref(mqtt_client), std::ref(root), std::ref(air_monitoring0_id), 10.0, 5.0, 1000.0, 100.0, 200.0, 300.0, 400.0);
}

int main(int argc, char const *argv[])
{
    mongocxx::instance inst{}; // This should be done only once.
    use::urban_sensing_engine_db db;

    bool init = false;
    if (init)
    {
        db.drop(); // Warning!! We are here deleting all the current data!!

        create_sensor_network(db);
        set_sensor_values(db);

        create_roads(db);
    }
    else
        db.init();

    // we dynamically set the values of the sensors..
    dynamic_set_sensor_values(db);

    return 0;
}
