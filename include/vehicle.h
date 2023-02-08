#pragma once

#include "vehicle_type.h"
#include "location.h"
#include <memory>

namespace use
{
  class urban_sensing_engine_db;
  class vehicle
  {
    friend class urban_sensing_engine_db;

  public:
    vehicle(const std::string &id, const vehicle_type &vt, std::unique_ptr<coco::location> l) : id(id), type(vt), loc(std::move(l)) {}

    std::string get_id() const { return id; }
    const vehicle_type &get_type() const { return type; }
    bool has_location() const { return loc.operator bool(); }
    const coco::location &get_location() const { return *loc; }

  private:
    std::string id;
    const vehicle_type &type;
    std::unique_ptr<coco::location> loc;
  };
} // namespace use
