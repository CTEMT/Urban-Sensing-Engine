#pragma once

#include "location.h"
#include "clips.h"

namespace use
{
  class urban_sensing_engine;
  class urban_sensing_engine_db;

  class occupancy
  {
    friend class urban_sensing_engine;
    friend class urban_sensing_engine_db;

  public:
    occupancy(const std::string &id, coco::location_ptr loc, long italians, long foreigners, long extraregional, long intraregional, long total) : id(id), loc(std::move(loc)), italians(italians), foreigners(foreigners), extraregionals(extraregional), intraregionals(intraregional), total(total) {}

    std::string get_id() const { return id; }
    const coco::location_ptr &get_location() const { return loc; }
    long get_italians() const { return italians; }
    long get_foreigners() const { return foreigners; }
    long get_extraregionals() const { return extraregionals; }
    long get_intraregionals() const { return intraregionals; }
    long get_total() const { return total; }

    Fact *get_fact() const { return fact; }

  private:
    const std::string id;
    coco::location_ptr loc;
    long italians, foreigners, extraregionals, intraregionals, total;
    Fact *fact = nullptr;
  };
  using occupancy_ptr = std::unique_ptr<occupancy>;

  inline json::json to_json(const occupancy &o) { return json::json{{"id", o.get_id()}, {"location", coco::to_json(*o.get_location())}, {"italians", o.get_italians()}, {"foreigners", o.get_foreigners()}, {"extraregionals", o.get_extraregionals()}, {"intraregionals", o.get_intraregionals()}, {"total", o.get_total()}}; }
} // namespace use
