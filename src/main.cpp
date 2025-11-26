#include "USPE_config.hpp"
#include "uspe.hpp"
#include "coco.hpp"
#include "coco_item.hpp"
#include "mongo_db.hpp"
#include "uspe_server.hpp"
#include "logging.hpp"
#include <mongocxx/instance.hpp>
#include <thread>

int main()
{
    mongocxx::instance inst{}; // This should be done only once.
    LOG_INFO("Starting Urban Sensing and Planning Engine (USPE)...");
    coco::mongo_db db;
    LOG_DEBUG("Connected to MongoDB database");
    LOG_DEBUG("Initializing CoCo framework");
    coco::coco cc(db);
    LOG_DEBUG("Adding USPE module");
    auto &uspe = cc.add_module<uspe::uspe>(cc);

    LOG_DEBUG("Loading USPE configuration");
    coco::config(cc);

    auto itms = cc.get_items();
    if (itms.empty())
    {
        LOG_WARN("No items found in the database. Creating default items.");
        auto &loc_tp = cc.get_type("Location");
        auto &rock_church_tp = cc.get_type("RockChurch");

        auto &loc0 = cc.create_item({loc_tp}, {{"lat", 40.662238}, {"lng", 16.611671}, {"name", "San Leonardo"}, {"address", "Sasso Caveoso, Matera, Italy"}});
        [[maybe_unused]] auto &rc0 = cc.create_item({rock_church_tp}, {{"name", "Chiesa di San Leonardo"}, {"location", loc0.get_id()}});

        LOG_INFO("Default items created.");
    }

    LOG_INFO("Starting USPE server...");
    coco::coco_server srv(cc);
    LOG_DEBUG("Adding USPE server module");
    srv.add_module<uspe::uspe_server>(srv, uspe);

    auto srv_ft = std::async(std::launch::async, [&srv]
                             { srv.start(); });

    return 0;
}
