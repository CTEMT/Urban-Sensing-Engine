#pragma once

#include "coco_listener.h"
#include "user.h"
#include "urban_sensing_engine.h"

namespace use
{
  class urban_sensing_engine_listener : public coco::coco_listener
  {
    friend class urban_sensing_engine;

  public:
    urban_sensing_engine_listener(urban_sensing_engine &use) : coco_listener(use), use(use) { use.listeners.push_back(this); }
    urban_sensing_engine_listener(const urban_sensing_engine_listener &orig) = delete;
    virtual ~urban_sensing_engine_listener() { use.listeners.erase(std::find(use.listeners.cbegin(), use.listeners.cend(), this)); }

  private:
    virtual void new_user([[maybe_unused]] const use::user &u) {}
    virtual void updated_user([[maybe_unused]] const use::user &u) {}
    virtual void removed_user([[maybe_unused]] const std::string &id) {}

    virtual void new_road_state([[maybe_unused]] const road &r, [[maybe_unused]] const float &state) {}
    virtual void new_building_state([[maybe_unused]] const building &b, [[maybe_unused]] const float &state) {}

    virtual void new_question([[maybe_unused]] const question &q) {}
    virtual void new_answer([[maybe_unused]] const question &q, [[maybe_unused]] const std::string &answer) {}

  protected:
    urban_sensing_engine &use;
  };
} // namespace use
