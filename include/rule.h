#pragma once

#include "json.h"
#include <vector>
#include <unordered_map>

namespace use
{
  class condition
  {
  public:
    condition() {}
    virtual ~condition() {}

    virtual bool verify(const std::unordered_map<std::string, smt::json> &state) const noexcept = 0;

    static condition *from_json(const smt::json &c);
  };

  class and_condition : public condition
  {
  public:
    and_condition(const std::vector<condition *> &conditions) : conditions(conditions) {}
    ~and_condition() {}

    bool verify(const std::unordered_map<std::string, smt::json> &state) const noexcept override;

  private:
    std::vector<condition *> conditions;
  };

  class or_condition : public condition
  {
  public:
    or_condition(const std::vector<condition *> &conditions) : conditions(conditions) {}
    ~or_condition() {}

    bool verify(const std::unordered_map<std::string, smt::json> &state) const noexcept override;

  private:
    std::vector<condition *> conditions;
  };

  enum op
  {
    GEq,
    LEq
  };

  class numeric_condition : public condition
  {
  public:
    numeric_condition(const op &c_op, const std::vector<std::string> &var_name, const double &var_value) : c_op(c_op), var_name(var_name), var_value(var_value) {}
    ~numeric_condition() {}

    bool verify(const std::unordered_map<std::string, smt::json> &state) const noexcept override;

  private:
    op c_op;
    std::vector<std::string> var_name;
    double var_value;
  };

  class rule
  {
  public:
    rule(const smt::json &r);
    ~rule();

    bool applicable(const std::unordered_map<std::string, smt::json> &state) const { return cond->verify(state); }

  private:
    condition *cond;
  };
} // namespace use
