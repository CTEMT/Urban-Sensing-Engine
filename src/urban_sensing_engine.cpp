#include "urban_sensing_engine.h"
#include "urban_sensing_engine_listener.h"
#include "logging.h"

namespace use
{
    void send_bus_message([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        UDFValue engine_ptr;
        if (!UDFFirstArgument(udfc, INTEGER_BIT, &engine_ptr))
            return;
        auto &coco = *reinterpret_cast<coco::coco *>(engine_ptr.integerValue->contents);
        urban_sensing_engine *use = nullptr;
        for (auto &l : coco.get_listeners())
            if (auto *coco_l = dynamic_cast<urban_sensing_engine *>(l))
            {
                use = coco_l;
                break;
            }

        UDFValue bus_id;
        if (!UDFNextArgument(udfc, STRING_BIT, &bus_id))
            return;

        UDFValue time;
        if (!UDFNextArgument(udfc, INTEGER_BIT, &time))
            return;

        UDFValue lat;
        if (!UDFNextArgument(udfc, FLOAT_BIT, &lat))
            return;

        UDFValue lng;
        if (!UDFNextArgument(udfc, FLOAT_BIT, &lng))
            return;

        UDFValue passengers;
        if (!UDFNextArgument(udfc, INTEGER_BIT, &passengers))
            return;

        use->update_bus_data(bus_id.lexemeValue->contents, time.integerValue->contents, lat.floatValue->contents, lng.floatValue->contents, passengers.integerValue->contents);
    }

    urban_sensing_engine::urban_sensing_engine(coco::coco &cc) : coco_listener(cc)
    {
        AddUDF(cc.get_environment(), "send_bus_message", "v", 6, 6, "lslddl", send_bus_message, "send_bus_message", NULL);
    }
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

    void urban_sensing_engine::update_bus_data(const std::string &bus_id, const long &time, const double &lat, const double &lng, const long &passengers)
    {
        for (const auto &listener : listeners)
            listener->update_bus_data(bus_id, time, lat, lng, passengers);
    }
} // namespace use
