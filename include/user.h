#pragma once

#include "json.h"
#include "clips.h"
#include <memory>

namespace use
{
  class urban_sensing_engine;
  class urban_sensing_engine_db;

  enum user_role
  {
    USER_ROLE_ADMIN = 0,
    USER_ROLE_DECISION_MAKER = 1,
    USER_ROLE_TECHNICIAN = 2,
    USER_ROLE_CITIZEN = 3
  };

  /**
   * @brief The user class.
   *
   */
  class user
  {
    friend class urban_sensing_engine;
    friend class urban_sensing_engine_db;

  public:
    user(const std::string &id, const std::string &participatory_id, const std::string &first_name, const std::string &last_name, const std::string &email, const std::string &password, const user_role &role = USER_ROLE_CITIZEN, const std::vector<std::string> &skills = {}, coco::location_ptr l = nullptr) : id(id), participatory_id(participatory_id), role(role), first_name(first_name), last_name(last_name), email(email), password(password), skills(skills), loc(std::move(l)) {}

    /**
     * @brief Get the id of user.
     *
     * @return const std::string& the id of the user.
     */
    const std::string &get_id() const { return id; }
    /**
     * @brief Get the participatory id of the user.
     *
     * @return const std::string& The participatory id of the user.
     */
    const std::string &get_participatory_id() const { return participatory_id; }
    /**
     * @brief Get the role of the user.
     *
     * @return user_role The role of the user.
     */
    const user_role &get_role() const { return role; }
    /**
     * @brief Get the first name of the user.
     *
     * @return const std::string& The first name of the user.
     */
    const std::string &get_first_name() const { return first_name; }
    /**
     * @brief Get the last name of the user.
     *
     * @return const std::string& The last name of the user.
     */
    const std::string &get_last_name() const { return last_name; }
    /**
     * @brief Get the email of the user.
     *
     * @return const std::string& The email of the user.
     */
    const std::string &get_email() const { return email; }
    /**
     * @brief Get the password of the user.
     *
     * @return const std::string& The password of the user.
     */
    const std::string &get_password() const { return password; }
    /**
     * @brief Get the skills of the user.
     *
     * @return const std::vector<std::string>& The skills of the user.
     */
    const std::vector<std::string> &get_skills() const { return skills; }
    void add_skill(const std::string &skill) { skills.push_back(skill); }
    void remove_skill(const std::string &skill)
    {
      auto it = std::find(skills.begin(), skills.end(), skill);
      if (it != skills.end())
        skills.erase(it);
    }
    /**
     * @brief Get the location of the user.
     *
     * @return const coco::location_ptr& The location of the user.
     */
    const coco::location_ptr &get_location() const { return loc; }

    /**
     * @brief Get the fact of the user.
     *
     * @return Fact* the fact of the user.
     */
    Fact *get_fact() const { return fact; }

  private:
    const std::string id, participatory_id;
    user_role role;
    std::string first_name, last_name, email, password;
    std::vector<std::string> skills;
    coco::location_ptr loc;
    Fact *fact = nullptr;
  };

  using user_ptr = std::unique_ptr<user>;

  inline json::json to_json(const user &u)
  {
    json::json j;
    j["id"] = u.get_id();
    j["participatory_id"] = u.get_participatory_id();
    switch (u.get_role())
    {
    case USER_ROLE_ADMIN:
      j["role"] = "admin";
      break;
    case USER_ROLE_DECISION_MAKER:
      j["role"] = "decision_maker";
      break;
    case USER_ROLE_TECHNICIAN:
      j["role"] = "technician";
      break;
    case USER_ROLE_CITIZEN:
      j["role"] = "citizen";
      break;
    default:
      break;
    }
    j["first_name"] = u.get_first_name();
    j["last_name"] = u.get_last_name();
    j["email"] = u.get_email();
    if (!u.get_skills().empty())
    {
      json::json skills(json::json_type::array);
      for (auto &s : u.get_skills())
        skills.push_back(s);
      j["skills"] = std::move(skills);
    }
    if (u.get_location())
      j["location"] = {{"lat", u.get_location()->y}, {"lng", u.get_location()->x}};
    return j;
  }
} // namespace focaal