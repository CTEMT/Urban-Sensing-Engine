#pragma once

#include "mongo_db.h"
#include "road.h"

namespace use
{
  class urban_sensing_engine_db : public coco::mongo_db
  {
  public:
    urban_sensing_engine_db(const std::string &root = COCO_ROOT, const std::string &mongodb_uri = MONGODB_URI(MONGODB_HOST, MONGODB_PORT));

    void init() override;

    std::string create_road(const std::string &name, std::unique_ptr<coco::location> l);

    std::vector<std::reference_wrapper<road>> get_all_roads();

  private:
    void create_road(const std::string &id, const std::string &name, std::unique_ptr<coco::location> l);

  private:
    mongocxx::v_noabi::collection roads_collection;
    std::unordered_map<std::string, std::unique_ptr<road>> roads;
  };
} // namespace use
