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

        users.clear();
        LOG("Retrieving all users..");
        for (const auto &doc : users_collection.find({}))
            users.emplace(doc["_id"].get_oid().value.to_string(), std::make_unique<user>(doc["_id"].get_oid().value.to_string(), doc["first_name"].get_string().value.to_string(), doc["last_name"].get_string().value.to_string(), doc["email"].get_string().value.to_string(), doc["password"].get_string().value.to_string(), static_cast<user_role>(doc["role"].get_int32().value)));
        LOG("Retrieved " << users.size() << " users..");

        roads.clear();
        LOG("Retrieving all roads..");
        for (const auto &doc : roads_collection.find({}))
            roads.emplace(doc["_id"].get_oid().value.to_string(), std::make_unique<road>(doc["_id"].get_oid().value.to_string(), doc["name"].get_string().value.to_string(), std::make_unique<coco::location>(doc["coordinates"]["x"].get_double().value, doc["coordinates"]["y"].get_double().value)));
        LOG("Retrieved " << roads.size() << " roads..");

        buildings.clear();
        LOG("Retrieving all buildings..");
        for (const auto &doc : buildings_collection.find({}))
            buildings.emplace(doc["_id"].get_oid().value.to_string(), std::make_unique<building>(doc["_id"].get_oid().value.to_string(), doc["name"].get_string().value.to_string(), get_road(doc["road_id"].get_oid().value.to_string()), doc["address"].get_string().value.to_string(), std::make_unique<coco::location>(doc["coordinates"]["x"].get_double().value, doc["coordinates"]["y"].get_double().value)));
        LOG("Retrieved " << buildings.size() << " buildings..");

        vehicle_types.clear();
        LOG("Retrieving all vehicle types..");
        for (const auto &doc : vehicle_types_collection.find({}))
            vehicle_types.emplace(doc["_id"].get_oid().value.to_string(), std::make_unique<vehicle_type>(doc["_id"].get_oid().value.to_string(), doc["name"].get_string().value.to_string(), doc["description"].get_string().value.to_string(), doc["manufacturer"].get_string().value.to_string()));
        LOG("Retrieved " << vehicle_types.size() << " vehicle types..");

        vehicles.clear();
        LOG("Retrieving all vehicles..");
        for (const auto &doc : vehicles_collection.find({}))
        {
            auto loc = doc.find("location");
            coco::location_ptr l;
            if (loc != doc.end())
                l = std::make_unique<coco::location>(loc->get_document().value["x"].get_double().value, loc->get_document().value["y"].get_double().value);
            vehicles.emplace(doc["_id"].get_oid().value.to_string(), std::make_unique<vehicle>(doc["_id"].get_oid().value.to_string(), get_vehicle_type(doc["vehicle_type_id"].get_oid().value.to_string()), std::move(l)));
        }
        LOG("Retrieved " << vehicles.size() << " vehicles..");
    }

    std::string urban_sensing_engine_db::create_user(const std::string &first_name, const std::string &last_name, const std::string &email, const std::string &password, const user_role &role)
    {
        using bsoncxx::builder::basic::kvp;
        auto u_doc = bsoncxx::builder::basic::document{};
        u_doc.append(kvp("first_name", first_name));
        u_doc.append(kvp("last_name", last_name));
        u_doc.append(kvp("email", email));
        u_doc.append(kvp("password", password));
        u_doc.append(kvp("role", role));

        auto result = users_collection.insert_one(u_doc.view());
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            users.emplace(id, std::make_unique<user>(id, first_name, last_name, email, password, role));
            return id;
        }
        else
            return {};
    }

    std::string urban_sensing_engine_db::get_user(const std::string &email, const std::string &password)
    {
        auto result = users_collection.find_one(bsoncxx::builder::stream::document{} << "email" << email << "password" << password << bsoncxx::builder::stream::finalize);
        if (result)
        {
            auto id = result->view()["_id"].get_oid().value.to_string();
            users.emplace(id, std::make_unique<user>(id, result->view()["first_name"].get_string().value.to_string(), result->view()["last_name"].get_string().value.to_string(), email, password, static_cast<user_role>(result->view()["role"].get_int32().value)));
            return id;
        }
        else
            return {};
    }

    void urban_sensing_engine_db::set_user_role(user &u, const user_role &role)
    {
        if (users_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(u.id) << bsoncxx::builder::stream::finalize, bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "role" << role << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize))
            users.at(u.id)->role = role;
    }

    void urban_sensing_engine_db::set_user_first_name(user &u, const std::string &first_name)
    {
        if (users_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(u.id) << bsoncxx::builder::stream::finalize, bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "first_name" << first_name << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize))
            users.at(u.id)->first_name = first_name;
    }

    void urban_sensing_engine_db::set_user_last_name(user &u, const std::string &last_name)
    {
        if (users_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(u.id) << bsoncxx::builder::stream::finalize, bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "last_name" << last_name << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize))
            users.at(u.id)->last_name = last_name;
    }

    void urban_sensing_engine_db::set_user_email(user &u, const std::string &email)
    {
        if (users_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(u.id) << bsoncxx::builder::stream::finalize, bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "email" << email << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize))
            users.at(u.id)->email = email;
    }

    void urban_sensing_engine_db::set_user_password(user &u, const std::string &password)
    {
        if (users_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(u.id) << bsoncxx::builder::stream::finalize, bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "password" << password << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize))
            users.at(u.id)->password = password;
    }

    void urban_sensing_engine_db::delete_user(user &u)
    {
        if (users_collection.delete_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(u.id) << bsoncxx::builder::stream::finalize))
            users.erase(u.id);
    }

    std::string urban_sensing_engine_db::create_road(const std::string &name, coco::location_ptr l)
    {
        auto result = roads_collection.insert_one(bsoncxx::builder::stream::document{} << "name" << name << "coordinates" << bsoncxx::builder::stream::open_document << "x" << l->x << "y" << l->y << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            roads.emplace(id, std::make_unique<road>(id, name, std::move(l)));
            return id;
        }
        else
            return {};
    }

    std::vector<std::reference_wrapper<road>> urban_sensing_engine_db::get_roads(const std::string &filter, const unsigned int limit)
    {
        std::vector<std::reference_wrapper<road>> result;
        std::string filter_lower = filter;
        for (auto &&c : filter_lower)
            c = std::tolower(c);
        for (auto &&road : roads)
        {
            std::string name = road.second->get_name();
            for (auto &&c : name)
                c = std::tolower(c);
            if (name.find(filter_lower) != std::string::npos)
                result.push_back(*road.second);
            if (result.size() == limit)
                break;
        }
        return result;
    }

    std::string urban_sensing_engine_db::create_building(const std::string &name, const road &r, const std::string &address, coco::location_ptr l)
    {
        auto result = buildings_collection.insert_one(bsoncxx::builder::stream::document{} << "name" << name << "road_id" << bsoncxx::oid{bsoncxx::stdx::string_view{r.get_id()}} << "address" << address << "coordinates" << bsoncxx::builder::stream::open_document << "x" << l->x << "y" << l->y << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            buildings.emplace(id, std::make_unique<building>(id, name, r, address, std::move(l)));
            return id;
        }
        else
            return {};
    }

    std::string urban_sensing_engine_db::create_vehicle_type(const std::string &name, const std::string &description, const std::string &manufacturer)
    {
        auto result = vehicle_types_collection.insert_one(bsoncxx::builder::stream::document{} << "name" << name << "description" << description << "manufacturer" << manufacturer << bsoncxx::builder::stream::finalize);
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            vehicle_types.emplace(id, std::make_unique<vehicle_type>(id, name, description, manufacturer));
            return id;
        }
        else
            return {};
    }

    std::string urban_sensing_engine_db::create_vehicle(const vehicle_type &vt, coco::location_ptr l)
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
            vehicles.emplace(id, std::make_unique<vehicle>(id, vt, std::move(l)));
            return id;
        }
        else
            return {};
    }

    void urban_sensing_engine_db::set_vehicle_location(vehicle &v, coco::location_ptr l)
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