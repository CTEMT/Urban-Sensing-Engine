#include "mongo_db.hpp"

namespace uspe
{
  class uspe_db : public coco::mongo_db
  {
  public:
    uspe_db(json::json &&cnfg = {{"name", COCO_NAME}}, std::string_view mongodb_uri = MONGODB_URI(MONGODB_HOST, MONGODB_PORT)) noexcept;
  };
} // namespace uspe
