#include "uspe_server.hpp"
#include "logging.hpp"

namespace uspe
{
    uspe_server::uspe_server(coco::coco &cc, std::string_view host, unsigned short port) : coco::coco_server(cc, host, port)
    {
    }
} // namespace uspe
