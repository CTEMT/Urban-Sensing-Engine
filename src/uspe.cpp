#include "uspe.hpp"
#include "uspe_db.hpp"

namespace uspe
{
    uspe::uspe() : coco::coco_core(std::make_unique<uspe_db>()) {}
} // namespace uspe
