#pragma once

#include "json.h"
#include <unordered_set>

namespace use
{
  class condition
  {
  public:
    condition() {}
    virtual ~condition() {}

    virtual bool verify(const std::unordered_set<std::string, std::string> &state) const noexcept = 0;
  };

  class and_condition : public condition
  {
  public:
    and_condition(const std::vector<condition *> &conditions) : conditions(conditions) {}
    ~and_condition() {}

    bool verify(const std::unordered_set<std::string, std::string> &state) const noexcept override;

  private:
    std::vector<condition *> conditions;
  };

  class or_condition : public condition
  {
  public:
    or_condition(const std::vector<condition *> &conditions) : conditions(conditions) {}
    ~or_condition() {}

    bool verify(const std::unordered_set<std::string, std::string> &state) const noexcept override;

  private:
    std::vector<condition *> conditions;
  };

  class rule
  {
  public:
    rule(const smt::json &r);
    ~rule();

    bool applicable(const std::unordered_set<std::string, std::string> &state) const { return cond->verify(state); }

  private:
    condition *cond;
  };
} // namespace use
