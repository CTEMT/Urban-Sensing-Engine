#pragma once

#include "coco_listener.h"

namespace use
{
  class urban_sensing_engine : public coco::coco_listener
  {
  public:
    urban_sensing_engine(coco::coco &cc);
    ~urban_sensing_engine();

  private:
    void message_arrived(const std::string &topic, json::json &msg) override;

    void tick(const semitone::rational &time) override;

    void start(const std::unordered_set<ratio::core::atom *> &atoms) override;
    void end(const std::unordered_set<ratio::core::atom *> &atoms) override;
  };
} // namespace use
