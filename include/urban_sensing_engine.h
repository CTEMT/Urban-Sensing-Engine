#pragma once

#include "mongo_db.h"
#include "coco.h"
#include "mqtt_middleware.h"

namespace use
{
  class urban_sensing_engine
  {
  public:
    urban_sensing_engine(const std::string &root = COCO_ROOT, const std::string &mongodb_uri = MONGODB_URI(MONGODB_HOST, MONGODB_PORT), const std::string &mqtt_uri = MQTT_URI(MQTT_HOST, MQTT_PORT));
    ~urban_sensing_engine();

  private:
    coco::mongo_db db;
    coco::coco coco;
  };
} // namespace use
