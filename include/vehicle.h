#pragma once

#include "vehicle_type.h"
#include "location.h"

namespace use
{
  class vehicle
  {
    friend class urban_sensing_engine;
    friend class urban_sensing_engine_db;

  public:
    vehicle(const std::string &id, const vehicle_type &vt, coco::location_ptr l) : id(id), type(vt), loc(std::move(l)) {}

    std::string get_id() const { return id; }
    const vehicle_type &get_type() const { return type; }
    bool has_location() const { return loc.operator bool(); }
    const coco::location_ptr &get_location() const { return loc; }

    Fact *get_fact() const { return fact; }

  private:
    std::string id;
    const vehicle_type &type;
    coco::location_ptr loc;
    Fact *fact = nullptr;
  };
  using vehicle_ptr = std::unique_ptr<vehicle>;

  inline json::json to_json(const vehicle &v)
  {
    json::json j{{"id", v.get_id()}, {"type", v.get_type().get_id()}};
    if (v.has_location())
      j["location"] = {{"lat", v.get_location()->y}, {"lng", v.get_location()->x}};
    return j;
  }
} // namespace use
