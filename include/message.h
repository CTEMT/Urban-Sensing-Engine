#pragma once

#include "clips.h"
#include <string>
#include <memory>

namespace use
{
  class urban_sensing_engine;
  class urban_sensing_engine_db;
  class user;

  class message
  {
    friend class urban_sensing_engine;
    friend class urban_sensing_engine_db;

  public:
    message(const std::string &id, const std::chrono::system_clock::time_point &timestamp, const std::string &level, const user &recipient, const std::string &content, const std::vector<std::string> &answers, const std::string &answer = "") : id(id), timestamp(timestamp), level(level), recipient(recipient), content(content), answers(answers), answer(answer) {}

    std::string get_id() const { return id; }
    std::chrono::system_clock::time_point get_timestamp() const { return timestamp; }
    std::string get_level() const { return level; }
    const user &get_recipient() const { return recipient; }
    std::string get_content() const { return content; }
    std::vector<std::string> get_answers() const { return answers; }
    std::string get_answer() const { return answer; }

    Fact *get_fact() const { return fact; }

  private:
    std::string id;
    std::chrono::system_clock::time_point timestamp;
    std::string level;
    const user &recipient;
    std::string content;
    std::vector<std::string> answers;
    std::string answer;
    Fact *fact = nullptr;
  };
  using message_ptr = std::unique_ptr<message>;

  inline json::json to_json(const message &q)
  {
    json::json j{{"id", q.get_id()}, {"timestamp", std::chrono::system_clock::to_time_t(q.get_timestamp())}, {"level", q.get_level()}, {"recipient", q.get_recipient().get_id()}, {"content", q.get_content()}};
    if (!q.get_answers().empty())
    {
      json::json j_answers(json::json_type::array);
      for (const auto &a : q.get_answers())
        j_answers.push_back(a);
      j["answers"] = j_answers;
    }
    if (!q.get_answer().empty())
      j["answer"] = q.get_answer();
    return j;
  }
} // namespace use
