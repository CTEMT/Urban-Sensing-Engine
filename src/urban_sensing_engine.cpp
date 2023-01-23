#include "urban_sensing_engine.h"
#include "urban_sensing_engine_listener.h"
#include "logging.h"

namespace use
{
    void send_message([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        UDFValue engine_ptr;
        if (!UDFFirstArgument(udfc, INTEGER_BIT, &engine_ptr))
            return;
        auto &e = *reinterpret_cast<urban_sensing_engine *>(engine_ptr.integerValue->contents);

        UDFValue level;
        if (!UDFNextArgument(udfc, SYMBOL_BIT, &level))
            return;

        UDFValue content;
        if (!UDFNextArgument(udfc, STRING_BIT, &content))
            return;

        e.fire_new_message(level.lexemeValue->contents, content.lexemeValue->contents);
    }

    void send_map_message([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        UDFValue engine_ptr;
        if (!UDFFirstArgument(udfc, INTEGER_BIT, &engine_ptr))
            return;
        auto &e = *reinterpret_cast<urban_sensing_engine *>(engine_ptr.integerValue->contents);

        UDFValue level;
        if (!UDFNextArgument(udfc, SYMBOL_BIT, &level))
            return;

        UDFValue lat;
        if (!UDFNextArgument(udfc, FLOAT_BIT, &lat))
            return;

        UDFValue lng;
        if (!UDFNextArgument(udfc, FLOAT_BIT, &lng))
            return;

        UDFValue content;
        if (!UDFNextArgument(udfc, STRING_BIT, &content))
            return;

        e.fire_new_map_message(level.lexemeValue->contents, lat.floatValue->contents, lng.floatValue->contents, content.lexemeValue->contents);
    }

    void send_bus_message([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        UDFValue engine_ptr;
        if (!UDFFirstArgument(udfc, INTEGER_BIT, &engine_ptr))
            return;
        auto &e = *reinterpret_cast<urban_sensing_engine *>(engine_ptr.integerValue->contents);

        UDFValue bus_id;
        if (!UDFNextArgument(udfc, SYMBOL_BIT, &bus_id))
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

        e.fire_new_bus_data(bus_id.lexemeValue->contents, time.integerValue->contents, lat.floatValue->contents, lng.floatValue->contents, passengers.integerValue->contents);
    }

    urban_sensing_engine::urban_sensing_engine(coco::coco_db &db) : coco_core(db)
    {
        AddUDF(env, "send_message", "v", 3, 3, "lss", send_message, "send_message", NULL);
        AddUDF(env, "send_map_message", "v", 5, 5, "lydds", send_map_message, "send_map_message", NULL);
        AddUDF(env, "send_bus_message", "v", 6, 6, "lslddl", send_bus_message, "send_bus_message", NULL);
    }

    void urban_sensing_engine::fire_new_message(const std::string &level, const std::string &content)
    {
        for (auto &l : listeners)
            l->new_message(level, content);
    }

    void urban_sensing_engine::fire_new_map_message(const std::string &level, const double &lat, const double &lng, const std::string &content)
    {
        for (auto &l : listeners)
            l->new_map_message(level, lat, lng, content);
    }

    void urban_sensing_engine::fire_new_bus_data(const std::string &bus_id, const long &time, const double &lat, const double &lng, const long &passengers)
    {
        for (auto &l : listeners)
            l->new_bus_data(bus_id, time, lat, lng, passengers);
    }
} // namespace use
