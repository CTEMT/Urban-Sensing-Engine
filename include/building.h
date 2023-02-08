#pragma once

#include "road.h"
#include <memory>

namespace use
{
  class building
  {
  public:
    building(const std::string &id, const std::string &name, const road &r, const std::string &address, std::unique_ptr<coco::location> l) : id(id), name(name), r(r), address(address), loc(std::move(l)) {}

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
    std::unique_ptr<coco::location> loc;
  };
} // namespace use
