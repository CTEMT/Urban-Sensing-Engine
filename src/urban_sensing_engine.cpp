#include "urban_sensing_engine.h"

namespace use
{
    urban_sensing_engine::urban_sensing_engine(coco::coco &cc) : coco_listener(cc) {}
    urban_sensing_engine::~urban_sensing_engine() {}

    void urban_sensing_engine::message_arrived([[maybe_unused]] const std::string &topic, [[maybe_unused]] json::json &msg) {}

    void urban_sensing_engine::tick([[maybe_unused]] const semitone::rational &time) {}

    void urban_sensing_engine::start([[maybe_unused]] const std::unordered_set<ratio::core::atom *> &atoms) {}
    void urban_sensing_engine::end([[maybe_unused]] const std::unordered_set<ratio::core::atom *> &atoms) {}

} // namespace use
