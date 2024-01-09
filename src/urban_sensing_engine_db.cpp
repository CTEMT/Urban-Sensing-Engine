#include "urban_sensing_engine_db.h"
#include "logging.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <unordered_set>

namespace use
{
    urban_sensing_engine_db::urban_sensing_engine_db(const std::string &root, const std::string &mongodb_uri) : mongo_db(root, mongodb_uri), users_collection(db["users"]), messages_collection(db["messages"]), intersections_collection(db["intersections"]), roads_collection(db["roads"]), buildings_collection(db["buildings"]), vehicle_types_collection(db["vehicle_types"]), vehicles_collection(db["vehicles"]) {}

    void urban_sensing_engine_db::init()
    {
        mongo_db::init();

        if (!has_sensor_type_by_name(PARTICIPATORY_TYPE))
        {
            LOG("Creating participatory sensor type..");
            std::vector<coco::parameter_ptr> parameters;
            parameters.push_back(std::make_unique<coco::symbol_parameter>("user_id", std::vector<std::string>()));
            parameters.push_back(std::make_unique<coco::float_parameter>("status", 0, 1));
            parameters.push_back(std::make_unique<coco::symbol_parameter>("subject_id", std::vector<std::string>()));
            parameters.push_back(std::make_unique<coco::string_parameter>("details"));
            create_sensor_type(PARTICIPATORY_TYPE, "Participatory sensor type", std::move(parameters));
        }

        users.clear();
        LOG("Retrieving all users..");
        for (const auto &doc : users_collection.find({}))
        {
            std::vector<std::string> skills;
            if (doc.find("skills") != doc.end())
            {
                auto skills_array = doc["skills"].get_array().value;
                for (auto &&s : skills_array)
                    skills.push_back(s.get_string().value.to_string());
            }
            users.emplace(doc["_id"].get_oid().value.to_string(), std::make_unique<user>(doc["_id"].get_oid().value.to_string(), doc["participatory_id"].get_oid().value.to_string(), doc["first_name"].get_string().value.to_string(), doc["last_name"].get_string().value.to_string(), doc["email"].get_string().value.to_string(), doc["password"].get_string().value.to_string(), static_cast<user_role>(doc["role"].get_int32().value), skills));
        }
        LOG("Retrieved " << users.size() << " users..");

        intersections.clear();
        LOG("Retrieving all intersections..");
        for (const auto &doc : intersections_collection.find({}))
            intersections.emplace(doc["_id"].get_oid().value.to_string(), std::make_unique<intersection>(doc["_id"].get_oid().value.to_string(), doc["osm_id"].get_string().value.to_string(), std::make_unique<coco::location>(doc["coordinates"]["x"].get_double().value, doc["coordinates"]["y"].get_double().value)));
        LOG("Retrieved " << intersections.size() << " intersections..");

        roads.clear();
        LOG("Retrieving all roads..");
        for (const auto &doc : roads_collection.find({}))
            roads.emplace(doc["_id"].get_oid().value.to_string(), std::make_unique<road>(doc["_id"].get_oid().value.to_string(), doc["osm_id"].get_string().value.to_string(), doc["name"].get_string().value.to_string(), get_intersection(doc["from"].get_oid().value.to_string()), get_intersection(doc["to"].get_oid().value.to_string()), doc["length"].get_double().value, doc["state"].get_double().value));
        LOG("Retrieved " << roads.size() << " roads..");

        buildings.clear();
        LOG("Retrieving all buildings..");
        for (const auto &doc : buildings_collection.find({}))
            buildings.emplace(doc["_id"].get_oid().value.to_string(), std::make_unique<building>(doc["_id"].get_oid().value.to_string(), doc["name"].get_string().value.to_string(), get_road(doc["road_id"].get_oid().value.to_string()), doc["address"].get_string().value.to_string(), doc["state"].get_double().value, std::make_unique<coco::location>(doc["coordinates"]["x"].get_double().value, doc["coordinates"]["y"].get_double().value)));
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

    std::string urban_sensing_engine_db::create_user(const std::string &first_name, const std::string &last_name, const std::string &email, const std::string &password, const user_role &role, const std::vector<std::string> &skills)
    {
        auto part_sens_id = create_sensor("part_" + email, get_sensor_type_by_name(PARTICIPATORY_TYPE));
        using bsoncxx::builder::basic::kvp;
        auto u_doc = bsoncxx::builder::basic::document{};
        u_doc.append(kvp("participatory_id", bsoncxx::oid{bsoncxx::stdx::string_view{part_sens_id}}));
        u_doc.append(kvp("first_name", first_name));
        u_doc.append(kvp("last_name", last_name));
        u_doc.append(kvp("email", email));
        u_doc.append(kvp("password", password));
        u_doc.append(kvp("role", role));
        if (!skills.empty())
        {
            auto skills_array = bsoncxx::builder::basic::array{};
            for (auto &&s : skills)
                skills_array.append(s);
            u_doc.append(kvp("skills", skills_array));
        }

        auto result = users_collection.insert_one(u_doc.view());
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            users.emplace(id, std::make_unique<user>(id, part_sens_id, first_name, last_name, email, password, role, skills));
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
            users.emplace(id, std::make_unique<user>(id, result->view()["participatory_id"].get_oid().value.to_string(), result->view()["first_name"].get_string().value.to_string(), result->view()["last_name"].get_string().value.to_string(), email, password, static_cast<user_role>(result->view()["role"].get_int32().value)));
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

    void urban_sensing_engine_db::set_user_skills(user &u, const std::vector<std::string> &skills)
    {
        auto skills_array = bsoncxx::builder::basic::array{};
        for (auto &&s : skills)
            skills_array.append(s);
        if (users_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(u.id) << bsoncxx::builder::stream::finalize, bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "skills" << skills_array << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize))
            users.at(u.id)->skills = skills;
    }
    void urban_sensing_engine_db::add_user_skill(user &u, const std::string &skill)
    {
        if (users_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(u.id) << bsoncxx::builder::stream::finalize, bsoncxx::builder::stream::document{} << "$addToSet" << bsoncxx::builder::stream::open_document << "skills" << skill << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize))
            users.at(u.id)->skills.push_back(skill);
    }
    void urban_sensing_engine_db::remove_user_skill(user &u, const std::string &skill)
    {
        if (users_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(u.id) << bsoncxx::builder::stream::finalize, bsoncxx::builder::stream::document{} << "$pull" << bsoncxx::builder::stream::open_document << "skills" << skill << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize))
            users.at(u.id)->skills.erase(std::remove(users.at(u.id)->skills.begin(), users.at(u.id)->skills.end(), skill), users.at(u.id)->skills.end());
    }

    void urban_sensing_engine_db::delete_user(user &u)
    {
        if (users_collection.delete_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(u.id) << bsoncxx::builder::stream::finalize))
            users.erase(u.id);
    }

    std::string urban_sensing_engine_db::create_message(const std::string &level, const user &recipient, const std::string &content, const std::vector<std::string> &answers)
    {
        auto s_doc = bsoncxx::builder::basic::document{};
        s_doc.append(bsoncxx::builder::basic::kvp("level", level));
        s_doc.append(bsoncxx::builder::basic::kvp("recipient_id", bsoncxx::oid{bsoncxx::stdx::string_view{recipient.get_id()}}));
        s_doc.append(bsoncxx::builder::basic::kvp("content", content));
        if (!answers.empty())
        {
            auto answers_array = bsoncxx::builder::basic::array{};
            for (auto &&a : answers)
                answers_array.append(a);
            s_doc.append(bsoncxx::builder::basic::kvp("answers", answers_array));
        }

        auto result = messages_collection.insert_one(s_doc.view());
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            messages.emplace(id, std::make_unique<message>(id, level, recipient, content, answers));
            return id;
        }
        else
            return {};
    }

    void urban_sensing_engine_db::set_message_answer(message &q, const std::string &answer)
    {
        if (messages_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(q.id) << bsoncxx::builder::stream::finalize, bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "answer" << answer << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize))
            q.answer = answer;
    }

    std::string urban_sensing_engine_db::create_intersection(const std::string &osm_id, coco::location_ptr l)
    {
        auto result = intersections_collection.insert_one(bsoncxx::builder::stream::document{} << "osm_id" << osm_id << "coordinates" << bsoncxx::builder::stream::open_document << "x" << l->x << "y" << l->y << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            intersections.emplace(id, std::make_unique<intersection>(id, osm_id, std::move(l)));
            return id;
        }
        else
            return {};
    }

    std::string urban_sensing_engine_db::create_road(const std::string &osm_id, const std::string &name, const intersection &from, const intersection &to, double length, double state)
    {
        auto result = roads_collection.insert_one(bsoncxx::builder::stream::document{} << "osm_id" << osm_id << "name" << name << "from" << bsoncxx::oid{bsoncxx::stdx::string_view{from.get_id()}} << "to" << bsoncxx::oid{bsoncxx::stdx::string_view{to.get_id()}} << "length" << length << "state" << state << bsoncxx::builder::stream::finalize);
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            roads.emplace(id, std::make_unique<road>(id, osm_id, name, std::move(from), std::move(to), length, state));
            return id;
        }
        else
            return {};
    }

    void urban_sensing_engine_db::set_road_state(road &r, const double &state)
    {
        if (roads_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(r.id) << bsoncxx::builder::stream::finalize, bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "state" << state << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize))
            roads.at(r.id)->state = state;
    }

    std::vector<std::reference_wrapper<road>> urban_sensing_engine_db::get_roads(const std::string &filter, const unsigned int limit) const
    {
        std::unordered_set<std::string> names;
        std::vector<std::reference_wrapper<road>> result;
        std::string filter_lower = filter;
        for (auto &&c : filter_lower)
            c = std::tolower(c);
        for (auto &&road : roads)
        {
            std::string name = road.second->get_name();
            for (auto &&c : name)
                c = std::tolower(c);
            if (name.find(filter_lower) != std::string::npos && names.insert(name).second)
                result.push_back(*road.second);
            if (result.size() == limit)
                break;
        }
        return result;
    }

    std::string urban_sensing_engine_db::create_building(const std::string &name, const road &r, const std::string &address, const double &state, coco::location_ptr l)
    {
        auto result = buildings_collection.insert_one(bsoncxx::builder::stream::document{} << "name" << name << "road_id" << bsoncxx::oid{bsoncxx::stdx::string_view{r.get_id()}} << "address" << address << "state" << state << "coordinates" << bsoncxx::builder::stream::open_document << "x" << l->x << "y" << l->y << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
        if (result)
        {
            auto id = result->inserted_id().get_oid().value.to_string();
            buildings.emplace(id, std::make_unique<building>(id, name, r, address, state, std::move(l)));
            return id;
        }
        else
            return {};
    }

    void urban_sensing_engine_db::set_building_state(building &b, const double &state)
    {
        if (buildings_collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid(b.id) << bsoncxx::builder::stream::finalize, bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << "state" << state << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize))
            buildings.at(b.id)->state = state;
    }

    std::vector<std::reference_wrapper<building>> urban_sensing_engine_db::get_buildings(const std::string &filter, const unsigned int limit) const
    {
        std::vector<std::reference_wrapper<building>> result;
        std::string filter_lower = filter;
        for (auto &&c : filter_lower)
            c = std::tolower(c);
        for (auto &&building : buildings)
        {
            std::string name = building.second->get_name();
            for (auto &&c : name)
                c = std::tolower(c);
            if (name.find(filter_lower) != std::string::npos)
                result.push_back(*building.second);
            if (result.size() == limit)
                break;
        }
        return result;
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