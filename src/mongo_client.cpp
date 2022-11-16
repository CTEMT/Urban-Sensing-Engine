#include "mongo_client.h"
#include "logging.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <sstream>

namespace use
{
    mongo_client::mongo_client(const std::string &db_uri, const std::string &root) : conn{mongocxx::uri{db_uri}}, db(conn[root]), sensor_types(db["sensor_types"]), sensor_network(db["sensor_network"]), sensor_data(db["sensor_data"])
    {
#ifdef DROP_DATABASE
        LOG("Dropping database..");
        db.drop();

        LOG("Creating sensor_types collection..");
        create_sensor_type("bus", "A type of sensor for the monitoring the position of buses");
        create_sensor_type("air_monitoring", "A type of sensor for the air monitoring");
        create_sensor_type("temperature", "A type of sensor for the monitoring the temperature");
#endif

        LOG("Retrieving all sensor types..");
        for (auto doc : sensor_types.find({}))
        {
            std::string id = doc["_id"].get_oid().value.to_string();
            sensor_type st = {id, doc["name"].get_string().value.to_string(), doc["description"].get_string().value.to_string()};
            sensor_types_ids.emplace(id, st);
        }

        if (!db.has_collection("sensor_data"))
        {
            LOG("Creating sensor_data collection..");
            sensor_data.create_index(bsoncxx::from_json("{\"sensorId\": 1, \"dateTime\": 1}"));
        }
    }

    void mongo_client::create_sensor_type(const std::string &name, const std::string &description)
    {
        LOG("Creating new sensor type..");
        auto result = sensor_types.insert_one(bsoncxx::builder::stream::document{} << "name" << name << "description" << description << bsoncxx::builder::stream::finalize);
        if (result)
        {
            auto id = result.value().inserted_id();
            sensor_type st = {id.get_string().value.to_string(), name, description};
            sensor_types_ids.emplace(st.id, st);
            LOG("Sensor type created..");
        }
    }

    void mongo_client::update_sensor_type(const std::string &id, const std::string &name, const std::string &description)
    {
        LOG("Updating sensor type..");
        auto result = sensor_types.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid{bsoncxx::stdx::string_view{id}} << bsoncxx::builder::stream::finalize,
                                              bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "name" << name << "description" << description << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
        if (result)
        {
            sensor_types_ids[id].name = name;
            sensor_types_ids[id].description = description;
            LOG("Sensor type updated..");
        }
    }

    void mongo_client::delete_sensor_type(const std::string &id)
    {
        LOG("Deleting sensor type..");
        auto result = sensor_types.delete_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid{bsoncxx::stdx::string_view{id}} << bsoncxx::builder::stream::finalize);
        if (result)
        {
            sensor_types_ids.erase(id);
            LOG("Sensor type deleted..");
        }
    }
} // namespace use
