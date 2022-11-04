#include "mongo_client.h"
#include "logging.h"
#include <bsoncxx/json.hpp>

namespace use
{
    mongo_client::mongo_client(const std::string &db_uri, const std::string &root) : conn{mongocxx::uri{db_uri}}, db(conn[root]), sensor_types(db["sensor_types"]), sensor_network(db["sensor_network"]), sensor_data(db["sensor_data"])
    {
        if (!db.has_collection("sensor_types"))
        {
            LOG("Creating sensor_types collection..");
            std::vector<bsoncxx::document::value> documents;
            documents.push_back(bsoncxx::from_json("{\"name\": \"air_monitoring\", \"description\": \"A type of sensor for the air monitoring\"}"));
            documents.push_back(bsoncxx::from_json("{\"name\": \"bus\", \"description\": \"A type of sensor for the monitoring the position of buses\"}"));
            auto ids = sensor_types.insert_many(documents)->inserted_ids();
        }

        if (!db.has_collection("sensor_data"))
        {
            LOG("Creating sensor_data collection..");
            sensor_data.create_index(bsoncxx::from_json("{\"sensorId\": 1, \"dateTime\": 1}"));
        }
    }
} // namespace use
