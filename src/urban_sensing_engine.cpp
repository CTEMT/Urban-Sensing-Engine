#include "urban_sensing_engine.h"
#include "urban_sensing_engine_listener.h"

namespace use
{
    urban_sensing_engine::urban_sensing_engine(coco::coco &cc) : coco_listener(cc) {}
    urban_sensing_engine::~urban_sensing_engine() {}

    void urban_sensing_engine::new_solver(const coco::coco_executor &exec)
    {
        for (const auto &listener : listeners)
            listener->new_solver(exec);
    }

    void urban_sensing_engine::started_solving(const coco::coco_executor &exec)
    {
        for (const auto &listener : listeners)
            listener->started_solving(exec);
    }
    void urban_sensing_engine::solution_found(const coco::coco_executor &exec)
    {
        for (const auto &listener : listeners)
            listener->solution_found(exec);
    }
    void urban_sensing_engine::inconsistent_problem(const coco::coco_executor &exec)
    {
        for (const auto &listener : listeners)
            listener->inconsistent_problem(exec);
    }

    void urban_sensing_engine::message_arrived(const std::string &topic, json::json &msg)
    {
        for (const auto &listener : listeners)
            listener->message_arrived(topic, msg);
    }

    void urban_sensing_engine::tick(const coco::coco_executor &exec, const semitone::rational &time)
    {
        for (const auto &listener : listeners)
            listener->tick(exec, time);
    }

    void urban_sensing_engine::start(const coco::coco_executor &exec, const std::unordered_set<ratio::core::atom *> &atoms)
    {
        for (const auto &listener : listeners)
            listener->start(exec, atoms);
    }
    void urban_sensing_engine::end(const coco::coco_executor &exec, const std::unordered_set<ratio::core::atom *> &atoms)
    {
        for (const auto &listener : listeners)
            listener->end(exec, atoms);
    }
} // namespace use
