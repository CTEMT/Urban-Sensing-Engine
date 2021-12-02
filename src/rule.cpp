#include "rule.h"
#include <algorithm>

namespace use
{
    bool and_condition::verify(const std::unordered_set<std::string, std::string> &state) const noexcept
    {
        return std::all_of(conditions.cbegin(), conditions.cend(), [state](const auto &c)
                           { return c->verify(state); });
    }

    bool or_condition::verify(const std::unordered_set<std::string, std::string> &state) const noexcept
    {
        return std::any_of(conditions.cbegin(), conditions.cend(), [state](const auto &c)
                           { return c->verify(state); });
    }

    rule::rule(const smt::json &r) {}
    rule::~rule() {}
} // namespace use
