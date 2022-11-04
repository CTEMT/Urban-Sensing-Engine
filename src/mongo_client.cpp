#include "mongo_client.h"
#include "logging.h"
#include <bsoncxx/json.hpp>
#include <sstream>

namespace use
{
    mongo_client::mongo_client(const std::string &db_uri, const std::string &root) : conn{mongocxx::uri{db_uri}}, db(conn[root]), sensor_types(db["sensor_types"]), sensor_network(db["sensor_network"]), sensor_data(db["sensor_data"])
    {
#ifdef DROP_DATABASE
        db.drop();
#endif

        if (!db.has_collection("sensor_types"))
        {
            LOG("Creating sensor_types collection..");
            std::vector<bsoncxx::document::value> documents;
            sensor_type bus = {"", "bus", "A type of sensor for the monitoring the position of buses"};
            sensor_type air_monitoring = {"", "air_monitoring", "A type of sensor for the air monitoring"};
            sensor_type temperature = {"", "temperature", "A type of sensor for the monitoring the temperature"};
            documents.push_back(bsoncxx::from_json(to_json(bus).dump()));
            documents.push_back(bsoncxx::from_json(to_json(air_monitoring).dump()));
            documents.push_back(bsoncxx::from_json(to_json(temperature).dump()));
            auto ids = sensor_types.insert_many(documents)->inserted_ids();
            sensor_types_ids.emplace(ids.at(0).get_string().value.to_string(), bus);
            sensor_types_ids.emplace(ids.at(1).get_string().value.to_string(), air_monitoring);
            sensor_types_ids.emplace(ids.at(2).get_string().value.to_string(), temperature);
        }
        else
        {
            LOG("Retrieving all sensor types..");
            for (auto doc : sensor_types.find({}))
            {
                std::stringstream ss;
                ss << bsoncxx::to_json(doc);
                auto j_st = json::load(ss);
                LOG_DEBUG("Found existing sensor type: " + j_st.dump());

                json::string_val &j_id = j_st["_id"]["$oid"];
                std::string id = j_id;
                json::string_val &j_name = j_st["name"];
                std::string name = j_name;
                json::string_val &j_description = j_st["description"];
                std::string description = j_description;
                sensor_type st = {id, name, description};
                sensor_types_ids.emplace(id, st);
            }
        }

        if (!db.has_collection("sensor_data"))
        {
            LOG("Creating sensor_data collection..");
            sensor_data.create_index(bsoncxx::from_json("{\"sensorId\": 1, \"dateTime\": 1}"));
        }
    }

    json::json mongo_client::to_json(const sensor_type &st) noexcept
    {
        json::json j_st;
        if (!st.id.empty())
            j_st["_id"] = st.id;
        j_st["name"] = st.name;
        j_st["description"] = st.description;
        return j_st;
    }
} // namespace use
