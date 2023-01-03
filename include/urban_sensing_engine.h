#pragma once

#include "mongo_db.h"
#include "coco.h"

namespace use
{
  class urban_sensing_engine
  {
  public:
    urban_sensing_engine();
    ~urban_sensing_engine();

  private:
    coco::mongo_db db;
    coco::coco coco;
  };
} // namespace use
