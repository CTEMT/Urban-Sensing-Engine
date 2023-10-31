#pragma once

#include "clips.h"
#include <string>
#include <memory>

namespace use
{
  class urban_sensing_engine;
  class urban_sensing_engine_db;

  class vehicle_type
  {
    friend class urban_sensing_engine;
    friend class urban_sensing_engine_db;

  public:
    vehicle_type(const std::string &id, const std::string &name, const std::string &description, const std::string &manufacturer) : id(id), name(name), description(description), manufacturer(manufacturer) {}

    std::string get_id() const { return id; }
    std::string get_name() const { return name; }
    std::string get_description() const { return description; }
    std::string get_manufacturer() const { return manufacturer; }

    Fact *get_fact() const { return fact; }

  private:
    std::string id;
    std::string name;
    std::string description;
    std::string manufacturer;
    Fact *fact = nullptr;
  };
  using vehicle_type_ptr = std::unique_ptr<vehicle_type>;

  inline json::json to_json(const vehicle_type &vt) { return {{"id", vt.get_id()}, {"name", vt.get_name()}, {"description", vt.get_description()}, {"manufacturer", vt.get_manufacturer()}}; }
} // namespace use
