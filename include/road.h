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
    road(const std::string &id, const std::string &name, double state, coco::location_ptr l) : id(id), name(name), state(state), loc(std::move(l)) {}

    std::string get_id() const { return id; }
    std::string get_name() const { return name; }
    double get_state() const { return state; }
    const coco::location_ptr &get_location() const { return loc; }

    Fact *get_fact() const { return fact; }

    friend void update_road_state(Environment *env, UDFContext *udfc, UDFValue *out);

  private:
    const std::string id;
    std::string name;
    double state;
    coco::location_ptr loc;
    Fact *fact = nullptr;
  };
  using road_ptr = std::unique_ptr<road>;

  inline json::json to_json(const road &r) { return {{"id", r.get_id()}, {"name", r.get_name()}, {"location", {{"lat", r.get_location()->y}, {"lng", r.get_location()->x}}}}; }
} // namespace use
