#include "urban_sensing_engine_db.h"
#include "logging.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>

namespace use
{
    urban_sensing_engine_db::urban_sensing_engine_db(const std::string &root, const std::string &mongodb_uri) : mongo_db(root, mongodb_uri), roads_collection(db["roads"]) {}

    void urban_sensing_engine_db::init()
    {
        mongo_db::init();
        roads.clear();
        LOG("Retrieving all roads..");
        for (const auto &doc : roads_collection.find({}))
        {
            std::unique_ptr<coco::location> l;
            l->x = doc["coordinates"]["x"].get_double().value;
            l->y = doc["coordinates"]["y"].get_double().value;
            create_road(doc["_id"].get_oid().value.to_string(), doc["name"].get_string().value.to_string(), std::move(l));
        }
        LOG("Retrieved " << roads.size() << " roads..");
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

    std::vector<std::reference_wrapper<road>> urban_sensing_engine_db::get_all_roads()
    {
        std::vector<std::reference_wrapper<road>> result;
        for (auto &&road : roads)
            result.push_back(*road.second);
        return result;
    }
} // namespace use