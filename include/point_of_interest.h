#pragma once

#include "location.h"
#include "clips.h"

namespace use
{
  class urban_sensing_engine;
  class urban_sensing_engine_db;

  class point_of_interest
  {
    friend class urban_sensing_engine;
    friend class urban_sensing_engine_db;

  public:
    point_of_interest(const std::string &id, const std::string &osm_id, const std::string &ui_id, const std::string &name, const std::string &type, long opening, long closing, coco::location_ptr l, json::json polygon) : id(id), osm_id(osm_id), ui_id(ui_id), name(name), type(type), opening(opening), closing(closing), loc(std::move(l)), polygon(std::move(polygon)) {}

    std::string get_id() const { return id; }
    std::string get_osm_id() const { return osm_id; }
    std::string get_ui_id() const { return ui_id; }
    std::string get_name() const { return name; }
    std::string get_type() const { return type; }
    long get_opening() const { return opening; }
    long get_closing() const { return closing; }
    const coco::location_ptr &get_location() const { return loc; }
    json::json get_polygon() const { return polygon; }

    Fact *get_fact() const { return fact; }

  private:
    const std::string id, osm_id, ui_id;
    std::string name, type;
    long opening, closing;
    coco::location_ptr loc;
    json::json polygon;
    Fact *fact = nullptr;
  };
  using point_of_interest_ptr = std::unique_ptr<point_of_interest>;

  inline json::json to_json(const point_of_interest &p)
  {
    json::json j;
    j["id"] = p.get_id();
    j["osm_id"] = p.get_osm_id();
    j["ui_id"] = p.get_ui_id();
    j["name"] = p.get_name();
    j["type"] = p.get_type();
    j["opening"] = p.get_opening();
    j["closing"] = p.get_closing();
    j["location"] = coco::to_json(*p.get_location());
    j["polygon"] = p.get_polygon();
    return j;
  }
} // namespace use