#include "urban_sensing_engine_db.h"
#include "logging.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>

namespace use
{
    urban_sensing_engine_db::urban_sensing_engine_db(const std::string &root, const std::string &mongodb_uri) : mongo_db(root, mongodb_uri), roads_collection(db["roads"]), buildings_collection(db["buildings_collection"]), vehicle_types_collection(db["vehicle_types"]), vehicles_collection(db["vehicles"]) {}

    void urban_sensing_engine_db::init()
    {
        mongo_db::init();
        roads.clear();
        LOG("Retrieving all roads..");
        for (const auto &doc : roads_collection.find({}))
        {
            auto l = std::make_unique<coco::location>();
            l->x = doc["coordinates"]["x"].get_double().value;
            l->y = doc["coordinates"]["y"].get_double().value;
            create_road(doc["_id"].get_oid().value.to_string(), doc["name"].get_string().value.to_string(), std::move(l));
        }
        LOG("Retrieved " << roads.size() << " roads..");

        buildings.clear();
        LOG("Retrieving all buildings..");
        for (const auto &doc : buildings_collection.find({}))
        {
            auto l = std::make_unique<coco::location>();
            l->x = doc["coordinates"]["x"].get_double().value;
            l->y = doc["coordinates"]["y"].get_double().value;
            create_building(doc["_id"].get_oid().value.to_string(), doc["name"].get_string().value.to_string(), get_road(doc["road_id"].get_oid().value.to_string()), doc["address"].get_string().value.to_string(), std::move(l));
        }
        LOG("Retrieved " << buildings.size() << " buildings..");

        vehicle_types.clear();
        LOG("Retrieving all vehicle types..");
        for (const auto &doc : vehicle_types_collection.find({}))
        {
            create_vehicle_type(doc["_id"].get_oid().value.to_string(), doc["name"].get_string().value.to_string(), doc["description"].get_string().value.to_string(), doc["manufacturer"].get_string().value.to_string());
        }
        LOG("Retrieved " << vehicle_types.size() << " vehicle types..");

        vehicles.clear();
        LOG("Retrieving all vehicles..");
        for (const auto &doc : vehicles_collection.find({}))
        {
            auto loc = doc.find("location");
            std::unique_ptr<coco::location> l;
            if (loc != doc.end())
            {
                auto loc_doc = loc->get_document().value;
                auto x = loc_doc["x"].get_double().value;
                auto y = loc_doc["y"].get_double().value;
                l = std::make_unique<coco::location>();
                l->x = x;
                l->y = y;
            }
            create_vehicle(doc["_id"].get_oid().value.to_string(), get_vehicle_type(doc["vehicle_type_id"].get_oid().value.to_string()), std::move(l));
        }
        LOG("Retrieved " << vehicles.size() << " vehicles..");
    }

    std::string urban_sensing_engine_db::create_road(const std::string &name, std::unique_ptr<coco::location> l)
    {
        auto result = roads_collection.insert_one(bsoncxx::builder::stream::document{} << "name" << name << "coordinates" << bsoncxx::builder::stream::open_document << "x" << l->x << "y" << l->y << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            create_road(id, name, std::move(l));
            return id;
        }
        else
            return {};
    }

    void urban_sensing_engine_db::create_road(const std::string &id, const std::string &name, std::unique_ptr<coco::location> l) { roads.emplace(id, std::make_unique<use::road>(id, name, std::move(l))); }

    road &urban_sensing_engine_db::get_road(const std::string &id) { return *roads.at(id); }

    std::vector<std::reference_wrapper<road>> urban_sensing_engine_db::get_all_roads()
    {
        std::vector<std::reference_wrapper<road>> result;
        for (auto &&road : roads)
            result.push_back(*road.second);
        return result;
    }

    std::string urban_sensing_engine_db::create_building(const std::string &name, const road &r, const std::string &address, std::unique_ptr<coco::location> l)
    {
        auto result = buildings_collection.insert_one(bsoncxx::builder::stream::document{} << "name" << name << "road_id" << bsoncxx::oid{bsoncxx::stdx::string_view{r.get_id()}} << "address" << address << "coordinates" << bsoncxx::builder::stream::open_document << "x" << l->x << "y" << l->y << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            create_building(id, name, r, address, std::move(l));
            return id;
        }
        else
            return {};
    }

    void urban_sensing_engine_db::create_building(const std::string &id, const std::string &name, const road &r, const std::string &address, std::unique_ptr<coco::location> l) { buildings.emplace(id, std::make_unique<use::building>(id, name, r, address, std::move(l))); }

    std::vector<std::reference_wrapper<building>> urban_sensing_engine_db::get_all_buildings()
    {
        std::vector<std::reference_wrapper<building>> result;
        for (auto &&building : buildings)
            result.push_back(*building.second);
        return result;
    }

    building &urban_sensing_engine_db::get_building(const std::string &id) { return *buildings.at(id); }

    std::string urban_sensing_engine_db::create_vehicle_type(const std::string &name, const std::string &description, const std::string &manufacturer)
    {
        auto result = vehicle_types_collection.insert_one(bsoncxx::builder::stream::document{} << "name" << name << "description" << description << "manufacturer" << manufacturer << bsoncxx::builder::stream::finalize);
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            create_vehicle_type(id, name, description, manufacturer);
            return id;
        }
        else
            return {};
    }

    void urban_sensing_engine_db::create_vehicle_type(const std::string &id, const std::string &name, const std::string &description, const std::string &manufacturer) { vehicle_types.emplace(id, std::make_unique<use::vehicle_type>(id, name, description, manufacturer)); }

    vehicle_type &urban_sensing_engine_db::get_vehicle_type(const std::string &id) { return *vehicle_types.at(id); }

    std::vector<std::reference_wrapper<vehicle_type>> urban_sensing_engine_db::get_all_vehicle_types()
    {
        std::vector<std::reference_wrapper<vehicle_type>> result;
        for (auto &&vehicle_type : vehicle_types)
            result.push_back(*vehicle_type.second);
        return result;
    }

    std::string urban_sensing_engine_db::create_vehicle(const vehicle_type &vt, std::unique_ptr<coco::location> l)
    {
        auto s_doc = bsoncxx::builder::basic::document{};
        s_doc.append(bsoncxx::builder::basic::kvp("vehicle_type_id", bsoncxx::oid{bsoncxx::stdx::string_view{vt.get_id()}}));
        if (l)
            s_doc.append(bsoncxx::builder::basic::kvp("location", [&l](bsoncxx::builder::basic ::sub_document subdoc)
                                                      { subdoc.append(bsoncxx::builder::basic::kvp("x", l->x), bsoncxx::builder::basic::kvp("y", l->y)); }));

        auto result = vehicles_collection.insert_one(s_doc.view());
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            create_vehicle(id, vt, std::move(l));
            return id;
        }
        else
            return {};
    }

    void urban_sensing_engine_db::create_vehicle(const std::string &id, const vehicle_type &vt, std::unique_ptr<coco::location> l) { vehicles.emplace(id, std::make_unique<use::vehicle>(id, vt, std::move(l))); }

    vehicle &urban_sensing_engine_db::get_vehicle(const std::string &id) { return *vehicles.at(id); }

    std::vector<std::reference_wrapper<vehicle>> urban_sensing_engine_db::get_all_vehicles()
    {
        std::vector<std::reference_wrapper<vehicle>> result;
        for (auto &&vehicle : vehicles)
            result.push_back(*vehicle.second);
        return result;
    }

    void urban_sensing_engine_db::set_vehicle_location(vehicle &v, std::unique_ptr<coco::location> l)
    {
        if (l)
        {
            auto result = vehicles_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid{bsoncxx::stdx::string_view{v.get_id()}} << bsoncxx::builder::stream::finalize,
                                                         bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "location" << bsoncxx::builder::stream::open_document << "x" << l->x << "y" << l->y << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
            if (result)
                v.loc.swap(l);
        }
        else
        {
            auto result = vehicles_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid{bsoncxx::stdx::string_view{v.get_id()}} << bsoncxx::builder::stream::finalize,
                                                         bsoncxx::builder::stream::document{} << "$unset" << bsoncxx::builder::stream::open_document << "location"
                                                                                              << "" << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
            if (result)
                v.loc = nullptr;
        }
    }
} // namespace use