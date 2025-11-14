#include "uspe_db.hpp"
#include "logging.hpp"

namespace uspe
{
    uspe_db::uspe_db(coco::coco_db &db) noexcept : db_module(db) {}
} // namespace uspe
