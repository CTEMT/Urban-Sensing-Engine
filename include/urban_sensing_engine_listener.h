#pragma once

#include "urban_sensing_engine.h"
#include "rational.h"
#include "item.h"
#include <unordered_set>

namespace use
{
  class urban_sensing_engine_listener
  {
    friend class urban_sensing_engine;

  public:
    urban_sensing_engine_listener(urban_sensing_engine &use) : use(use) { use.listeners.push_back(this); }
    urban_sensing_engine_listener(const urban_sensing_engine_listener &orig) = delete;
    virtual ~urban_sensing_engine_listener() { use.listeners.erase(std::find(use.listeners.cbegin(), use.listeners.cend(), this)); }

  private:
    virtual void new_solver([[maybe_unused]] const coco::coco_executor &exec) {}

    virtual void started_solving([[maybe_unused]] const coco::coco_executor &exec) {}
    virtual void solution_found([[maybe_unused]] const coco::coco_executor &exec) {}
    virtual void inconsistent_problem([[maybe_unused]] const coco::coco_executor &exec) {}

    virtual void message_arrived([[maybe_unused]] const std::string &topic, [[maybe_unused]] json::json &msg) {}

    virtual void tick([[maybe_unused]] const coco::coco_executor &exec, [[maybe_unused]] const semitone::rational &time) {}

    virtual void start([[maybe_unused]] const coco::coco_executor &exec, [[maybe_unused]] const std::unordered_set<ratio::core::atom *> &atoms) {}
    virtual void end([[maybe_unused]] const coco::coco_executor &exec, [[maybe_unused]] const std::unordered_set<ratio::core::atom *> &atoms) {}

  protected:
    urban_sensing_engine &use;
  };
} // namespace use
