#include "uspe_server.hpp"
#include "logging.hpp"

namespace uspe
{
    uspe_server::uspe_server(coco::coco_server &srv, uspe &spe) noexcept : server_module(srv), spe(spe) {}
} // namespace uspe
