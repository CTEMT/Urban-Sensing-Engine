#include "uspe.hpp"

namespace uspe
{
    uspe::uspe(std::unique_ptr<uspe_db> &&db) : coco::coco_core(std::move(db)) {}
} // namespace uspe
