#pragma once

#include "mongo_db.hpp"

namespace uspe
{
  /**
   * @brief Urban Sensing and Planning Engine Database.
   *
   * This class represents the database for the Urban Sensing and Planning Engine (USPE).
   * It is derived from the coco::mongo_db class.
   */
  class uspe_db final : public coco::mongo_db
  {
  public:
    /**
     * @brief Default constructor for the uspe_db class.
     */
    uspe_db();
  };
} // namespace uspe
