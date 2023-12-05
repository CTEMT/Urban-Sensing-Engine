#pragma once

#include "clips.h"
#include <string>
#include <memory>

namespace use
{
  class urban_sensing_engine;
  class urban_sensing_engine_db;
  class user;

  class question
  {
    friend class urban_sensing_engine;
    friend class urban_sensing_engine_db;

  public:
    question(const std::string &id, const std::string &level, const user &recipient, const std::string &content, const std::vector<std::string> &answers, const std::string &answer = "") : id(id), level(level), recipient(recipient), content(content), answers(answers), answer(answer) {}

    std::string get_id() const { return id; }
    std::string get_level() const { return level; }
    const user &get_recipient() const { return recipient; }
    std::string get_content() const { return content; }
    std::vector<std::string> get_answers() const { return answers; }
    std::string get_answer() const { return answer; }

    Fact *get_fact() const { return fact; }

  private:
    std::string id;
    std::string level;
    const user &recipient;
    std::string content;
    std::vector<std::string> answers;
    std::string answer;
    Fact *fact = nullptr;
  };
  using question_ptr = std::unique_ptr<question>;

  inline json::json to_json(const question &q)
  {
    json::json j{{"id", q.get_id()}, {"level", q.get_level()}, {"recipient", q.get_recipient().get_id()}, {"content", q.get_content()}};
    json::json j_answers(json::json_type::array);
    for (const auto &a : q.get_answers())
      j_answers.push_back(a);
    j["answers"] = j_answers;
    if (!q.get_answer().empty())
      j["answer"] = q.get_answer();
    return j;
  }
} // namespace use
