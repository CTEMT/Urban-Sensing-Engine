#pragma once

#include "mongo_db.h"
#include "road.h"
#include "building.h"
#include "vehicle_type.h"
#include "vehicle.h"

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

    std::string create_vehicle_type(const std::string &name, const std::string &description, const std::string &manufacturer);
    vehicle_type &get_vehicle_type(const std::string &id);

    std::vector<std::reference_wrapper<vehicle_type>> get_all_vehicle_types();

    std::string create_vehicle(const vehicle_type &vt, std::unique_ptr<coco::location> l);
    vehicle &get_vehicle(const std::string &id);

    std::vector<std::reference_wrapper<vehicle>> get_all_vehicles();

    void set_vehicle_location(vehicle &v, std::unique_ptr<coco::location> l);

  private:
    void create_road(const std::string &id, const std::string &name, std::unique_ptr<coco::location> l);
    void create_building(const std::string &id, const std::string &name, const road &r, const std::string &address, std::unique_ptr<coco::location> l);
    void create_vehicle_type(const std::string &id, const std::string &name, const std::string &description, const std::string &manufacturer);
    void create_vehicle(const std::string &id, const vehicle_type &vt, std::unique_ptr<coco::location> l);

  private:
    mongocxx::v_noabi::collection roads_collection;
    mongocxx::v_noabi::collection buildings_collection;
    mongocxx::v_noabi::collection vehicle_types_collection;
    mongocxx::v_noabi::collection vehicles_collection;
    std::unordered_map<std::string, std::unique_ptr<road>> roads;
    std::unordered_map<std::string, std::unique_ptr<building>> buildings;
    std::unordered_map<std::string, std::unique_ptr<vehicle_type>> vehicle_types;
    std::unordered_map<std::string, std::unique_ptr<vehicle>> vehicles;
  };
} // namespace use
