#pragma once

#include "location.h"
#include "clips.h"

namespace use
{
  class urban_sensing_engine;
  class urban_sensing_engine_db;

  class road
  {
    friend class urban_sensing_engine;
    friend class urban_sensing_engine_db;

  public:
    road(const std::string &id, const std::string &name, coco::location_ptr l) : id(id), name(name), loc(std::move(l)) {}

    std::string get_id() const { return id; }
    std::string get_name() const { return name; }
    const coco::location_ptr &get_location() const { return loc; }

    Fact *get_fact() const { return fact; }

  private:
    const std::string id;
    std::string name;
    coco::location_ptr loc;
    Fact *fact = nullptr;
  };
  using road_ptr = std::unique_ptr<road>;

  inline json::json to_json(const road &r) { return {{"id", r.get_id()}, {"name", r.get_name()}, {"location", {{"lat", r.get_location()->y}, {"lng", r.get_location()->x}}}}; }
} // namespace use
