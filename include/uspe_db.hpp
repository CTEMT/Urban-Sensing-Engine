#pragma once
#include "coco_db.hpp"

namespace uspe
{
  class uspe_db : public coco::db_module
  {
  public:
    uspe_db(coco::coco_db &db) noexcept;
  };
} // namespace uspe
