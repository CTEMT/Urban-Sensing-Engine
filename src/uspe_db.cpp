#include "uspe_db.hpp"
#include "geometry_parameter.hpp"
#include "logging.hpp"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>

namespace uspe
{
    uspe_db::uspe_db() : users_collection{db["users"]}
    {
        new_parameter_converter<geometry_parameter_converter>();

        if (users_collection.list_indexes().begin() == users_collection.list_indexes().end())
        {
            LOG_DEBUG("Creating indexes for the users collection.");
            users_collection.create_index(bsoncxx::builder::stream::document{} << "email" << 1 << bsoncxx::builder::stream::finalize);
        }

        auto types = get_types();
        if (types.empty())
        {
            std::unordered_map<std::string, std::unique_ptr<coco::coco_parameter>> user_static_pars;
            user_static_pars.emplace("first_name", std::make_unique<coco::string_parameter>("first_name"));
            user_static_pars.emplace("last_name", std::make_unique<coco::string_parameter>("last_name"));
            user_static_pars.emplace("role", std::make_unique<coco::symbolic_parameter>("role", std::vector<std::string>{"Admin", "Decision Maker", "Technician", "Citizen"}));
            user_type_id = create_type("User", "A user of the House of the Emerging Technologies.", std::move(user_static_pars), {}).get_id();
        }
        else
            for (auto &type : types)
                if (type.get().get_name() == "User")
                {
                    user_type_id = type.get().get_id();
                    break;
                }
    }

    std::string uspe_db::login(const std::string &email, const std::string &password)
    {
        using bsoncxx::builder::basic::kvp;
        bsoncxx::builder::basic::document doc{};
        doc.append(kvp("email", email));
        doc.append(kvp("password", password));

        if (auto result = users_collection.find_one(doc.view()))
            return result->view()["item_id"].get_oid().value.to_string();
        else
            throw std::runtime_error("Failed to login.");
    }

    std::string uspe_db::create_user(const std::string &first_name, const std::string &last_name, const std::string &email, const std::string &password, const std::string &role)
    {
        auto user_item = create_item(get_type(user_type_id), email, {{"first_name", first_name}, {"last_name", last_name}, {"role", role}});

        using bsoncxx::builder::basic::kvp;
        bsoncxx::builder::basic::document doc{};
        doc.append(kvp("email", email));
        doc.append(kvp("password", password));
        doc.append(kvp("item_id", bsoncxx::oid{user_item.get_id()}));

        if (auto result = users_collection.insert_one(doc.view()))
            return user_item.get_id();
        else
            throw std::runtime_error("Failed to create user.");
    }
} // namespace uspe