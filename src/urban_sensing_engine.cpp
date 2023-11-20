#include "urban_sensing_engine.h"
#include "urban_sensing_engine_db.h"
#include "urban_sensing_engine_listener.h"
#include "logging.h"

namespace use
{
    void update_road_state(Environment *, UDFContext *udfc, UDFValue *)
    {
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        UDFValue road_id;
        if (!UDFFirstArgument(udfc, SYMBOL_BIT, &road_id))
            return;

        UDFValue state;
        if (!UDFNextArgument(udfc, FLOAT_BIT, &state))
            return;

        LOG_DEBUG("Road '" + static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_road(road_id.lexemeValue->contents).get_name() + "' state is now " + std::to_string(state.floatValue->contents));
        static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_road(road_id.lexemeValue->contents).state = state.floatValue->contents;
        if (state.floatValue->contents == 0)
            e.road_state.erase(road_id.lexemeValue->contents);
        else
            e.road_state.insert(road_id.lexemeValue->contents);
        e.fire_new_road_state(road_id.lexemeValue->contents, state.floatValue->contents);
    }
    void update_building_state(Environment *, UDFContext *udfc, UDFValue *)
    {
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        UDFValue building_id;
        if (!UDFFirstArgument(udfc, SYMBOL_BIT, &building_id))
            return;

        UDFValue state;
        if (!UDFNextArgument(udfc, FLOAT_BIT, &state))
            return;

        LOG_DEBUG("Building '" + static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_building(building_id.lexemeValue->contents).get_name() + "' state is now " + std::to_string(state.floatValue->contents));
        static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_building(building_id.lexemeValue->contents).state = state.floatValue->contents;
        if (state.floatValue->contents == 0)
            e.building_state.erase(building_id.lexemeValue->contents);
        else
            e.building_state.insert(building_id.lexemeValue->contents);
        e.fire_new_building_state(building_id.lexemeValue->contents, state.floatValue->contents);
    }

    void send_message([[maybe_unused]] Environment *, UDFContext *udfc, [[maybe_unused]] UDFValue *)
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

    void send_question([[maybe_unused]] Environment *, UDFContext *udfc, [[maybe_unused]] UDFValue *)
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

    void send_map_message([[maybe_unused]] Environment *, UDFContext *udfc, [[maybe_unused]] UDFValue *)
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

    void send_bus_message([[maybe_unused]] Environment *, UDFContext *udfc, [[maybe_unused]] UDFValue *)
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

    urban_sensing_engine::urban_sensing_engine(urban_sensing_engine_db &db) : coco_core(db)
    {
        AddUDF(env, "update_road_state", "v", 2, 2, "yd", update_road_state, "update_road_state", this);
        AddUDF(env, "update_building_state", "v", 2, 2, "yd", update_building_state, "update_building_state", this);

        AddUDF(env, "send_message", "v", 2, 2, "ys", send_message, "send_message", this);
        AddUDF(env, "send_question", "v", 3, 3, "ysm", send_question, "send_question", this);
        AddUDF(env, "send_map_message", "v", 4, 4, "ydds", send_map_message, "send_map_message", this);
        AddUDF(env, "send_bus_message", "v", 5, 5, "ylddl", send_bus_message, "send_bus_message", this);
    }

    void urban_sensing_engine::init()
    {
        coco_core::init();

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());

        std::string fact_str;

        // we assert the user facts..
        for (auto &u : db.get_users())
        {
            fact_str = "(user (user_id " + u.get().get_id() + ") (user_role ";
            switch (u.get().get_role())
            {
            case user_role::USER_ROLE_ADMIN:
                fact_str += "admin";
                break;
            case user_role::USER_ROLE_DECISION_MAKER:
                fact_str += "decision_maker";
                break;
            case user_role::USER_ROLE_TECHNICIAN:
                fact_str += "technician";
                break;
            case user_role::USER_ROLE_CITIZEN:
                fact_str += "citizen";
                break;
            default:
                LOG_ERR("Unknown user role: " << u.get().get_role());
                break;
            }
            fact_str += ") (first_name \"" + u.get().get_first_name() + "\") (last_name \"" + u.get().get_last_name() + "\") (email \"" + u.get().get_email() + "\")";
            if (u.get().get_location())
                fact_str += " (coordinates " + std::to_string(u.get().get_location()->y) + " " + std::to_string(u.get().get_location()->x) + ")";
            fact_str += ")";
            LOG_DEBUG("Asserting fact: " << fact_str);
            u.get().fact = AssertString(env, fact_str.c_str());

            for (auto &s : u.get().get_skills())
            {
                fact_str = "(skill (user_id " + u.get().get_id() + ") (name " + s + "))";
                LOG_DEBUG("Asserting fact: " << fact_str);
                AssertString(env, fact_str.c_str());
            }
        }

        // we assert the road facts..
        for (auto &r : db.get_roads())
        {
            fact_str = "(road (road_id " + r.get().get_id() + ") (name \"" + r.get().get_name() + "\") (coordinates " + std::to_string(r.get().get_location()->y) + " " + std::to_string(r.get().get_location()->x) + "))";
            LOG_DEBUG("Asserting fact: " << fact_str);
            r.get().fact = AssertString(env, fact_str.c_str());

            if (r.get().get_state() > 0)
                road_state.insert(r.get().get_id());
        }

        // we assert the building facts..
        for (auto &b : db.get_buildings())
        {
            fact_str = "(building (building_id " + b.get().get_id() + ") (name \"" + b.get().get_name() + "\") (coordinates " + std::to_string(b.get().get_location()->y) + " " + std::to_string(b.get().get_location()->x) + "))";
            LOG_DEBUG("Asserting fact: " << fact_str);
            b.get().fact = AssertString(env, fact_str.c_str());

            if (b.get().get_state() > 0)
                building_state.insert(b.get().get_id());
        }

        // we assert the vehicle type facts..
        for (auto &vt : db.get_vehicle_types())
        {
            fact_str = "(vehicle_type (vehicle_type_id " + vt.get().get_id() + ") (name \"" + vt.get().get_name() + "\") (description \"" + vt.get().get_description() + "\") (manufacturer \"" + vt.get().get_manufacturer() + "\"))";
            LOG_DEBUG("Asserting fact: " << fact_str);
            vt.get().fact = AssertString(env, fact_str.c_str());
        }

        // we assert the vehicle facts..
        for (auto &v : db.get_vehicles())
        {
            fact_str = "(vehicle (vehicle_id " + v.get().get_id() + ") (vehicle_type_id " + v.get().get_type().get_id() + ")";
            if (v.get().get_location())
                fact_str += " (coordinates " + std::to_string(v.get().get_location()->y) + " " + std::to_string(v.get().get_location()->x) + ")";
            fact_str += ")";
            LOG_DEBUG("Asserting fact: " << fact_str);
            v.get().fact = AssertString(env, fact_str.c_str());
        }

        // we run the rules engine to update the policy..
        Run(env, -1);
    }

    void urban_sensing_engine::create_user(const std::string &first_name, const std::string &last_name, const std::string &email, const std::string &password, const user_role &role, const std::vector<std::string> &skills)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        db.create_user(first_name, last_name, email, password, role, skills);

        std::string fact_str = "(user (user_id " + email + ") (role ";
        switch (role)
        {
        case user_role::USER_ROLE_ADMIN:
            fact_str += "admin";
            break;
        case user_role::USER_ROLE_DECISION_MAKER:
            fact_str += "decision_maker";
            break;
        case user_role::USER_ROLE_TECHNICIAN:
            fact_str += "technician";
            break;
        case user_role::USER_ROLE_CITIZEN:
            fact_str += "citizen";
            break;
        default:
            LOG_ERR("Unknown user role: " << role);
            break;
        }
        fact_str += ") (first_name " + first_name + ") (last_name " + last_name + ") (email " + email + ")";
        fact_str += ")";
        LOG_DEBUG("Asserting fact: " << fact_str);

        AssertString(env, fact_str.c_str());

        for (auto &s : skills)
        {
            fact_str = "(skill (user_id " + email + ") (skill " + s + "))";
            LOG_DEBUG("Asserting fact: " << fact_str);
            AssertString(env, fact_str.c_str());
        }

        // we run the rules engine to update the policy..
        Run(env, -1);
    }

    void urban_sensing_engine::set_user_email(const std::string &id, const std::string &email)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        db.set_user_email(id, email);

        FMPutSlotString(CreateFactModifier(env, db.get_user(id).fact), "email", email.c_str());

        // we run the rules engine to update the policy..
        Run(env, -1);
    }

    void urban_sensing_engine::set_user_password(const std::string &id, const std::string &password)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        db.set_user_password(id, password);

        FMPutSlotString(CreateFactModifier(env, db.get_user(id).fact), "password", password.c_str());

        // we run the rules engine to update the policy..
        Run(env, -1);
    }

    void urban_sensing_engine::set_user_role(const std::string &id, const user_role &role)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        db.set_user_role(id, role);

        std::string fact_str = "(user (user_id " + id + ") (role ";
        switch (role)
        {
        case user_role::USER_ROLE_ADMIN:
            fact_str += "admin";
            break;
        case user_role::USER_ROLE_DECISION_MAKER:
            fact_str += "decision_maker";
            break;
        case user_role::USER_ROLE_TECHNICIAN:
            fact_str += "technician";
            break;
        case user_role::USER_ROLE_CITIZEN:
            fact_str += "citizen";
            break;
        default:
            LOG_ERR("Unknown user role: " << role);
            break;
        }
        fact_str += "))";
        LOG_DEBUG("Asserting fact: " << fact_str);

        FMPutSlotSymbol(CreateFactModifier(env, db.get_user(id).fact), "role", fact_str.c_str());

        // we run the rules engine to update the policy..
        Run(env, -1);
    }

    void urban_sensing_engine::set_user_skills(const std::string &id, const std::vector<std::string> &skills)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        db.set_user_skills(id, skills);

        // we retract the old skills..
        Eval(env, ("(do-for-all-facts ((?s skill)) (eq ?s:user_id \"" + id + "\") (retract ?s))").c_str(), NULL);

        // we assert the new skills..
        for (auto &s : skills)
        {
            std::string fact_str = "(skill (user_id " + id + ") (skill " + s + "))";
            LOG_DEBUG("Asserting fact: " << fact_str);
            AssertString(env, fact_str.c_str());
        }

        // we run the rules engine to update the policy..
        Run(env, -1);
    }

    void urban_sensing_engine::delete_user(const std::string &id)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        db.delete_user(id);

        // we retract the user..
        Retract(db.get_user(id).fact);

        // we retract the skills..
        Eval(env, ("(do-for-all-facts ((?s skill)) (eq ?s:user_id \"" + id + "\") (retract ?s))").c_str(), NULL);

        // we run the rules engine to update the policy..
        Run(env, -1);
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

    void urban_sensing_engine::fire_new_road_state(const std::string &road_id, const float &state)
    {
        for (auto &l : listeners)
            l->new_road_state(road_id, state);
    }
    void urban_sensing_engine::fire_new_building_state(const std::string &building_id, const float &state)
    {
        for (auto &l : listeners)
            l->new_building_state(building_id, state);
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
