#pragma once

#include "coco_core.h"
#include "user.h"

namespace use
{
  class urban_sensing_engine_listener;
  class urban_sensing_engine_db;

  class urban_sensing_engine : public coco::coco_core
  {
    friend class urban_sensing_engine_listener;

  public:
    urban_sensing_engine(urban_sensing_engine_db &db);

    void answer_question(const long long id, const std::string &answer);

    void init() override;

    const std::unordered_map<std::string, double> &get_road_state() const { return road_state; }
    const std::unordered_map<std::string, double> &get_building_state() const { return building_state; }

    void create_user(const std::string &first_name, const std::string &last_name, const std::string &email, const std::string &password, const user_role &role = user_role::USER_ROLE_CITIZEN, const std::vector<std::string> &skills = {});
    void set_user_email(const std::string &id, const std::string &email);
    void set_user_password(const std::string &id, const std::string &password);
    void set_user_role(const std::string &id, const user_role &role);
    void set_user_skills(const std::string &id, const std::vector<std::string> &skills);
    void delete_user(const std::string &id);

  private:
    friend void update_road_state(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void update_building_state(Environment *env, UDFContext *udfc, UDFValue *out);

    friend void send_message(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void send_question(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void send_map_message(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void send_bus_message(Environment *env, UDFContext *udfc, UDFValue *out);

  private:
    void fire_new_road_state(const std::string &road_id, const float &state);
    void fire_new_building_state(const std::string &building_id, const float &state);

    void fire_new_message(const std::string &level, const std::string &content);
    void fire_new_question(const std::string &level, const long long id, const std::string &content, const std::vector<std::string> &answers);
    void fire_new_map_message(const std::string &level, const double &lat, const double &lng, const std::string &content);
    void fire_new_bus_data(const std::string &bus_id, const long &time, const double &lat, const double &lng, const long &passengers);

  private:
    std::unordered_map<std::string, double> road_state;      // the road state..
    std::unordered_map<std::string, double> building_state;  // the building state..
    std::vector<urban_sensing_engine_listener *> listeners; // the urban sensing engine listeners..
  };
} // namespace use
