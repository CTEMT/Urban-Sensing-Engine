#pragma once

#include "road.h"

namespace use
{
  class building
  {
  public:
    building(const std::string &id, const std::string &name, const road &r, const std::string &address, coco::location_ptr l) : id(id), name(name), r(r), address(address), loc(std::move(l)) {}

    std::string get_id() const { return id; }
    std::string get_name() const { return name; }
    const road &get_road() const { return r; }
    std::string get_address() const { return address; }
    const coco::location &get_location() const { return *loc; }

  private:
    const std::string id;
    std::string name;
    const road &r;
    std::string address;
    coco::location_ptr loc;
  };
  using building_ptr = std::unique_ptr<building>;
} // namespace use
