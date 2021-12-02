#include "rule.h"
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
            smt::double_val &j_var_value = static_cast<smt::double_val &>(*c->get("var_value"));
            if (j_op.get() == "GEq")
                return new numeric_condition(op::GEq, j_var_name.get(), j_var_value.get());
            else if (j_op.get() == "LEq")
                return new numeric_condition(op::LEq, j_var_name.get(), j_var_value.get());
        }
        return nullptr;
    }

    bool and_condition::verify(const std::unordered_map<std::string, std::string> &state) const noexcept
    {
        return std::all_of(conditions.cbegin(), conditions.cend(), [state](const auto &c)
                           { return c->verify(state); });
    }

    bool or_condition::verify(const std::unordered_map<std::string, std::string> &state) const noexcept
    {
        return std::any_of(conditions.cbegin(), conditions.cend(), [state](const auto &c)
                           { return c->verify(state); });
    }

    bool numeric_condition::verify(const std::unordered_map<std::string, std::string> &state) const noexcept
    {
        if (state.find(var_name) == state.end())
            return false;
        else
            switch (c_op)
            {
            case GEq:
                return std::atof(state.at(var_name).c_str()) >= var_value;
            case LEq:
                return std::atof(state.at(var_name).c_str()) <= var_value;
            default:
                return false;
            }
    }

    rule::rule(const smt::json &r) : cond(condition::from_json(r->get("condition"))) {}
    rule::~rule() {}
} // namespace use
