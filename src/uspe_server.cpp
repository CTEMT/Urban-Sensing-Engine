#include "uspe_server.hpp"
#include "logging.hpp"

namespace uspe
{
    uspe_server::uspe_server(std::unique_ptr<uspe_db> &&db) : coco::coco_core(std::move(db))
    {
        LOG_DEBUG("OpenAPI: " << j_open_api.dump());
        LOG_DEBUG("AsyncAPI: " << j_async_api.dump());

        add_route(network::Get, "^/$", std::bind(&uspe_server::index, this, std::placeholders::_1));
        add_route(network::Get, "^(/assets/.+)|/.+\\.ico|/.+\\.png", std::bind(&uspe_server::assets, this, std::placeholders::_1));
        add_route(network::Get, "^/open_api$", std::bind(&uspe_server::open_api, this, std::placeholders::_1));
        add_route(network::Get, "^/async_api$", std::bind(&uspe_server::async_api, this, std::placeholders::_1));
    }

    std::unique_ptr<network::response> uspe_server::index(network::request &) { return std::make_unique<network::file_response>("client/dist/index.html"); }
    std::unique_ptr<network::response> uspe_server::assets(network::request &req)
    {
        std::string target = req.get_target();
        if (target.find('?') != std::string::npos)
            target = target.substr(0, target.find('?'));
        return std::make_unique<network::file_response>("client/dist" + target);
    }
    std::unique_ptr<network::response> uspe_server::open_api(network::request &) { return std::make_unique<network::json_response>(j_open_api); }
    std::unique_ptr<network::response> uspe_server::async_api(network::request &) { return std::make_unique<network::json_response>(j_async_api); }
} // namespace uspe
