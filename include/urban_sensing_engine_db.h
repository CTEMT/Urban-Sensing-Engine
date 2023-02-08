#pragma once

#include "mongo_db.h"
#include "road.h"
#include "building.h"

namespace use
{
  class urban_sensing_engine_db : public coco::mongo_db
  {
  public:
    urban_sensing_engine_db(const std::string &root = COCO_ROOT, const std::string &mongodb_uri = MONGODB_URI(MONGODB_HOST, MONGODB_PORT));

    void init() override;

    std::string create_road(const std::string &name, std::unique_ptr<coco::location> l);
    road &get_road(const std::string &id);

    std::vector<std::reference_wrapper<road>> get_all_roads();

    std::string create_building(const std::string &name, const road &r, const std::string &address, std::unique_ptr<coco::location> l);
    building &get_building(const std::string &id);

    std::vector<std::reference_wrapper<building>> get_all_buildings();

  private:
    void create_road(const std::string &id, const std::string &name, std::unique_ptr<coco::location> l);
    void create_building(const std::string &id, const std::string &name, const road &r, const std::string &address, std::unique_ptr<coco::location> l);

  private:
    mongocxx::v_noabi::collection roads_collection;
    mongocxx::v_noabi::collection buildings_collection;
    std::unordered_map<std::string, std::unique_ptr<road>> roads;
    std::unordered_map<std::string, std::unique_ptr<building>> buildings;
  };
} // namespace use
