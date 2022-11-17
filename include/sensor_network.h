#pragma once

#include "json.h"
#include "clips.h"

namespace use
{
  struct location
  {
    double lat, lng;
  };
  
  class sensor_type
  {
  public:
    sensor_type(const std::string &id, const std::string &name, const std::string &description, Fact *fact) : id(id), name(name), description(description), fact(fact) {}
    ~sensor_type() = default;

    const std::string &get_id() const { return id; }

    const std::string &get_name() const { return name; }
    void set_name(const std::string &n) { name = n; }

    const std::string &get_description() const { return description; }
    void set_description(const std::string &d) { description = d; }

    Fact *get_fact() const { return fact; }
    void set_fact(Fact *f) { fact = f; }

  private:
    const std::string id;
    std::string name;
    std::string description;
    Fact *fact;
  };

  class sensor
  {
  public:
    sensor(const std::string &id, const sensor_type &type, std::unique_ptr<location> l, Fact *fact) : id(id), type(type), loc(std::move(l)), fact(fact) {}
    ~sensor() = default;

    const std::string &get_id() const { return id; }
    const sensor_type &get_type() const { return type; }

    Fact *get_fact() const { return fact; }
    void set_fact(Fact *f) { fact = f; }

    location &get_location() const { return *loc; }
    void set_location(std::unique_ptr<location> l) { loc.swap(l); }

    json::json &get_value() const { return *value; }
    void set_value(std::unique_ptr<json::json> val) { value.swap(val); }

  private:
    const std::string id;
    const sensor_type &type;
    Fact *fact;
    std::unique_ptr<location> loc;
    std::unique_ptr<json::json> value;
  };
} // namespace use
