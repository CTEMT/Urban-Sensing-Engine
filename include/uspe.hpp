#pragma once

#include "coco_module.hpp"

namespace uspe
{
  class uspe : public coco::coco_module
  {
  public:
    uspe(coco::coco &cc) noexcept;
  };
} // namespace uspe
