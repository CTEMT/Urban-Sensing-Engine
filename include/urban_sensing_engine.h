#pragma once

#include "coco_core.h"

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

  private:
    friend void send_message(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void send_question(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void send_map_message(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void send_bus_message(Environment *env, UDFContext *udfc, UDFValue *out);

  private:
    void fire_new_message(const std::string &level, const std::string &content);
    void fire_new_question(const std::string &level, const long long id, const std::string &content, const std::vector<std::string> &answers);
    void fire_new_map_message(const std::string &level, const double &lat, const double &lng, const std::string &content);
    void fire_new_bus_data(const std::string &bus_id, const long &time, const double &lat, const double &lng, const long &passengers);

  private:
    std::vector<urban_sensing_engine_listener *> listeners; // the urban sensing engine listeners..
  };
} // namespace use
