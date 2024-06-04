#include "uspe_server.hpp"

namespace uspe
{
    uspe_server::uspe_server(std::unique_ptr<uspe_db> &&db) : coco::coco_core(std::move(db)) {}
} // namespace uspe
