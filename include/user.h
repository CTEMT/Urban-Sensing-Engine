#pragma once

#include "json.h"
#include "clips.h"
#include <memory>

namespace use
{
  class urban_sensing_engine_db;

  enum user_role
  {
    USER_ROLE_ADMIN = 0,
    USER_ROLE_DECISION_MAKER = 1,
    USER_ROLE_CITIZEN = 2
  };

  /**
   * @brief The user class.
   *
   */
  class user
  {
    friend class urban_sensing_engine_db;

  public:
    user(const std::string &id, const std::string &first_name, const std::string &last_name, const std::string &email, const std::string &password, const user_role &role) : id(id), role(role), first_name(first_name), last_name(last_name), email(email), password(password) {}

    /**
     * @brief Get the id of user.
     *
     * @return const std::string& the id of the user.
     */
    const std::string &get_id() const { return id; }
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
     * @brief Get the fact of the user.
     *
     * @return Fact* the fact of the user.
     */
    Fact *get_fact() const { return fact; }

  private:
    const std::string id;
    user_role role;
    std::string first_name, last_name, email, password;
    Fact *fact = nullptr;
  };

  using user_ptr = std::unique_ptr<user>;

  inline json::json to_json(const user &u)
  {
    json::json j;
    j["id"] = u.get_id();
    j["role"] = u.get_role();
    j["first_name"] = u.get_first_name();
    j["last_name"] = u.get_last_name();
    j["email"] = u.get_email();
    return j;
  }
} // namespace focaal