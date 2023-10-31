#pragma once

#include "location.h"

namespace use
{
  class road
  {
  public:
    road(const std::string &id, const std::string &name, coco::location_ptr l) : id(id), name(name), loc(std::move(l)) {}

    std::string get_id() const { return id; }
    std::string get_name() const { return name; }
    const coco::location &get_location() const { return *loc; }

  private:
    const std::string id;
    std::string name;
    coco::location_ptr loc;
  };
  using road_ptr = std::unique_ptr<road>;

  inline json::json to_json(const road &r) { return {{"id", r.get_id()}, {"name", r.get_name()}, {"location", to_json(r.get_location())}}; }
} // namespace use
