#include "urban_sensing_engine.h"
#include "urban_sensing_engine_db.h"
#include "mqtt/async_client.h"
#include <thread>
#include <chrono>
#include <future>
#include <random>

#define MQTT_URI(host, port) host ":" port

void create_users(coco::mongo_db &db)
{
    LOG("Creating main users..");
    db.create_user("Admin1", "Admin1", "admin1@cnr.it", "admin", {"CTE-MT"}, {{"type", "admin"}});
    db.create_user("Admin2", "Admin2", "admin2@cnr.it", "admin", {"CTE-MT"}, {{"type", "admin"}});
}

void create_sensor_network(coco::mongo_db &db)
{
    LOG("Creating sensor network..");

    LOG("Creating sensor types..");
    auto temp_type_id = db.create_sensor_type("temperature", "A type of sensor for measuring temperature", {{"temperature", coco::parameter_type::Float}});
    auto bus_type_id = db.create_sensor_type("bus", "A smart bus", {{"lat", coco::parameter_type::Float}, {"lng", coco::parameter_type::Float}, {"passengers", coco::parameter_type::Integer}});
    auto gate_type_id = db.create_sensor_type("gate", "A smart gate for detecting vehicles' passages", {{"pedestrian", coco::parameter_type::Integer}, {"vehicle", coco::parameter_type::Integer}, {"other", coco::parameter_type::Integer}});
    auto air_monitoring_type_id = db.create_sensor_type("air_monitoring", "A smart air monitoring station", {{"pm10", coco::parameter_type::Float}, {"pm2.5", coco::parameter_type::Float}, {"co2", coco::parameter_type::Float}, {"co", coco::parameter_type::Float}, {"no2", coco::parameter_type::Float}, {"o3", coco::parameter_type::Float}, {"so2", coco::parameter_type::Float}});
    auto weather_station_type_id = db.create_sensor_type("weather_station", "A smart weather station", {{"temperature", coco::parameter_type::Float}, {"humidity", coco::parameter_type::Float}, {"pressure", coco::parameter_type::Float}, {"wind_speed", coco::parameter_type::Float}, {"wind_direction", coco::parameter_type::Float}, {"rain", coco::parameter_type::Float}});
    auto participatory_sensing_type_id = db.create_sensor_type("participatory_sensing", "A participatory sensing device", {{"status", coco::parameter_type::Float}, {"subject_id", coco::parameter_type::Symbol}});
    auto occupancy_prediction_type_id = db.create_sensor_type("occupancy_prediction", "A smart occupancy prediction device", {{"occupancy", coco::parameter_type::Integer}});
    auto message_sender_type_id = db.create_sensor_type("message_sender", "A message sender device", {{"message", coco::parameter_type::String}});

    LOG("Creating sensors..");
    db.create_sensor("MessageSender0", db.get_sensor_type(message_sender_type_id));

    if (COCO_ROOT == "CTE-MT")
    {
        LOG("Creating CTE-MT sensors..");
        auto temp0_id = db.create_sensor("Temp0", db.get_sensor_type(temp_type_id), new coco::location{16.604, 40.666});
        auto bus0_id = db.create_sensor("Bus0", db.get_sensor_type(bus_type_id));
        auto bus1_id = db.create_sensor("Bus1", db.get_sensor_type(bus_type_id));
        auto gate0_id = db.create_sensor("Gate0", db.get_sensor_type(gate_type_id), new coco::location{16.614, 40.676});
        auto air_monitoring0_id = db.create_sensor("AirMonitoring0", db.get_sensor_type(air_monitoring_type_id), new coco::location{16.594, 40.686});
        auto weather_station0_id = db.create_sensor("WeatherStation0", db.get_sensor_type(weather_station_type_id), new coco::location{16.624, 40.656});
        auto occupancy_prediction0_id = db.create_sensor("OccupancyPrediction0", db.get_sensor_type(occupancy_prediction_type_id), new coco::location{16.654, 40.636});
        auto participatory_sensing0_id = db.create_sensor("ParticipatorySensing0", db.get_sensor_type(participatory_sensing_type_id), new coco::location{16.644, 40.646});

        LOG("Setting CTE-MT sensor values..");
        auto time = std::chrono::system_clock::now();

        db.set_sensor_value(db.get_sensor(temp0_id), time, {{"temperature", 20.0}});
        db.set_sensor_value(db.get_sensor(bus0_id), time, {{"lat", 40.669}, {"lng", 16.609}, {"passengers", 10.0}});
        db.set_sensor_value(db.get_sensor(bus1_id), time, {{"lat", 40.659}, {"lng", 16.599}, {"passengers", 15.0}});
        db.set_sensor_value(db.get_sensor(gate0_id), time, {{"pedestrian", 15.0}, {"vehicle", 10.0}, {"other", 5.0}});
        db.set_sensor_value(db.get_sensor(air_monitoring0_id), time, {{"pm10", 10.0}, {"pm2.5", 5.0}, {"co2", 100.0}, {"co", 10.0}, {"no2", 5.0}, {"o3", 5.0}, {"so2", 5.0}});
    }
}

void create_data(use::urban_sensing_engine_db &db)
{
    if (COCO_ROOT == "CTE-MT")
    {
        LOG("Creating CTE-MT users..");
        auto usr0_id = db.create_user("Mario", "Rossi", "mario.rossi@test.it", "psw_01", {"CTE-MT"}, {{"type", "user"}, {"roles", {"edilizia", "mobilita"}}});
        auto usr1_id = db.create_user("Giuseppe", "Verdi", "giuseppe.verdi@test.it", "psw_02", {"CTE-MT"}, {{"type", "user"}, {"roles", {"mobilita"}}});
        auto usr2_id = db.create_user("Luigi", "Bianchi", "luigi.bianchi@test.it", "psw_03", {"CTE-MT"}, {{"type", "user"}, {"roles", {"edilizia", "verde"}}});

        LOG("Creating CTE-MT roads..");
        auto road0_id = db.create_road("Via XX Settembre", new coco::location{16.606201033431592, 40.66886987440025});
        auto road1_id = db.create_road("Via Lucana", new coco::location{16.610004133290108, 40.66184045053739});
        auto road2_id = db.create_road("Via Roma", new coco::location{16.604441555519582, 40.667417126550916});
        auto road3_id = db.create_road("Via San Biagio", new coco::location{16.608006069013957, 40.667882003624854});
        auto road4_id = db.create_road("Piazza Vittorio Veneto", new coco::location{16.606513082508386, 40.6669187521497});
        auto road5_id = db.create_road("Vico Conservatorio", new coco::location{16.611608001257064, 40.668147065962714});
        auto road6_id = db.create_road("Via Duomo", new coco::location{16.61030855551953, 40.66624791599131});
        auto road7_id = db.create_road("Piazza Duomo", new coco::location{16.611154282508345, 40.66679155634107});
        auto road8_id = db.create_road("Piazzetta Pascoli", new coco::location{16.610178297849036, 40.663425300341075});
        auto road9_id = db.create_road("Piazza del Sedile", new coco::location{16.60974561148296, 40.6660429407916});
        auto road10_id = db.create_road("Via San Potito", new coco::location{16.61219574017874, 40.666770004486914});
        auto road11_id = db.create_road("Via del Castello", new coco::location{16.60025755551943, 40.662020781572956});
        auto road12_id = db.create_road("Via T. Stigliani", new coco::location{16.60734906901401, 40.669571057618185});

        LOG("Creating CTE-MT buildings..");
        auto building0_id = db.create_building("Palazzo dell'Annunziata", db.get_road(road4_id), "Piazza Vittorio Veneto", new coco::location{16.606513082508386, 40.6669187521497});
        auto building1_id = db.create_building("Palazzo Bernardini", db.get_road(road5_id), "Via Conservatorio", new coco::location{16.608006069013957, 40.667882003624854});
        auto building2_id = db.create_building("Palazzo Bronzini", db.get_road(road6_id), "Via Duomo, 2", new coco::location{16.61030855551953, 40.66624791599131});
        auto building3_id = db.create_building("Palazzo Gattini", db.get_road(road7_id), "Piazza Duomo, 13", new coco::location{16.611154282508345, 40.66679155634107});
        auto building4_id = db.create_building("Palazzo Lanfranchi", db.get_road(road8_id), "Piazzetta Pascoli, 1", new coco::location{16.610178297849036, 40.663425300341075});
        auto building5_id = db.create_building("Palazzo Malvinni-Malvezzi", db.get_road(road7_id), "Piazza Duomo, 14", new coco::location{16.60974561148296, 40.6660429407916});
        auto building6_id = db.create_building("Palazzo Ridola", db.get_road(road10_id), "Piazza Duomo", new coco::location{16.61219574017874, 40.666770004486914});
        auto building7_id = db.create_building("Palazzo del Sedile", db.get_road(road9_id), "Piazza del Sedile", new coco::location{16.60025755551943, 40.662020781572956});
        auto building8_id = db.create_building("Palazzo Venusio", db.get_road(road10_id), "Via San Potito", new coco::location{16.60734906901401, 40.669571057618185});
    }
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

        mqtt_client.publish(mqtt::make_message(root + "/sensor/" + temp_id, temp_val.to_string(), 1, true));
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

        json::json bus_val{{"lat", lat}, {"lng", lng}, {"passengers", passengers}};
        mqtt_client.publish(mqtt::make_message(root + "/sensor/" + bus_id, bus_val.to_string(), 1, true));
    }
}

void update_gate(mqtt::async_client &mqtt_client, const std::string &root, const std::string &gate_id, long pedestrian, long vehicle, long other)
{
    std::random_device rnd_dev;
    std::mt19937 generator(rnd_dev());
    std::uniform_int_distribution<int> distr(-1, 1);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        do
        {
            pedestrian += distr(generator);
            vehicle += distr(generator);
            other += distr(generator);
        } while (pedestrian < 0);

        json::json gate_val{{"pedestrian", pedestrian}, {"vehicle", vehicle}, {"other", other}};
        mqtt_client.publish(mqtt::make_message(root + "/sensor/" + gate_id, gate_val.to_string(), 1, true));
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

        json::json air_monitoring_val{{"pm10", pm10}, {"pm2.5", pm2_5}, {"co2", co2}, {"co", co}, {"no2", no2}, {"o3", o3}, {"so2", so2}};
        mqtt_client.publish(mqtt::make_message(root + "/sensor/" + air_monitoring_id, air_monitoring_val.to_string(), 1, true));
    }
}

void dynamic_set_sensor_values(coco::mongo_db &db)
{
    auto sensors = db.get_sensors();

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
    auto gate0_ftr = std::async(std::launch::async, update_gate, std::ref(mqtt_client), std::ref(root), std::ref(gate0_id), 15, 10, 2);
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

        create_users(db);
        create_sensor_network(db);

        create_data(db);
    }
    else
        db.init();

    // we dynamically set the values of the sensors..
    dynamic_set_sensor_values(db);

    return 0;
}
