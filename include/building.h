#pragma once

#include "road.h"
#include "clips.h"

namespace use
{
  class urban_sensing_engine;
  class urban_sensing_engine_db;

  class building
  {
    friend class urban_sensing_engine;
    friend class urban_sensing_engine_db;

  public:
    building(const std::string &id, const std::string &name, const road &r, const std::string &address, double state, coco::location_ptr l) : id(id), name(name), r(r), address(address), state(state), loc(std::move(l)) {}

    std::string get_id() const { return id; }
    std::string get_name() const { return name; }
    const road &get_road() const { return r; }
    std::string get_address() const { return address; }
    double get_state() const { return state; }
    const coco::location_ptr &get_location() const { return loc; }

    Fact *get_fact() const { return fact; }

    friend void update_building_state(Environment *env, UDFContext *udfc, UDFValue *out);

  private:
    const std::string id;
    std::string name;
    const road &r;
    std::string address;
    double state;
    coco::location_ptr loc;
    Fact *fact = nullptr;
  };
  using building_ptr = std::unique_ptr<building>;

  inline json::json to_json(const building &b) { return {{"id", b.get_id()}, {"name", b.get_name()}, {"road", b.get_road().get_id()}, {"address", b.get_address()}, {"location", {{"lat", b.get_location()->y}, {"lng", b.get_location()->x}}}}; }
} // namespace use
