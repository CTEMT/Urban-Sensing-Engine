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
        add_route(network::Post, "^/users$", std::bind(&uspe_server::create_user, this, std::placeholders::_1));
        add_route(network::Put, "^/users/.$", std::bind(&uspe_server::update_user, this, std::placeholders::_1));
        add_route(network::Delete, "^/users/.$", std::bind(&uspe_server::delete_user, this, std::placeholders::_1));
    }

    std::unique_ptr<network::response> uspe_server::index(network::request &) { return std::make_unique<network::file_response>(DASHBOARD_FOLDER "/dist/index.html"); }
    std::unique_ptr<network::response> uspe_server::assets(network::request &req)
    {
        std::string target = req.get_target();
        if (target.find('?') != std::string::npos)
            target = target.substr(0, target.find('?'));
        return std::make_unique<network::file_response>(DASHBOARD_FOLDER "/dist" + target);
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
        if (!json_body.contains("first_name") || !json_body.contains("last_name") || !json_body.contains("email") || !json_body.contains("password"))
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", "Bad Request. Missing first_name, last_name, email, or password fields"}}), network::bad_request);
        std::string first_name = json_body["first_name"];
        std::string last_name = json_body["last_name"];
        std::string email = json_body["email"];
        std::string password = json_body["password"];
        if (first_name.empty() || last_name.empty() || email.empty() || password.empty())
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", "Bad Request. Empty first_name, last_name, email, or password fields"}}), network::bad_request);
        try
        {
            return std::make_unique<network::json_response>(json::json({{"item_id", get_db().create_user(first_name, last_name, email, password, "Citizen")}}));
        }
        catch (const std::exception &e)
        {
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", e.what()}}), network::bad_request);
        }
    }

    std::unique_ptr<network::response> uspe_server::update_user(network::request &req)
    {
        auto json_req = dynamic_cast<network::json_request *>(&req);
        if (json_req == nullptr)
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", "Bad Request. JSON expected"}}), network::bad_request);
        auto json_body = json_req->get_body();
        if (!json_body.contains("first_name") && !json_body.contains("last_name") && !json_body.contains("email") && !json_body.contains("password") && !json_body.contains("role"))
            return std::make_unique<network::json_response>(json::json({{"code", 400}, {"message", "Bad Request. Missing first_name, last_name, email, password, or role fields"}}), network::bad_request);
        if (auto token = req.get_headers().find("token"); token != req.get_headers().end())
            try
            {
                auto &caller = get_db().get_item(token->second);
                auto &updating_user = get_db().get_item(req.get_target().substr(7));
                if (role_to_int(caller.get_parameters()["role"]) < role_to_int(updating_user.get_parameters()["role"]))
                    return std::make_unique<network::json_response>(json::json({{"code", 401}, {"message", "Unauthorized. Caller has lower role"}}), network::unauthorized);
                auto updating_user_parameters = updating_user.get_parameters();
                for (auto &[key, value] : json_body.as_object())
                    updating_user_parameters[key] = value;
                get_db().set_item_parameters(updating_user, updating_user_parameters);
                return std::make_unique<network::response>();
            }
            catch (const std::exception &e)
            {
                return std::make_unique<network::json_response>(json::json({{"code", 401}, {"message", e.what()}}), network::unauthorized);
            }
        return std::make_unique<network::json_response>(json::json({{"code", 401}, {"message", "Unauthorized. Missing token"}}), network::unauthorized);
    }

    std::unique_ptr<network::response> uspe_server::delete_user(network::request &req)
    {
        if (auto token = req.get_headers().find("token"); token != req.get_headers().end())
            try
            {
                auto &caller = get_db().get_item(token->second);
                auto &deleting_user = get_db().get_item(req.get_target().substr(7));
                if (role_to_int(caller.get_parameters()["role"]) < role_to_int(deleting_user.get_parameters()["role"]))
                    return std::make_unique<network::json_response>(json::json({{"code", 401}, {"message", "Unauthorized. Caller has lower role"}}), network::unauthorized);
                get_db().delete_item(deleting_user);
                return std::make_unique<network::response>();
            }
            catch (const std::exception &e)
            {
                return std::make_unique<network::json_response>(json::json({{"code", 401}, {"message", e.what()}}), network::unauthorized);
            }
        return std::make_unique<network::json_response>(json::json({{"code", 401}, {"message", "Unauthorized. Missing token"}}), network::unauthorized);
    }
} // namespace uspe
