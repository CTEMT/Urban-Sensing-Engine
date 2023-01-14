#pragma once

#include "coco_listener.h"
#include "urban_sensing_engine.h"

namespace use
{
  class urban_sensing_engine_listener : public coco::coco_listener
  {
  public:
    urban_sensing_engine_listener(urban_sensing_engine &use) : coco_listener(use), use(use) { use.listeners.push_back(this); }
    urban_sensing_engine_listener(const urban_sensing_engine_listener &orig) = delete;
    virtual ~urban_sensing_engine_listener() { use.listeners.erase(std::find(use.listeners.cbegin(), use.listeners.cend(), this)); }

    virtual void new_message([[maybe_unused]] const std::string &level, [[maybe_unused]] const std::string &content) {}
    virtual void new_bus_data([[maybe_unused]] const std::string &bus_id, [[maybe_unused]] const long &time, [[maybe_unused]] const double &lat, [[maybe_unused]] const double &lng, [[maybe_unused]] const long &passengers) {}

  protected:
    urban_sensing_engine &use;
  };
} // namespace use
