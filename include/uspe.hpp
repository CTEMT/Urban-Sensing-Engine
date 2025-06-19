#include "coco.hpp"
#include "uspe_db.hpp"

namespace uspe
{
  class uspe : public coco::coco
  {
  public:
    uspe(uspe_db &db) noexcept;
  };
} // namespace uspe
