#pragma once

#include "mongo_db.h"
#include "user.h"
#include "message.h"
#include "road.h"
#include "building.h"
#include "vehicle_type.h"
#include "vehicle.h"
#include "occupancy.h"
#include "point_of_interest.h"

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

    std::string create_message(const std::chrono::system_clock::time_point &timestamp, const std::string &level, const user &recipient, const std::string &content, const std::vector<std::string> &answers, coco::location_ptr l = nullptr);
    bool has_message(const std::string &id) const { return messages.find(id) != messages.end(); }
    message &get_message(const std::string &id) { return *messages.at(id); }
    std::vector<std::reference_wrapper<message>> get_messages() const
    {
      std::vector<std::reference_wrapper<message>> messages_vector;
      messages_vector.reserve(messages.size());
      for (auto &q : messages)
        messages_vector.push_back(std::ref(*q.second));
      return messages_vector;
    }
    std::vector<std::reference_wrapper<message>> get_messages_for_user(const user &u) const
    {
      std::vector<std::reference_wrapper<message>> messages_vector;
      for (auto &q : messages)
        if (q.second->get_recipient().get_id() == u.get_id())
          messages_vector.push_back(std::ref(*q.second));
      return messages_vector;
    }
    void set_message_answer(message &q, const std::string &answer);

    std::string create_intersection(const std::string &osm_id, coco::location_ptr l);
    bool has_intersection(const std::string &id) const { return intersections.find(id) != intersections.end(); }
    intersection &get_intersection(const std::string &id) { return *intersections.at(id); }

    std::vector<std::reference_wrapper<intersection>> get_intersections() const
    {
      std::vector<std::reference_wrapper<intersection>> intersections_vector;
      intersections_vector.reserve(intersections.size());
      for (auto &i : intersections)
        intersections_vector.push_back(std::ref(*i.second));
      return intersections_vector;
    }

    std::string create_road(const std::string &osm_id, const std::string &name, const intersection &from, const intersection &to, double length, double state);
    road &get_road(const std::string &id) { return *roads.at(id); }
    void set_road_state(road &r, const double &state);

    std::vector<std::reference_wrapper<road>> get_roads(const std::string &filter = "", const unsigned int limit = -1) const;

    std::string create_building(const std::string &name, const road &r, const std::string &address, const double &state, coco::location_ptr l);
    building &get_building(const std::string &id) { return *buildings.at(id); }
    void set_building_state(building &b, const double &state);

    std::vector<std::reference_wrapper<building>> get_buildings(const std::string &filter = "", const unsigned int limit = -1) const;

    std::string create_vehicle_type(const std::string &name, const std::string &description, const std::string &manufacturer);
    vehicle_type &get_vehicle_type(const std::string &id) { return *vehicle_types.at(id); }

    std::vector<std::reference_wrapper<vehicle_type>> get_vehicle_types() const
    {
      std::vector<std::reference_wrapper<vehicle_type>> vehicle_types_vector;
      vehicle_types_vector.reserve(vehicle_types.size());
      for (auto &vt : vehicle_types)
        vehicle_types_vector.push_back(std::ref(*vt.second));
      return vehicle_types_vector;
    }

    std::string create_vehicle(const vehicle_type &vt, coco::location_ptr l);
    vehicle &get_vehicle(const std::string &id) { return *vehicles.at(id); }

    std::vector<std::reference_wrapper<vehicle>> get_vehicles() const
    {
      std::vector<std::reference_wrapper<vehicle>> vehicles_vector;
      vehicles_vector.reserve(vehicles.size());
      for (auto &v : vehicles)
        vehicles_vector.push_back(std::ref(*v.second));
      return vehicles_vector;
    }

    void set_vehicle_location(vehicle &v, coco::location_ptr l);

    std::vector<std::reference_wrapper<occupancy>> get_occupancies() const
    {
      std::vector<std::reference_wrapper<occupancy>> occupancies_vector;
      occupancies_vector.reserve(occupancies.size());
      for (auto &o : occupancies)
        occupancies_vector.push_back(std::ref(*o.second));
      return occupancies_vector;
    }

    std::string create_occupancy(coco::location_ptr loc, long italians, long foreigners, long extraregional, long intraregional, long total);
    occupancy &get_occupancy(const std::string &id) { return *occupancies.at(id); }
    void update_occupancy(occupancy &o, long italians, long foreigners, long extraregional, long intraregional, long total);

    std::vector<std::reference_wrapper<point_of_interest>> get_points_of_interest() const
    {
      std::vector<std::reference_wrapper<point_of_interest>> pois_vector;
      pois_vector.reserve(pois.size());
      for (auto &p : pois)
        pois_vector.push_back(std::ref(*p.second));
      return pois_vector;
    }

    std::string create_point_of_interest(const std::string &osm_id, const std::string &ui_id, const std::string &name, const std::string &type, long opening, long closing, coco::location_ptr l, json::json polygon);
    bool has_point_of_interest(const std::string &id) const { return pois.find(id) != pois.end(); }
    point_of_interest &get_point_of_interest(const std::string &id) { return *pois.at(id); }

  private:
    mongocxx::v_noabi::collection users_collection;
    mongocxx::v_noabi::collection messages_collection;
    mongocxx::v_noabi::collection intersections_collection;
    mongocxx::v_noabi::collection roads_collection;
    mongocxx::v_noabi::collection buildings_collection;
    mongocxx::v_noabi::collection vehicle_types_collection;
    mongocxx::v_noabi::collection vehicles_collection;
    mongocxx::v_noabi::collection occupancies_collection;
    mongocxx::v_noabi::collection pois_collection;
    std::unordered_map<std::string, user_ptr> users;                 // The users of the current instance.
    std::unordered_map<std::string, message_ptr> messages;           // The messages of the current instance.
    std::unordered_map<std::string, intersection_ptr> intersections; // The intersections of the current instance.
    std::unordered_map<std::string, road_ptr> roads;                 // The roads of the current instance.
    std::unordered_map<std::string, building_ptr> buildings;         // The buildings of the current instance.
    std::unordered_map<std::string, vehicle_type_ptr> vehicle_types; // The vehicle types of the current instance.
    std::unordered_map<std::string, vehicle_ptr> vehicles;           // The vehicles of the current instance.
    std::unordered_map<std::string, occupancy_ptr> occupancies;      // The occupancies of the current instance.
    std::unordered_map<std::string, point_of_interest_ptr> pois;     // The pois of the current instance.
  };
} // namespace use
