#include "coco_server.hpp"

namespace uspe
{
  class uspe_server : public coco::coco_server
  {
  public:
    uspe_server(coco::coco &cc, std::string_view host = SERVER_HOST, unsigned short port = SERVER_PORT);
  };
} // namespace uspe
