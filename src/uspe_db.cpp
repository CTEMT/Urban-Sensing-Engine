#include "uspe_db.hpp"
#include "logging.hpp"

namespace uspe
{
    uspe_db::uspe_db(json::json &&cnfg, std::string_view mongodb_uri) noexcept : mongo_db(std::move(cnfg), mongodb_uri)
    {
    }
} // namespace uspe
