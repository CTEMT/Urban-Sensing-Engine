#pragma once

#include "memory.h"
#include <string>

namespace use
{
  class vehicle_type
  {
  public:
    vehicle_type(const std::string &id, const std::string &name, const std::string &description, const std::string &manufacturer) : id(id), name(name), description(description), manufacturer(manufacturer) {}

    std::string get_id() const { return id; }
    std::string get_name() const { return name; }
    std::string get_description() const { return description; }
    std::string get_manufacturer() const { return manufacturer; }

  private:
    std::string id;
    std::string name;
    std::string description;
    std::string manufacturer;
  };
  using vehicle_type_ptr = utils::u_ptr<vehicle_type>;
} // namespace use
