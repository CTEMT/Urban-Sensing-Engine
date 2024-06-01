#pragma once

#include "mongo_db.hpp"

namespace uspe
{
  class uspe_db final : public coco::mongo_db
  {
  public:
    uspe_db();
  };
} // namespace uspe
