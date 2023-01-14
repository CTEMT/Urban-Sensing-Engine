#pragma once

#include "coco_core.h"

namespace use
{
  class urban_sensing_engine_listener;

  class urban_sensing_engine : public coco::coco_core
  {
    friend class urban_sensing_engine_listener;

  public:
    urban_sensing_engine(coco::coco_db &db);

  private:
    friend void send_message(Environment *env, UDFContext *udfc, UDFValue *out);
    friend void send_bus_message(Environment *env, UDFContext *udfc, UDFValue *out);

  private:
    void fire_new_message(const std::string &level, const std::string &content);
    void fire_new_bus_data(const std::string &bus_id, const long &time, const double &lat, const double &lng, const long &passengers);

  private:
    std::vector<urban_sensing_engine_listener *> listeners; // the urban sensing engine listeners..
  };
} // namespace use
