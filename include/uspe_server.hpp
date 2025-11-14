#pragma once
#include "coco_server.hpp"
#include "uspe.hpp"

namespace uspe
{
  class uspe_server : public coco::server_module
  {
  public:
    uspe_server(coco::coco_server &srv, uspe &spe) noexcept;

  private:
    uspe &spe;
  };
} // namespace uspe
