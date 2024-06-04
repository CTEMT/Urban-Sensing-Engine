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

        add_route(network::Post, "^/login$", std::bind(&uspe_server::login, this, std::placeholders::_1));
        add_route(network::Post, "^/create_user$", std::bind(&uspe_server::create_user, this, std::placeholders::_1));
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

    std::unique_ptr<network::response> uspe_server::login(network::request &req)
    {
        auto json_req = dynamic_cast<network::json_request *>(&req);
        if (json_req == nullptr)
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", "Bad Request. JSON expected"}}), network::bad_request);
        auto json_body = json_req->get_body();
        if (!json_body.contains("email") || !json_body.contains("password"))
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", "Bad Request. Missing email or password fields"}}), network::bad_request);
        std::string email = json_body["email"];
        std::string password = json_body["password"];
        if (email.empty() || password.empty())
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", "Bad Request. Empty email or password fields"}}), network::bad_request);
        try
        {
            return std::make_unique<network::json_response>(json::json({{"token", get_db().login(email, password)}}));
        }
        catch (const std::exception &e)
        {
            return std::make_unique<network::json_response>(json::json({{"code", 401}, {"message", e.what()}}), network::unauthorized);
        }
    }
    std::unique_ptr<network::response> uspe_server::create_user(network::request &req)
    {
        auto json_req = dynamic_cast<network::json_request *>(&req);
        if (json_req == nullptr)
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", "Bad Request. JSON expected"}}), network::bad_request);
        auto json_body = json_req->get_body();
        if (!json_body.contains("first_name") || !json_body.contains("last_name") || !json_body.contains("email") || !json_body.contains("password") || !json_body.contains("role"))
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", "Bad Request. Missing first_name, last_name, email, password, or role fields"}}), network::bad_request);
        std::string first_name = json_body["first_name"];
        std::string last_name = json_body["last_name"];
        std::string email = json_body["email"];
        std::string password = json_body["password"];
        std::string role = json_body["role"];
        if (first_name.empty() || last_name.empty() || email.empty() || password.empty() || role.empty())
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", "Bad Request. Empty first_name, last_name, email, password, or role fields"}}), network::bad_request);
        if (role != "Admin" && role != "Decision Maker" && role != "Technician" && role != "Citizen")
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", "Bad Request. Invalid role"}}), network::bad_request);
        if (role != "Citizen")
            if (auto token = req.get_headers().find("token"); token != req.get_headers().end())
                try
                {
                    auto &caller = get_db().get_item(token->second);
                    std::string caller_role = caller.get_parameters()["role"];
                    if (role == "Admin" && caller_role != "Admin")
                        return std::make_unique<network::json_response>(json::json({{"code", 401}, {"message", "Unauthorized. Caller is not an Admin"}}), network::unauthorized);
                    else if (role == "Decision Maker" && caller_role != "Admin" && caller_role != "Decision Maker")
                        return std::make_unique<network::json_response>(json::json({{"code", 401}, {"message", "Unauthorized. Caller is not an Admin or Decision Maker"}}), network::unauthorized);
                    else if (role == "Technician" && caller_role != "Admin" && caller_role != "Decision Maker" && caller_role != "Technician")
                        return std::make_unique<network::json_response>(json::json({{"code", 401}, {"message", "Unauthorized. Caller is not an Admin, Decision Maker, or Technician"}}), network::unauthorized);
                }
                catch (const std::exception &e)
                {
                    return std::make_unique<network::json_response>(json::json({{"code", 401}, {"message", e.what()}}), network::unauthorized);
                }
        try
        {
            return std::make_unique<network::json_response>(json::json({{"item_id", get_db().create_user(first_name, last_name, email, password, role)}}));
        }
        catch (const std::exception &e)
        {
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", e.what()}}), network::bad_request);
        }
    }
} // namespace uspe
