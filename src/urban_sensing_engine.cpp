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
        e.fire_new_road_state(r, state.floatValue->contents);
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
        e.fire_new_building_state(b, state.floatValue->contents);
    }
    void generate_riddle_buildings(Environment *env, UDFContext *udfc, UDFValue *out)
    {
        auto &e = *reinterpret_cast<urban_sensing_engine *>(udfc->context);

        std::string buildings_rddle;
        for (auto &b : static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_buildings())
            buildings_rddle += "Building b_" + b.get().get_id() + " = new Building(\"" + b.get().get_id() + "\");\n";

        out->lexemeValue = CreateString(env, buildings_rddle.c_str());
    }

    void send_message(Environment *env, UDFContext *udfc, UDFValue *out)
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

        use::urban_sensing_engine_db &db = static_cast<use::urban_sensing_engine_db &>(e.get_database());
        std::string message_id = db.create_message(std::chrono::system_clock::now(), level.lexemeValue->contents, static_cast<use::urban_sensing_engine_db &>(e.get_database()).get_user(recipient.lexemeValue->contents), content.lexemeValue->contents, as);

        e.fire_new_message(db.get_message(message_id));

        out->lexemeValue = CreateSymbol(env, message_id.c_str());
    }

    urban_sensing_engine::urban_sensing_engine(urban_sensing_engine_db &db) : coco_core(db)
    {
        AddUDF(env, "generate_riddle_users", "s", 0, 0, "", generate_riddle_users, "generate_riddle_users", this);

        AddUDF(env, "update_road_state", "v", 2, 2, "yd", update_road_state, "update_road_state", this);
        AddUDF(env, "generate_riddle_roads", "s", 0, 0, "", generate_riddle_roads, "generate_riddle_roads", this);

        AddUDF(env, "update_building_state", "v", 2, 2, "yd", update_building_state, "update_building_state", this);
        AddUDF(env, "generate_riddle_buildings", "s", 0, 0, "", generate_riddle_buildings, "generate_riddle_buildings", this);

        AddUDF(env, "send_message", "y", 4, 4, "yysm", send_message, "send_message", this);
    }

    void urban_sensing_engine::init(const std::vector<std::string> &files)
    {
        load_rules(files);

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

        // we assert the occupancy facts..
        for (auto &o : db.get_occupancies())
        {
            fact_str = "(occupancy (tile_id " + o.get().get_id() + ") (latitude " + std::to_string(o.get().get_location()->y) + ") (longitude " + std::to_string(o.get().get_location()->x) + ") (italians " + std::to_string(o.get().get_italians()) + ") (foreigners " + std::to_string(o.get().get_foreigners()) + ") (extraregionals " + std::to_string(o.get().get_extraregionals()) + ") (intraregionals " + std::to_string(o.get().get_intraregionals()) + ") (total " + std::to_string(o.get().get_total()) + "))";
            // LOG_DEBUG("Asserting fact: " << fact_str);
            o.get().fact = AssertString(env, fact_str.c_str());
        }

        // we run the rules engine to update the policy..
        Run(env, -1);

        // we start the coco core..
        start();
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

    void urban_sensing_engine::answer_message(const std::string &id, const std::string &answer)
    {
        const std::lock_guard<std::recursive_mutex> lock(get_mutex());
        auto &message = dynamic_cast<use::urban_sensing_engine_db &>(get_database()).get_message(id);
        LOG_DEBUG("Answering message \"" << message.get_content() << "\" with \"" << answer << "\"");

        urban_sensing_engine_db &db = dynamic_cast<urban_sensing_engine_db &>(get_database());
        db.set_message_answer(message, answer);

        Eval(env, ("(answer_message " + id + " \"" + answer + "\")").c_str(), NULL);

        // we run the rules engine to update the policy..
        Run(env, -1);

        fire_new_answer(message, answer);
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
            l->deleted_user(id);
    }

    void urban_sensing_engine::fire_new_road_state(const road &r, const float &state)
    {
        for (auto &l : listeners)
            l->new_road_state(r, state);
    }
    void urban_sensing_engine::fire_new_building_state(const building &b, const float &state)
    {
        for (auto &l : listeners)
            l->new_building_state(b, state);
    }

    void urban_sensing_engine::fire_new_message(const message &q)
    {
        for (auto &l : listeners)
            l->new_message(q);
    }

    void urban_sensing_engine::fire_new_answer(const message &q, const std::string &answer)
    {
        for (auto &l : listeners)
            l->new_answer(q, answer);
    }
} // namespace use
