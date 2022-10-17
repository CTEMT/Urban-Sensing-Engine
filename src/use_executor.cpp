#include "use_executor.h"
#include "item.h"
#include "predicate.h"
#include "urban_sensing_engine.h"
#include "use_defs.h"

namespace use
{
    use_executor::use_executor(urban_sensing_engine &use, ratio::executor::executor &exec) : core_listener(exec.get_solver()), solver_listener(exec.get_solver()), executor_listener(exec), use(use), exec(exec) {}

    void use_executor::log([[maybe_unused]] const std::string &msg) {}
    void use_executor::read([[maybe_unused]] const std::string &script) {}
    void use_executor::read([[maybe_unused]] const std::vector<std::string> &files) {}

    void use_executor::state_changed()
    {
#ifdef SOLVING_MONITORING
        json::json j_sc;
        j_sc["type"] = "state_changed";
        j_sc["state"] = to_json(exec.get_solver());
        j_sc["timelines"] = to_timelines(exec.get_solver());
        json::array j_executing;
        for (const auto &atm : executing)
            j_executing.push_back(get_id(*atm));
        j_sc["executing"] = std::move(j_executing);
        j_sc["time"] = to_json(current_time);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)) + "/state", j_sc.dump(), QOS, true));
#endif
    }

    void use_executor::started_solving()
    {
        json::json j_ss;
        j_ss["type"] = "started_solving";

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_ss.dump()));
    }
    void use_executor::solution_found()
    {
        c_flaw = nullptr;
        c_resolver = nullptr;

        json::json j_sf;
        j_sf["type"] = "solution_found";
        j_sf["state"] = to_json(exec.get_solver());
        j_sf["timelines"] = to_timelines(exec.get_solver());
        json::array j_executing;
        for (const auto &atm : executing)
            j_executing.push_back(get_id(*atm));
        j_sf["executing"] = std::move(j_executing);
        j_sf["time"] = to_json(current_time);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)) + "/state", j_sf.dump(), QOS, true));

        json::json j_gr;
        j_gr["type"] = "graph";
        json::array j_flaws;
        for (const auto &f : flaws)
            j_flaws.push_back(to_json(*f));
        j_gr["flaws"] = std::move(j_flaws);
        json::array j_resolvers;
        for (const auto &r : resolvers)
            j_resolvers.push_back(to_json(*r));
        j_gr["resolvers"] = std::move(j_resolvers);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)) + "/graph", j_gr.dump(), QOS, true));
    }
    void use_executor::inconsistent_problem()
    {
        c_flaw = nullptr;
        c_resolver = nullptr;

        json::json j_ip;
        j_ip["type"] = "inconsistent_problem";

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_ip.dump()));
    }

    void use_executor::flaw_created(const ratio::solver::flaw &f)
    {
        flaws.insert(&f);

#ifdef SOLVING_MONITORING
        json::json j_fc;
        j_fc["type"] = "flaw_created";
        j_fc["id"] = get_id(f);
        j_fc["phi"] = to_string(f.get_phi());
        json::array j_causes;
        j_causes.reserve(f.get_causes().size());
        for (const auto &c : f.get_causes())
            j_causes.push_back(get_id(*c));
        j_fc["causes"] = std::move(j_causes);
        j_fc["state"] = slv.get_sat_core()->value(f.get_phi());
        j_fc["cost"] = to_json(f.get_estimated_cost());
        auto [lb, ub] = f.get_solver().get_idl_theory().bounds(f.get_position());
        json::json j_pos;
        if (lb > std::numeric_limits<semitone::I>::min())
            j_pos["lb"] = lb;
        if (ub > std::numeric_limits<semitone::I>::max())
            j_pos["ub"] = ub;
        j_fc["pos"] = std::move(j_pos);
        j_fc["data"] = f.get_data();

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_fc.dump()));
#endif
    }
    void use_executor::flaw_state_changed([[maybe_unused]] const ratio::solver::flaw &f)
    {
#ifdef SOLVING_MONITORING
        json::json j_fsc;
        j_fsc["type"] = "flaw_state_changed";
        j_fsc["id"] = get_id(f);
        j_fsc["state"] = slv.get_sat_core()->value(f.get_phi());

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_fsc.dump()));
#endif
    }
    void use_executor::flaw_cost_changed([[maybe_unused]] const ratio::solver::flaw &f)
    {
#ifdef SOLVING_MONITORING
        json::json j_fcc;
        j_fcc["type"] = "flaw_cost_changed";
        j_fcc["id"] = get_id(f);
        j_fcc["cost"] = to_json(f.get_estimated_cost());

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_fcc.dump()));
#endif
    }
    void use_executor::flaw_position_changed([[maybe_unused]] const ratio::solver::flaw &f)
    {
#ifdef SOLVING_MONITORING
        json::json j_fpc;
        j_fpc["type"] = "flaw_position_changed";
        j_fpc["id"] = get_id(f);
        auto [lb, ub] = f.get_solver().get_idl_theory().bounds(f.get_position());
        json::json j_pos;
        if (lb > std::numeric_limits<semitone::I>::min())
            j_pos["lb"] = lb;
        if (ub > std::numeric_limits<semitone::I>::max())
            j_pos["ub"] = ub;
        j_fpc["pos"] = std::move(j_pos);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_fpc.dump()));
#endif
    }
    void use_executor::current_flaw(const ratio::solver::flaw &f)
    {
        c_flaw = &f;
        c_resolver = nullptr;

#ifdef SOLVING_MONITORING
        json::json j_cf;
        j_cf["type"] = "current_flaw";
        j_cf["id"] = get_id(f);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_cf.dump()));
#endif
    }

    void use_executor::resolver_created(const ratio::solver::resolver &r)
    {
        resolvers.insert(&r);

#ifdef SOLVING_MONITORING
        json::json j_rc;
        j_rc["type"] = "resolver_created";
        j_rc["id"] = get_id(r);
        j_rc["rho"] = to_string(r.get_rho());
        json::array j_preconditions;
        j_preconditions.reserve(r.get_preconditions().size());
        for (const auto &pre : r.get_preconditions())
            j_preconditions.push_back(get_id(*pre));
        j_rc["preconditions"] = std::move(j_preconditions);
        j_rc["effect"] = get_id(r.get_effect());
        j_rc["state"] = slv.get_sat_core()->value(r.get_rho());
        j_rc["intrinsic_cost"] = to_json(r.get_intrinsic_cost());
        j_rc["data"] = r.get_data();

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_rc.dump()));
#endif
    }
    void use_executor::resolver_state_changed([[maybe_unused]] const ratio::solver::resolver &r)
    {
#ifdef SOLVING_MONITORING
        json::json j_rsc;
        j_rsc["type"] = "resolver_state_changed";
        j_rsc["id"] = get_id(r);
        j_rsc["state"] = slv.get_sat_core()->value(r.get_rho());

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_rsc.dump()));
#endif
    }
    void use_executor::current_resolver(const ratio::solver::resolver &r)
    {
        c_resolver = &r;

#ifdef SOLVING_MONITORING
        json::json j_cr;
        j_cr["type"] = "current_resolver";
        j_cr["id"] = get_id(r);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_cr.dump()));
#endif
    }

    void use_executor::causal_link_added([[maybe_unused]] const ratio::solver::flaw &f, [[maybe_unused]] const ratio::solver::resolver &r)
    {
#ifdef SOLVING_MONITORING
        json::json j_cla;
        j_cla["type"] = "causal_link_added";
        j_cla["flaw_id"] = get_id(f);
        j_cla["resolver_id"] = get_id(r);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_cla.dump()));
#endif
    }

    void use_executor::tick(const semitone::rational &time)
    {
        current_time = time;

        json::json j_t;
        j_t["type"] = "tick";
        j_t["time"] = to_json(time);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_t.dump()));
    }
    void use_executor::starting(const std::unordered_set<ratio::core::atom *> &atoms)
    {
        json::json j_st;
        j_st["type"] = "starting";
        json::array starting;
        for (const auto &a : atoms)
            starting.push_back(get_id(*a));
        j_st["starting"] = std::move(starting);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_st.dump()));
    }
    void use_executor::start(const std::unordered_set<ratio::core::atom *> &atoms)
    {
        executing.insert(atoms.cbegin(), atoms.cend());

        json::json j_st;
        j_st["type"] = "start";
        json::array start;
        for (const auto &a : atoms)
            start.push_back(get_id(*a));
        j_st["start"] = std::move(start);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_st.dump()));
    }
    void use_executor::ending(const std::unordered_set<ratio::core::atom *> &atoms)
    {
        json::json j_en;
        j_en["type"] = "ending";
        json::array ending;
        for (const auto &a : atoms)
            ending.push_back(get_id(*a));
        j_en["ending"] = std::move(ending);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_en.dump()));
    }
    void use_executor::end(const std::unordered_set<ratio::core::atom *> &atoms)
    {
        for (const auto &a : atoms)
            executing.erase(a);

        json::json j_en;
        j_en["type"] = "end";
        json::array end;
        for (const auto &a : atoms)
            end.push_back(get_id(*a));
        j_en["end"] = std::move(end);

        use.mqtt_client.publish(mqtt::make_message(use.root + SOLVERS_TOPIC + "/" + std::to_string(reinterpret_cast<uintptr_t>(this)), j_en.dump()));
    }
} // namespace use
