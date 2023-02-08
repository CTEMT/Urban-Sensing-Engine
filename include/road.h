#pragma once

#include "location.h"
#include <memory>

namespace use
{
  class road
  {
  public:
    road(const std::string &id, const std::string &name, std::unique_ptr<coco::location> l) : id(id), name(name), loc(std::move(l)) {}

  private:
    const std::string id;
    std::string name;
    std::unique_ptr<coco::location> loc;
  };
} // namespace use
