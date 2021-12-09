#include "rule.h"
#include <sstream>
#include <algorithm>

namespace use
{
    condition *condition::from_json(const smt::json &c)
    {
        smt::string_val &cond_type = static_cast<smt::string_val &>(*c->get("type"));
        if (cond_type.get() == "and")
        {
            smt::array_val &j_conds = static_cast<smt::array_val &>(*c->get("conditions"));
            std::vector<condition *> conds;
            conds.reserve(j_conds.size());
            for (size_t i = 0; i < j_conds.size(); ++i)
                conds.push_back(from_json(j_conds.get(i)));
            return new and_condition(conds);
        }
        else if (cond_type.get() == "or")
        {
            smt::array_val &j_conds = static_cast<smt::array_val &>(*c->get("conditions"));
            std::vector<condition *> conds;
            conds.reserve(j_conds.size());
            for (size_t i = 0; i < j_conds.size(); ++i)
                conds.push_back(from_json(j_conds.get(i)));
            return new or_condition(conds);
        }
        else if (cond_type.get() == "numeric")
        {
            smt::string_val &j_op = static_cast<smt::string_val &>(*c->get("op"));
            smt::string_val &j_var_name = static_cast<smt::string_val &>(*c->get("var_name"));
            std::stringstream ss(j_var_name.get());
            std::vector<std::string> s_ids;
            while (ss.good())
            {
                std::string s_id;
                std::getline(ss, s_id, '.');
                s_ids.push_back(s_id);
            }
            smt::double_val &j_var_value = static_cast<smt::double_val &>(*c->get("var_value"));
            if (j_op.get() == "GEq")
                return new numeric_condition(op::GEq, s_ids, j_var_value.get());
            else if (j_op.get() == "LEq")
                return new numeric_condition(op::LEq, s_ids, j_var_value.get());
        }
        else
            std::cerr << "unknown condition type " << cond_type.get() << std::endl;
        return nullptr;
    }

    bool and_condition::verify(const std::unordered_map<std::string, smt::json> &state) const noexcept
    {
        return std::all_of(conditions.cbegin(), conditions.cend(), [state](const auto &c)
                           { return c->verify(state); });
    }

    bool or_condition::verify(const std::unordered_map<std::string, smt::json> &state) const noexcept
    {
        return std::any_of(conditions.cbegin(), conditions.cend(), [state](const auto &c)
                           { return c->verify(state); });
    }

    effect *effect::from_json(const smt::json &c)
    {
        smt::string_val &eff_type = static_cast<smt::string_val &>(*c->get("type"));
        if (eff_type.get() == "message")
        {
            smt::string_val &j_message = static_cast<smt::string_val &>(*c->get("type"));
            return new message_effect(j_message.get());
        }
        else
            std::cerr << "unknown effect type " << eff_type.get() << std::endl;
        return nullptr;
    }

    bool numeric_condition::verify(const std::unordered_map<std::string, smt::json> &state) const noexcept
    {
        if (state.find(var_name[0]) == state.end())
            return false;
        else
        {
            auto j_val = state.at(var_name[0]);
            for (int i = 1; i < var_name.size(); ++i)
                if (!j_val->has(var_name[i]))
                    return false;
                else
                    j_val = j_val->get(var_name[i]);
            const auto c_val = static_cast<smt::double_val &>(*j_val).get();

            switch (c_op)
            {
            case GEq:
                return c_val >= var_value;
            case LEq:
                return c_val <= var_value;
            default:
                return false;
            }
        }
    }

    void message_effect::apply() const noexcept
    {
        std::cout << "**************************" << std::endl;
        std::cout << message << std::endl;
        std::cout << "**************************" << std::endl;
    }

    rule::rule(const smt::json &r) : cond(condition::from_json(r->get("condition"))), eff(effect::from_json(r->get("effect"))) {}
    rule::~rule() {}
} // namespace use
