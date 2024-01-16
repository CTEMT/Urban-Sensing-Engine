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
    occupancy(const std::string &id, coco::location_ptr loc, long italians, long foreigners, long extraregional, long intraregional, long total) : id(id), loc(std::move(loc)), italians(italians), foreigners(foreigners), extraregional(extraregional), intraregional(intraregional), total(total) {}

    std::string get_id() const { return id; }
    const coco::location_ptr &get_location() const { return loc; }
    long get_italians() const { return italians; }
    long get_foreigners() const { return foreigners; }
    long get_extraregional() const { return extraregional; }
    long get_intraregional() const { return intraregional; }
    long get_total() const { return total; }

    Fact *get_fact() const { return fact; }

  private:
    const std::string id;
    coco::location_ptr loc;
    long italians, foreigners, extraregional, intraregional, total;
    Fact *fact = nullptr;
  };
  using occupancy_ptr = std::unique_ptr<occupancy>;
} // namespace use
