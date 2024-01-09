#pragma once

#include "coco_core.h"
#include "user.h"
#include "message.h"
#include "road.h"
#include "building.h"

namespace use
{
  class urban_sensing_engine_listener;
  class urban_sensing_engine_db;

  class urban_sensing_engine : public coco::coco_core
  {
    friend class urban_sensing_engine_listener;

  public:
    urban_sensing_engine(urban_sensing_engine_db &db);

    void answer_message(const std::string &id, const std::string &answer);

    /**
     * @brief Initializes the urban sensing engine.
     * 
     * @param files The files to load.
    */
    void init(const std::vector<std::string> &files);

    const std::unordered_set<std::string> &get_road_state() const { return road_state; }
    const std::unordered_set<std::string> &get_building_state() const { return building_state; }

    std::string create_user(const std::string &first_name, const std::string &last_name, const std::string &email, const std::string &password, const user_role &role = user_role::USER_ROLE_CITIZEN, const std::vector<std::string> &skills = {});
    void set_user_first_name(const std::string &id, const std::string &first_name);
    void set_user_last_name(const std::string &id, const std::string &last_name);
    void set_user_email(const std::string &id, const std::string &email);
    void set_user_password(const std::string &id, const std::string &password);
    void set_user_role(const std::string &id, const user_role &role);
    void set_user_skills(const std::string &id, const std::vector<std::string> &skills);
    void delete_user(const std::string &id);

  private:
    friend void generate_riddle_users(Environment *env, UDFContext *udfc, UDFValue *out);

    friend void update_road_state(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void generate_riddle_roads(Environment *env, UDFContext *udfc, UDFValue *out);

    friend void update_building_state(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void generate_riddle_buildings(Environment *env, UDFContext *udfc, UDFValue *out);

    friend void send_message(Environment *env, UDFContext *udfc, UDFValue *out);

  private:
    void fire_new_user(const use::user &u);
    void fire_updated_user(const use::user &u);
    void fire_removed_user(const std::string &id);

    void fire_new_road_state(const road &r, const float &state);
    void fire_new_building_state(const building &b, const float &state);

    void fire_new_message(const message &q);
    void fire_new_answer(const message &q, const std::string &answer);

  private:
    std::unordered_set<std::string> road_state;             // the road state..
    std::unordered_set<std::string> building_state;         // the building state..
    std::vector<urban_sensing_engine_listener *> listeners; // the urban sensing engine listeners..
  };
} // namespace use
