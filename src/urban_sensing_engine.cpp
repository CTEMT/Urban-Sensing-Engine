#include "urban_sensing_engine.h"
#include "logging.h"
#include "use_defs.h"
#include "use_executor.h"
#include <sstream>
#include <unordered_set>
#include <chrono>

namespace use
{
    void send_message([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        UDFValue engine_ptr;
        if (!UDFFirstArgument(udfc, NUMBER_BITS, &engine_ptr))
            return;
        auto &e = *reinterpret_cast<urban_sensing_engine *>(engine_ptr.integerValue->contents);

        UDFValue level;
        if (!UDFNextArgument(udfc, SYMBOL_BIT, &level))
            return;

        UDFValue content;
        if (!UDFNextArgument(udfc, STRING_BIT, &content))
            return;

        if (e.mqtt_client.is_connected())
        {
            json::json msg;
            msg["type"] = "message";
            msg["level"] = level.lexemeValue->contents;
            msg["content"] = content.lexemeValue->contents;

            e.mqtt_client.publish(mqtt::make_message(e.root + MESSAGE_TOPIC, msg.dump()));
        }
    }

    void send_map_message([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        UDFValue engine_ptr;
        if (!UDFFirstArgument(udfc, NUMBER_BITS, &engine_ptr))
            return;
        auto &e = *reinterpret_cast<urban_sensing_engine *>(engine_ptr.integerValue->contents);

        UDFValue level;
        if (!UDFNextArgument(udfc, SYMBOL_BIT, &level))
            return;

        UDFValue lat;
        if (!UDFNextArgument(udfc, NUMBER_BITS, &lat))
            return;

        UDFValue lng;
        if (!UDFNextArgument(udfc, NUMBER_BITS, &lng))
            return;

        UDFValue content;
        if (!UDFNextArgument(udfc, STRING_BIT, &content))
            return;

        if (e.mqtt_client.is_connected())
        {
            json::json msg;
            msg["type"] = "map_message";
            msg["level"] = level.lexemeValue->contents;
            json::object loc;
            loc["lat"] = lat.floatValue->contents;
            loc["lng"] = lng.floatValue->contents;
            msg["location"] = std::move(loc);
            msg["content"] = content.lexemeValue->contents;

            e.mqtt_client.publish(mqtt::make_message(e.root + MESSAGE_TOPIC, msg.dump()));
        }
    }

    void send_bus_message([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        UDFValue engine_ptr;
        if (!UDFFirstArgument(udfc, NUMBER_BITS, &engine_ptr))
            return;
        auto &e = *reinterpret_cast<urban_sensing_engine *>(engine_ptr.integerValue->contents);

        UDFValue bus;
        if (!UDFNextArgument(udfc, SYMBOL_BIT, &bus))
            return;

        UDFValue lat;
        if (!UDFNextArgument(udfc, NUMBER_BITS, &lat))
            return;

        UDFValue lng;
        if (!UDFNextArgument(udfc, NUMBER_BITS, &lng))
            return;

        UDFValue content;
        if (!UDFNextArgument(udfc, STRING_BIT, &content))
            return;

        if (e.mqtt_client.is_connected())
        {
            json::json msg;
            msg["type"] = "bus_message";
            msg["bus"] = bus.lexemeValue->contents;
            json::object loc;
            loc["lat"] = lat.floatValue->contents;
            loc["lng"] = lng.floatValue->contents;
            msg["location"] = std::move(loc);
            msg["content"] = content.lexemeValue->contents;

            e.mqtt_client.publish(mqtt::make_message(e.root + MESSAGE_TOPIC, msg.dump()));
        }
    }

    void new_solver(Environment *env, UDFContext *udfc, UDFValue *out)
    {
        LOG_DEBUG("Creating new solver..");

        UDFValue engine_ptr;
        if (!UDFFirstArgument(udfc, NUMBER_BITS, &engine_ptr))
            return;
        auto &e = *reinterpret_cast<urban_sensing_engine *>(engine_ptr.integerValue->contents);

        auto slv = new ratio::solver::solver();
        auto exec = new ratio::executor::executor(*slv);
        auto use_exec = std::make_unique<use_executor>(e, *exec);
        uintptr_t exec_ptr = reinterpret_cast<uintptr_t>(use_exec.get());

        e.executors.push_back(std::move(use_exec));

        if (e.mqtt_client.is_connected())
        {
            json::json msg;
            msg["type"] = "solvers";
            json::array solvers;
            solvers.reserve(e.executors.size());
            for (const auto &exec : e.executors)
                solvers.push_back(reinterpret_cast<uintptr_t>(exec.get()));
            msg["solvers"] = std::move(solvers);

            e.mqtt_client.publish(mqtt::make_message(e.root + SOLVERS_TOPIC, msg.dump(), QOS, true));
        }

        out->integerValue = CreateInteger(env, exec_ptr);
    }

    void read_script([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        UDFValue exec_ptr;
        if (!UDFFirstArgument(udfc, NUMBER_BITS, &exec_ptr))
            return;
        auto use_exec = reinterpret_cast<use_executor *>(exec_ptr.integerValue->contents);
        auto exec = &use_exec->get_executor();
        auto slv = &exec->get_solver();

        UDFValue riddle;
        if (!UDFNextArgument(udfc, STRING_BIT, &riddle))
            return;

        // we read a riddle script..
        LOG_DEBUG("Reading RiDDLe snippet..");
        slv->read(riddle.lexemeValue->contents);
        slv->solve();
    }

    void read_files([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        UDFValue exec_ptr;
        if (!UDFFirstArgument(udfc, NUMBER_BITS, &exec_ptr))
            return;
        auto use_exec = reinterpret_cast<use_executor *>(exec_ptr.integerValue->contents);
        auto exec = &use_exec->get_executor();
        auto slv = &exec->get_solver();

        UDFValue riddle;
        if (!UDFNextArgument(udfc, MULTIFIELD_BIT, &riddle))
            return;

        // we read some riddle files..
        LOG_DEBUG("Reading RiDDLe files..");
        std::vector<std::string> fs;
        for (size_t i = 0; i < riddle.multifieldValue->length; ++i)
            fs.push_back(riddle.multifieldValue->contents[i].lexemeValue->contents);
        slv->read(fs);
        slv->solve();
    }

    void delete_solver([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        UDFValue engine_ptr;
        if (!UDFFirstArgument(udfc, NUMBER_BITS, &engine_ptr))
            return;
        auto &e = *reinterpret_cast<urban_sensing_engine *>(engine_ptr.integerValue->contents);

        UDFValue exec_ptr;
        if (!UDFFirstArgument(udfc, NUMBER_BITS, &exec_ptr))
            return;
        auto use_exec = reinterpret_cast<use_executor *>(exec_ptr.integerValue->contents);
        auto exec = &use_exec->get_executor();
        auto slv = &exec->get_solver();

        auto use_exec_it = std::find_if(e.executors.cbegin(), e.executors.cend(), [use_exec](auto &slv_ptr)
                                        { return slv_ptr.get() == use_exec; });
        e.executors.erase(use_exec_it);
        delete exec;
        delete slv;

        if (e.mqtt_client.is_connected())
        {
            json::json msg;
            msg["type"] = "deleted_reasoner";
            msg["id"] = reinterpret_cast<uintptr_t>(use_exec);

            e.mqtt_client.publish(mqtt::make_message(e.root + SOLVERS_TOPIC, msg.dump()));
        }
    }

    mqtt_callback::mqtt_callback(urban_sensing_engine &engine) : engine(engine) {}

    void mqtt_callback::connected([[maybe_unused]] const std::string &cause)
    {
        LOG("MQTT client connected!");

        LOG_DEBUG("Subscribing to '" + engine.root + SENSORS_TOPIC "' topic..");
        engine.mqtt_client.subscribe(engine.root + SENSORS_TOPIC, 1);

        LOG_DEBUG("Subscribing to '" + engine.root + PARTICIPATORY_TOPIC "/#' topic..");
        engine.mqtt_client.subscribe(engine.root + PARTICIPATORY_TOPIC + "/#", 1);
    }
    void mqtt_callback::connection_lost([[maybe_unused]] const std::string &cause)
    {
        LOG_WARN("MQTT connection lost! trying to reconnect..");
        engine.mqtt_client.reconnect()->wait();
    }
    void mqtt_callback::message_arrived(mqtt::const_message_ptr msg)
    {
        LOG_DEBUG("Message arrived on topic " + msg->get_topic() + "..");
        if (msg->get_topic() == engine.root + SENSORS_TOPIC)
        { // The sensor network has been updated..
            LOG_DEBUG("The sensor network has been updated..");

            auto j_sensors = json::load(msg->get_payload());
            json::array &sensors = j_sensors;
            std::unordered_set<std::string> sensors_set;
            for (size_t i = 0; i < sensors.size(); ++i)
            {
                json::object &j_sensor = sensors[i];
                json::string_val &j_id = j_sensor["id"];
                std::string sensor_id = j_id;
                sensors_set.insert(sensor_id);
                json::string_val &j_type = j_sensor["type"];
                std::string sensor_type = j_type;

                if (engine.sensor_types.insert(sensor_type).second)
                    AssertString(engine.env, ("(sensor_type (name " + sensor_type + "))").c_str());

                if (!engine.sensors.count(sensor_id))
                {
                    LOG_DEBUG("Subscribing to '" + engine.root + SENSOR_TOPIC + '/' + sensor_id + "' topic..");
                    engine.mqtt_client.subscribe(engine.root + SENSOR_TOPIC + '/' + sensor_id, 1);

                    std::string fact_str = "(sensor (id " + sensor_id + ") (sensor_type " + sensor_type + ")";
                    if (j_sensor.has("location"))
                    {
                        json::number_val &j_lat = j_sensor["location"]["lat"];
                        double lat = j_lat;
                        json::number_val &j_lng = j_sensor["location"]["lng"];
                        double lng = j_lng;

                        fact_str += " (location " + std::to_string(lat) + " " + std::to_string(lng) + ")";
                    }
                    fact_str += ')';

                    auto *s_fact = AssertString(engine.env, fact_str.c_str());
                    engine.sensors.emplace(sensor_id, std::make_unique<sensor>(sensor_id, sensor_type, s_fact));
                }
            }
            for (auto it = engine.sensors.begin(); it != engine.sensors.end();)
                if (!sensors_set.count(it->first))
                {
                    LOG_DEBUG("Unsubscribing from '" + engine.root + SENSOR_TOPIC + '/' + it->first + "' topic..");
                    engine.mqtt_client.unsubscribe(engine.root + SENSOR_TOPIC + '/' + it->first);

                    Retract(engine.sensors.at(it->first)->get_fact());
                    engine.sensors.erase(it->first);
                }
                else
                    ++it;
        }
        else if (msg->get_topic().rfind(engine.root + SENSOR_TOPIC + '/', 0) == 0)
        { // we have a new sensor value..
            std::string sensor_id = msg->get_topic();
            sensor_id.erase(0, (engine.root + SENSOR_TOPIC + '/').length());
            LOG_DEBUG("Sensor " + sensor_id + " has been updated..");
            auto j_value = json::load(msg->get_payload());
            json::object &j_val = j_value;

            auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            std::string fact_str = "(sensor_value (sensor_id " + sensor_id + ") (local_time " + std::to_string(time) + ") (val";
            for (const auto &[id, val] : j_val)
            {
                json::string_val &j_v = val;
                fact_str += ' ';
                fact_str += j_v;
            }
            fact_str += "))";

            AssertString(engine.env, fact_str.c_str());

            auto val = std::make_unique<json::json>(std::move(j_value));
            engine.sensors.at(sensor_id)->set_value(std::move(val));
        }
        else if (msg->get_topic().rfind(engine.root + PARTICIPATORY_TOPIC + '/', 0) == 0)
        { // we have a new participatory data..
            std::string user_id = msg->get_topic();
            user_id.erase(0, (engine.root + PARTICIPATORY_TOPIC + '/').length());

            auto j_participatory = json::load(msg->get_payload());

            json::string_val &j_type = j_participatory["type"];
            std::string participatory_type = j_type;

            LOG_DEBUG("User " + user_id + " has added a new " + participatory_type + " participatory data..");

            auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            std::string fact_str = "(participatory_data (user_id " + user_id + ") (participatory_type " + participatory_type + ") (local_time " + std::to_string(time) + ") (val";
            json::object &j_val = j_participatory["value"];
            for (const auto &[id, val] : j_val)
            {
                json::string_val &j_v = val;
                fact_str += ' ';
                fact_str += j_v;
            }
            fact_str += "))";

            AssertString(engine.env, fact_str.c_str());
        }

        Run(engine.env, -1);
#ifdef VERBOSE_LOG
        Eval(engine.env, "(facts)", NULL);
#endif
    }

    sensor::sensor(const std::string &id, const std::string &type, Fact *fact) : id(id), type(type), fact(fact) {}

    urban_sensing_engine::urban_sensing_engine(const std::string &root, const std::string &server_uri, const std::string &db_uri, const std::string &client_id) : root(root), mqtt_client(server_uri, client_id), db_client(db_uri, root), msg_callback(*this), use_timer(1000, std::bind(&urban_sensing_engine::tick, this)), env(CreateEnvironment())
    {
        options.set_clean_session(true);
        options.set_keep_alive_interval(20);

        mqtt_client.set_callback(msg_callback);

        AddUDF(env, "send_message", "v", 3, 3, "lys", send_message, "send_message", NULL);
        AddUDF(env, "send_map_message", "v", 5, 5, "lydds", send_map_message, "send_map_message", NULL);
        AddUDF(env, "send_bus_message", "v", 5, 5, "lydds", send_bus_message, "send_bus_message", NULL);
        AddUDF(env, "new_solver", "l", 1, 1, "l", new_solver, "new_solver", NULL);
        AddUDF(env, "read_script", "v", 2, 2, "ls", read_script, "read_script", NULL);
        AddUDF(env, "read_files", "v", 2, 2, "lm", read_files, "read_files", NULL);
        AddUDF(env, "delete_solver", "v", 2, 2, "ll", delete_solver, "delete_solver", NULL);
    }
    urban_sensing_engine::~urban_sensing_engine()
    {
        use_timer.stop();
        DestroyEnvironment(env);
    }

    void urban_sensing_engine::connect()
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
    void urban_sensing_engine::init()
    {
        LOG("Loading policy rules..");
        Load(env, "rules/rules.clp");
        Reset(env);

        AssertString(env, ("(configuration (engine_ptr " + std::to_string(reinterpret_cast<uintptr_t>(this)) + "))").c_str());
        Run(env, -1);
#ifdef VERBOSE_LOG
        Eval(env, "(facts)", NULL);
#endif

        use_timer.start();
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

    void urban_sensing_engine::tick()
    {
        for (auto &exec : executors)
            exec->tick();
    }
} // namespace use
