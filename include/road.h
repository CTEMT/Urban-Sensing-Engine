#pragma once

#include "location.h"
#include "clips.h"

namespace use
{
  class urban_sensing_engine;
  class urban_sensing_engine_db;

  class intersection
  {
    friend class urban_sensing_engine;
    friend class urban_sensing_engine_db;

  public:
    intersection(const std::string &id, const std::string &osm_id, coco::location_ptr l) : id(id), osm_id(osm_id), loc(std::move(l)) {}

    std::string get_id() const { return id; }
    std::string get_osm_id() const { return osm_id; }
    const coco::location_ptr &get_location() const { return loc; }

    Fact *get_fact() const { return fact; }

  private:
    const std::string id, osm_id;
    coco::location_ptr loc;
    Fact *fact = nullptr;
  };
  using intersection_ptr = std::unique_ptr<intersection>;

  class road
  {
    friend class urban_sensing_engine;
    friend class urban_sensing_engine_db;

  public:
    road(const std::string &id, const std::string &osm_id, const std::string &name, const intersection &from, const intersection &to, double length, double state) : id(id), osm_id(osm_id), name(name), from(from), to(to), length(length), state(state) {}

    std::string get_id() const { return id; }
    std::string get_osm_id() const { return osm_id; }
    std::string get_name() const { return name; }
    const intersection &get_from() const { return from; }
    const intersection &get_to() const { return to; }
    double get_length() const { return length; }
    double get_state() const { return state; }

    Fact *get_fact() const { return fact; }

    friend void update_road_state(Environment *env, UDFContext *udfc, UDFValue *out);

  private:
    const std::string id, osm_id;
    std::string name;
    const intersection &from;
    const intersection &to;
    double length;
    double state;
    Fact *fact = nullptr;
  };
  using road_ptr = std::unique_ptr<road>;

  inline json::json to_json(const intersection &i) { return {{"id", i.get_id()}, {"osm_id", i.get_osm_id()}, {"location", {"x", i.get_location()->x, "y", i.get_location()->y}}}; }
  inline json::json to_json(const road &r) { return {{"id", r.get_id()}, {"osm_id", r.get_osm_id()}, {"name", r.get_name()}, {"from", r.get_from().get_id()}, {"to", r.get_to().get_id()}, {"length", r.get_length()}, {"state", r.get_state()}}; }
} // namespace use
