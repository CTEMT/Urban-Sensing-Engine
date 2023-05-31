#include "urban_sensing_engine.h"
#include "urban_sensing_engine_listener.h"
#include "logging.h"

namespace use
{
    void send_message([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        UDFValue level;
        if (!UDFFirstArgument(udfc, SYMBOL_BIT, &level))
            return;

        UDFValue content;
        if (!UDFNextArgument(udfc, STRING_BIT, &content))
            return;

        e.fire_new_message(level.lexemeValue->contents, content.lexemeValue->contents);
    }

    void send_question([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        UDFValue level;
        if (!UDFFirstArgument(udfc, SYMBOL_BIT, &level))
            return;

        UDFValue id;
        if (!UDFNextArgument(udfc, INTEGER_BIT, &id))
            return;

        UDFValue content;
        if (!UDFNextArgument(udfc, STRING_BIT, &content))
            return;

        UDFValue answers;
        if (!UDFNextArgument(udfc, MULTIFIELD_BIT, &answers))
            return;

        std::vector<std::string> as;
        for (size_t i = 0; i < answers.multifieldValue->length; i++)
        {
            auto &answer = answers.multifieldValue->contents[i];
            if (answer.header->type != STRING_TYPE)
                return;
            as.push_back(answer.lexemeValue->contents);
        }

        e.fire_new_question(level.lexemeValue->contents, id.integerValue->contents, content.lexemeValue->contents, as);
    }

    void send_map_message([[maybe_unused]] Environment *env, UDFContext *udfc, [[maybe_unused]] UDFValue *out)
    {
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        UDFValue level;
        if (!UDFFirstArgument(udfc, SYMBOL_BIT, &level))
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
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        UDFValue bus_id;
        if (!UDFFirstArgument(udfc, SYMBOL_BIT, &bus_id))
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
        AddUDF(env, "send_message", "v", 2, 2, "ys", send_message, "send_message", this);
        AddUDF(env, "send_question", "v", 3, 3, "ysm", send_question, "send_question", this);
        AddUDF(env, "send_map_message", "v", 4, 4, "ydds", send_map_message, "send_map_message", this);
        AddUDF(env, "send_bus_message", "v", 5, 5, "ylddl", send_bus_message, "send_bus_message", this);
    }

    void urban_sensing_engine::answer_question(const long long id, const std::string &answer)
    {
        LOG_DEBUG("Answering question " << id << " with " << answer);
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        std::string fact_str = "(answer (question_id " + std::to_string(id) + ") (answer " + answer + "))";
        LOG_DEBUG("Asserting fact: " << fact_str);

        Fact *sv_f = AssertString(env, fact_str.c_str());
        // we run the rules engine to update the policy..
        Run(env, -1);

        // we retract the sensor value fact..
        Retract(sv_f);
        // we run the rules engine to update the policy..
        Run(env, -1);
    }

    void urban_sensing_engine::fire_new_message(const std::string &level, const std::string &content)
    {
        for (auto &l : listeners)
            l->new_message(level, content);
    }

    void urban_sensing_engine::fire_new_question(const std::string &level, const long long id, const std::string &content, const std::vector<std::string> &answers)
    {
        for (auto &l : listeners)
            l->new_question(level, id, content, answers);
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
