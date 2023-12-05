#include "urban_sensing_engine.h"
#include "urban_sensing_engine_db.h"
#include "urban_sensing_engine_listener.h"
#include "logging.h"

namespace use
{
    void generate_riddle_users(Environment *env, UDFContext *udfc, UDFValue *out)
    {
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        std::set<std::string> skills;
        std::string users_rddle;
        for (auto &u : static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_users())
        {
            switch (u.get().get_role())
            {
            case user_role::USER_ROLE_ADMIN:
                users_rddle += "Admin u_" + u.get().get_id() + " = new Admin(\"" + u.get().get_id() + "\");\n";
                break;
            case user_role::USER_ROLE_DECISION_MAKER:
                users_rddle += "DecisionMaker u_" + u.get().get_id() + " = new DecisionMaker(\"" + u.get().get_id() + "\");\n";
                break;
            case user_role::USER_ROLE_TECHNICIAN:
                users_rddle += "Technician u_" + u.get().get_id() + " = new Technician(\"" + u.get().get_id() + "\");\n";
                break;
            case user_role::USER_ROLE_CITIZEN:
                users_rddle += "Citizen u_" + u.get().get_id() + " = new Citizen(\"" + u.get().get_id() + "\");\n";
                break;
            default:
                break;
            }
            for (auto &s : u.get().get_skills())
                skills.insert(s);
        }

        for (auto &s : skills)
            users_rddle += "Skill s_" + s + " = new Skill(\"" + s + "\");\n";

        for (auto &u : static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_users())
            for (auto &s : u.get().get_skills())
                users_rddle += "fact u_" + u.get().get_id() + "_s_" + s + " = new HasSkill(u: u_" + u.get().get_id() + ", s: s_" + s + ");\n";

        out->lexemeValue = CreateString(env, users_rddle.c_str());
    }

    void update_road_state(Environment *, UDFContext *udfc, UDFValue *)
    {
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        UDFValue road_id;
        if (!UDFFirstArgument(udfc, SYMBOL_BIT, &road_id))
            return;

        UDFValue state;
        if (!UDFNextArgument(udfc, FLOAT_BIT, &state))
            return;

        auto &r = static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_road(road_id.lexemeValue->contents);
        LOG_DEBUG("Road '" + r.get_name() + "' state is now " + std::to_string(state.floatValue->contents));
        static_cast<use::urban_sensing_engine_db &>(e.get_database()).set_road_state(r, state.floatValue->contents);
        if (state.floatValue->contents == 0)
            e.road_state.erase(road_id.lexemeValue->contents);
        else
            e.road_state.insert(road_id.lexemeValue->contents);
        e.fire_new_road_state(road_id.lexemeValue->contents, state.floatValue->contents);
    }
    void generate_riddle_roads(Environment *env, UDFContext *udfc, UDFValue *out)
    {
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        std::string roads_rddle;
        for (auto &r : static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_roads())
            roads_rddle += "Road r_" + r.get().get_id() + " = new Road(\"" + r.get().get_id() + "\");\n";

        out->lexemeValue = CreateString(env, roads_rddle.c_str());
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

        auto &b = static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_building(building_id.lexemeValue->contents);
        LOG_DEBUG("Building '" + b.get_name() + "' state is now " + std::to_string(state.floatValue->contents));
        static_cast<use::urban_sensing_engine_db &>(e.get_database()).set_building_state(b, state.floatValue->contents);
        if (state.floatValue->contents == 0)
            e.building_state.erase(building_id.lexemeValue->contents);
        else
            e.building_state.insert(building_id.lexemeValue->contents);
        e.fire_new_building_state(building_id.lexemeValue->contents, state.floatValue->contents);
    }
    void generate_riddle_buildings(Environment *env, UDFContext *udfc, UDFValue *out)
    {
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        std::string buildings_rddle;
        for (auto &b : static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_buildings())
            buildings_rddle += "Building b_" + b.get().get_id() + " = new Building(\"" + b.get().get_id() + "\");\n";

        out->lexemeValue = CreateString(env, buildings_rddle.c_str());
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

    void send_question(Environment *env, UDFContext *udfc, UDFValue *out)
    {
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        UDFValue level;
        if (!UDFFirstArgument(udfc, SYMBOL_BIT, &level))
            return;

        UDFValue recipient;
        if (!UDFNextArgument(udfc, SYMBOL_BIT, &recipient))
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

        std::string question_id = static_cast<use::urban_sensing_engine_db &>(e.get_database()).create_question(level.lexemeValue->contents, static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_user(recipient.lexemeValue->contents), content.lexemeValue->contents, as);

        e.fire_new_question(question_id, level.lexemeValue->contents, static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_user(recipient.lexemeValue->contents), content.lexemeValue->contents, as);

        out->lexemeValue = CreateSymbol(env, question_id.c_str());
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
        AddUDF(env, "generate_riddle_users", "s", 0, 0, "", generate_riddle_users, "generate_riddle_users", this);

        AddUDF(env, "update_road_state", "v", 2, 2, "yd", update_road_state, "update_road_state", this);
        AddUDF(env, "generate_riddle_roads", "s", 0, 0, "", generate_riddle_roads, "generate_riddle_roads", this);

        AddUDF(env, "update_building_state", "v", 2, 2, "yd", update_building_state, "update_building_state", this);
        AddUDF(env, "generate_riddle_buildings", "s", 0, 0, "", generate_riddle_buildings, "generate_riddle_buildings", this);

        AddUDF(env, "send_message", "v", 2, 2, "ys", send_message, "send_message", this);
        AddUDF(env, "send_question", "y", 4, 4, "yysm", send_question, "send_question", this);
        AddUDF(env, "send_map_message", "v", 4, 4, "ydds", send_map_message, "send_map_message", this);
        AddUDF(env, "send_bus_message", "v", 5, 5, "ylddl", send_bus_message, "send_bus_message", this);
    }

    void urban_sensing_engine::init()
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());
        coco_core::init();

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());

        std::string fact_str;

        // we assert the user facts..
        for (auto &u : db.get_users())
        {
            fact_str = "(user (user_id " + u.get().get_id() + ") (user_role " + to_string(u.get().get_role()) + ") (first_name \"" + u.get().get_first_name() + "\") (last_name \"" + u.get().get_last_name() + "\") (email \"" + u.get().get_email() + "\")";
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

        // we assert the intersection facts..
        for (auto &i : db.get_intersections())
        {
            fact_str = "(intersection (intersection_id " + i.get().get_id() + ") (coordinates " + std::to_string(i.get().get_location()->y) + " " + std::to_string(i.get().get_location()->x) + "))";
            // LOG_DEBUG("Asserting fact: " << fact_str);
            i.get().fact = AssertString(env, fact_str.c_str());
        }

        // we assert the road facts..
        for (auto &r : db.get_roads())
        {
            fact_str = "(road (road_id " + r.get().get_id() + ") (name \"" + r.get().get_name() + "\") (source " + r.get().get_from().get_id() + ") (target " + r.get().get_to().get_id() + ") (road_state " + std::to_string(r.get().get_state()) + "))";
            // LOG_DEBUG("Asserting fact: " << fact_str);
            r.get().fact = AssertString(env, fact_str.c_str());

            if (r.get().get_state() > 0)
                road_state.insert(r.get().get_id());
        }

        // we assert the building facts..
        for (auto &b : db.get_buildings())
        {
            fact_str = "(building (building_id " + b.get().get_id() + ") (name \"" + b.get().get_name() + "\") (coordinates " + std::to_string(b.get().get_location()->y) + " " + std::to_string(b.get().get_location()->x) + ") (building_state " + std::to_string(b.get().get_state()) + "))";
            // LOG_DEBUG("Asserting fact: " << fact_str);
            b.get().fact = AssertString(env, fact_str.c_str());

            if (b.get().get_state() > 0)
                building_state.insert(b.get().get_id());
        }

        // we assert the vehicle type facts..
        for (auto &vt : db.get_vehicle_types())
        {
            fact_str = "(vehicle_type (vehicle_type_id " + vt.get().get_id() + ") (name \"" + vt.get().get_name() + "\") (description \"" + vt.get().get_description() + "\") (manufacturer \"" + vt.get().get_manufacturer() + "\"))";
            // LOG_DEBUG("Asserting fact: " << fact_str);
            vt.get().fact = AssertString(env, fact_str.c_str());
        }

        // we assert the vehicle facts..
        for (auto &v : db.get_vehicles())
        {
            fact_str = "(vehicle (vehicle_id " + v.get().get_id() + ") (vehicle_type_id " + v.get().get_type().get_id() + ")";
            if (v.get().get_location())
                fact_str += " (coordinates " + std::to_string(v.get().get_location()->y) + " " + std::to_string(v.get().get_location()->x) + ")";
            fact_str += ")";
            // LOG_DEBUG("Asserting fact: " << fact_str);
            v.get().fact = AssertString(env, fact_str.c_str());
        }

        // we run the rules engine to update the policy..
        Run(env, -1);
    }

    std::string urban_sensing_engine::create_user(const std::string &first_name, const std::string &last_name, const std::string &email, const std::string &password, const user_role &role, const std::vector<std::string> &skills)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        auto user_id = db.create_user(first_name, last_name, email, password, role, skills);

        std::string fact_str = "(user (user_id " + user_id + ") (user_role " + to_string(role) + ") (first_name \"" + first_name + "\") (last_name \"" + last_name + "\") (email \"" + email + "\"))";
        LOG_DEBUG("Asserting fact: " << fact_str);

        db.get_user(user_id).fact = AssertString(env, fact_str.c_str());

        for (auto &s : skills)
        {
            fact_str = "(skill (user_id " + email + ") (skill " + s + "))";
            LOG_DEBUG("Asserting fact: " << fact_str);
            AssertString(env, fact_str.c_str());
        }

        // we run the rules engine to update the policy..
        Run(env, -1);

        fire_new_user(db.get_user(user_id));
        return user_id;
    }

    void urban_sensing_engine::set_user_first_name(const std::string &id, const std::string &first_name)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        if (!db.has_user(id))
            throw std::runtime_error("User '" + id + "' does not exist.");

        db.set_user_first_name(id, first_name);

        FMPutSlotString(CreateFactModifier(env, db.get_user(id).fact), "first_name", ("\"" + first_name + "\"").c_str());

        // we run the rules engine to update the policy..
        Run(env, -1);

        fire_updated_user(db.get_user(id));
    }

    void urban_sensing_engine::set_user_last_name(const std::string &id, const std::string &last_name)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        if (!db.has_user(id))
            throw std::runtime_error("User '" + id + "' does not exist.");

        db.set_user_last_name(id, last_name);

        FMPutSlotString(CreateFactModifier(env, db.get_user(id).fact), "last_name", ("\"" + last_name + "\"").c_str());

        // we run the rules engine to update the policy..
        Run(env, -1);

        fire_updated_user(db.get_user(id));
    }

    void urban_sensing_engine::set_user_email(const std::string &id, const std::string &email)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        if (!db.has_user(id))
            throw std::runtime_error("User '" + id + "' does not exist.");

        db.set_user_email(id, email);

        FMPutSlotString(CreateFactModifier(env, db.get_user(id).fact), "email", ("\"" + email + "\"").c_str());

        // we run the rules engine to update the policy..
        Run(env, -1);

        fire_updated_user(db.get_user(id));
    }

    void urban_sensing_engine::set_user_password(const std::string &id, const std::string &password)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        if (!db.has_user(id))
            throw std::runtime_error("User '" + id + "' does not exist.");

        db.set_user_password(id, password);

        // we run the rules engine to update the policy..
        Run(env, -1);

        fire_updated_user(db.get_user(id));
    }

    void urban_sensing_engine::set_user_role(const std::string &id, const user_role &role)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        if (!db.has_user(id))
            throw std::runtime_error("User '" + id + "' does not exist.");

        db.set_user_role(id, role);

        std::string fact_str = "(user (user_id " + id + ") (user_role " + to_string(role) + "))";
        LOG_DEBUG("Asserting fact: " << fact_str);

        FMPutSlotSymbol(CreateFactModifier(env, db.get_user(id).fact), "role", fact_str.c_str());

        // we run the rules engine to update the policy..
        Run(env, -1);

        fire_updated_user(db.get_user(id));
    }

    void urban_sensing_engine::set_user_skills(const std::string &id, const std::vector<std::string> &skills)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        if (!db.has_user(id))
            throw std::runtime_error("User '" + id + "' does not exist.");

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

        fire_updated_user(db.get_user(id));
    }

    void urban_sensing_engine::delete_user(const std::string &id)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        if (!db.has_user(id))
            throw std::runtime_error("User '" + id + "' does not exist.");

        auto &u = db.get_user(id);
        delete_sensor(db.get_sensor(u.get_participatory_id()));
        auto f = u.fact;
        db.delete_user(id);

        // we retract the user..
        Retract(f);

        // we retract the skills..
        Eval(env, ("(do-for-all-facts ((?s skill)) (eq ?s:user_id \"" + id + "\") (retract ?s))").c_str(), NULL);

        // we run the rules engine to update the policy..
        Run(env, -1);

        fire_removed_user(id);
    }

    void urban_sensing_engine::answer_question(const std::string &id, const std::string &answer)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());
        auto &question = dynamic_cast<use::urban_sensing_engine_db &>(get_database()).get_question(id);
        LOG_DEBUG("Answering question \"" << question.get_content() << "\" with \"" << answer << "\"");

        Eval(env, ("(answer_question " + id + " \"" + answer + "\")").c_str(), NULL);

        // we run the rules engine to update the policy..
        Run(env, -1);
    }

    void urban_sensing_engine::fire_new_user(const use::user &u)
    {
        for (auto &l : listeners)
            l->new_user(u);
    }
    void urban_sensing_engine::fire_updated_user(const use::user &u)
    {
        for (auto &l : listeners)
            l->updated_user(u);
    }
    void urban_sensing_engine::fire_removed_user(const std::string &id)
    {
        for (auto &l : listeners)
            l->removed_user(id);
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

    void urban_sensing_engine::fire_new_question(const std::string id, const std::string &level, const user &recipient, const std::string &content, const std::vector<std::string> &answers)
    {
        for (auto &l : listeners)
            l->new_question(id, level, recipient, content, answers);
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
