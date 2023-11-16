#pragma once

#include "mongo_db.h"
#include "user.h"
#include "road.h"
#include "building.h"
#include "vehicle_type.h"
#include "vehicle.h"

#define PARTICIPATORY_TYPE "participatory"

namespace use
{
  class urban_sensing_engine_db : public coco::mongo_db
  {
  public:
    urban_sensing_engine_db(const std::string &root = COCO_NAME, const std::string &mongodb_uri = MONGODB_URI(MONGODB_HOST, MONGODB_PORT));

    void init() override;

    /**
     * @brief Create a user object with the given first name, last name, email, password and type and returns its id.
     *
     * @param admin The admin flag of the user.
     * @param first_name The first name of the user.
     * @param last_name The last name of the user.
     * @param email The email of the user.
     * @param password The password of the user.
     * @param data The data of the user.
     * @return std::string The id of the created user.
     */
    std::string create_user(const std::string &first_name, const std::string &last_name, const std::string &email, const std::string &password, const user_role &role = USER_ROLE_CITIZEN, const std::vector<std::string> &skills = {});

    /**
     * @brief Check if the user object with the given id exists.
     *
     * @param id the id of the user.
     * @return true if the user exists.
     * @return false if the user does not exist.
     */
    bool has_user(const std::string &id) const { return users.find(id) != users.end(); }
    /**
     * @brief Get the user object with the given id.
     *
     * @param id the id of the user.
     * @return user& the user.
     */
    user &get_user(const std::string &id) const { return *users.at(id); }
    /**
     * @brief Get the users object.
     *
     * @return std::vector<std::reference_wrapper<user>> the users.
     */
    std::vector<std::reference_wrapper<user>> get_users() const
    {
      std::vector<std::reference_wrapper<user>> users_vector;
      for (auto &u : users)
        users_vector.push_back(std::ref(*u.second));
      return users_vector;
    }
    /**
     * @brief Get the user object with the given email and password.
     *
     * @param email the email of the user.
     * @param password the password of the user.
     * @return std::string the id of the user, or an empty string if the user does not exist.
     */
    std::string get_user(const std::string &email, const std::string &password);

    void set_user_role(user &u, const user_role &role);
    void set_user_role(const std::string &id, const user_role &role) { set_user_role(*users.at(id), role); }

    /**
     * @brief Set the user's first name.
     *
     * @param u the user to update.
     * @param first_name the new first name of the user.
     */
    void set_user_first_name(user &u, const std::string &first_name);
    void set_user_first_name(const std::string &id, const std::string &first_name) { set_user_first_name(*users.at(id), first_name); }
    /**
     * @brief Set the user's last name.
     *
     * @param u the user to update.
     * @param last_name the new last name of the user.
     */
    void set_user_last_name(user &u, const std::string &last_name);
    void set_user_last_name(const std::string &id, const std::string &last_name) { set_user_last_name(*users.at(id), last_name); }
    /**
     * @brief Set the user's email.
     *
     * @param u the user to update.
     * @param email the new email of the user.
     */
    void set_user_email(user &u, const std::string &email);
    void set_user_email(const std::string &id, const std::string &email) { set_user_email(*users.at(id), email); }
    /**
     * @brief Set the user's password.
     *
     * @param u the user to update.
     * @param password the new password of the user.
     */
    void set_user_password(user &u, const std::string &password);
    void set_user_password(const std::string &id, const std::string &password) { set_user_password(*users.at(id), password); }
    /**
     * @brief Set the user's skills.
     *
     * @param u the user to update.
     * @param skills the new skills of the user.
     */
    void set_user_skills(user &u, const std::vector<std::string> &skills);
    void set_user_skills(const std::string &id, const std::vector<std::string> &skills) { set_user_skills(*users.at(id), skills); }
    void add_user_skill(user &u, const std::string &skill);
    void add_user_skill(const std::string &id, const std::string &skill) { add_user_skill(*users.at(id), skill); }
    void remove_user_skill(user &u, const std::string &skill);
    void remove_user_skill(const std::string &id, const std::string &skill) { remove_user_skill(*users.at(id), skill); }

    /**
     * @brief Delete the user object with the given id.
     *
     * @param u the user to delete.
     */
    void delete_user(user &u);
    void delete_user(const std::string &id) { delete_user(*users.at(id)); }

    std::string create_road(const std::string &name, coco::location_ptr l);
    road &get_road(const std::string &id) { return *roads.at(id); }

    std::vector<std::reference_wrapper<road>> get_roads(const std::string &filter = "", const unsigned int limit = -1) const;

    std::string create_building(const std::string &name, const road &r, const std::string &address, coco::location_ptr l);
    building &get_building(const std::string &id) { return *buildings.at(id); }

    std::vector<std::reference_wrapper<building>> get_buildings(const std::string &filter = "", const unsigned int limit = -1) const;

    std::string create_vehicle_type(const std::string &name, const std::string &description, const std::string &manufacturer);
    vehicle_type &get_vehicle_type(const std::string &id) { return *vehicle_types.at(id); }

    std::vector<std::reference_wrapper<vehicle_type>> get_vehicle_types() const
    {
      std::vector<std::reference_wrapper<vehicle_type>> vehicle_types_vector;
      for (auto &vt : vehicle_types)
        vehicle_types_vector.push_back(std::ref(*vt.second));
      return vehicle_types_vector;
    }

    std::string create_vehicle(const vehicle_type &vt, coco::location_ptr l);
    vehicle &get_vehicle(const std::string &id) { return *vehicles.at(id); }

    std::vector<std::reference_wrapper<vehicle>> get_vehicles() const
    {
      std::vector<std::reference_wrapper<vehicle>> vehicles_vector;
      for (auto &v : vehicles)
        vehicles_vector.push_back(std::ref(*v.second));
      return vehicles_vector;
    }

    void set_vehicle_location(vehicle &v, coco::location_ptr l);

  private:
    mongocxx::v_noabi::collection users_collection;
    mongocxx::v_noabi::collection roads_collection;
    mongocxx::v_noabi::collection buildings_collection;
    mongocxx::v_noabi::collection vehicle_types_collection;
    mongocxx::v_noabi::collection vehicles_collection;
    std::unordered_map<std::string, user_ptr> users; // The users of the current instance.
    std::unordered_map<std::string, road_ptr> roads;
    std::unordered_map<std::string, building_ptr> buildings;
    std::unordered_map<std::string, vehicle_type_ptr> vehicle_types;
    std::unordered_map<std::string, vehicle_ptr> vehicles;
  };
} // namespace use
