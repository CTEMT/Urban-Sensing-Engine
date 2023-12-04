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
    question(const std::string &id, const std::string &level, const user &recipient, const std::string &content, const std::vector<std::string> &answers, int answer = -1) : id(id), level(level), recipient(recipient), content(content), answers(answers), answer(answer) {}

    std::string get_id() const { return id; }
    std::string get_level() const { return level; }
    const user &get_recipient() const { return recipient; }
    std::string get_content() const { return content; }
    std::vector<std::string> get_answers() const { return answers; }
    int get_answer() const { return answer; }

    Fact *get_fact() const { return fact; }

  private:
    std::string id;
    std::string level;
    const user &recipient;
    std::string content;
    std::vector<std::string> answers;
    int answer;
    Fact *fact = nullptr;
  };

  using question_ptr = std::unique_ptr<question>;
} // namespace use
