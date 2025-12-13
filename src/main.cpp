#include "USPE_config.hpp"
#include "uspe.hpp"
#include "mongo_db.hpp"
#include "uspe_server.hpp"
#include "cors.hpp"
#include "coco_mqtt.hpp"
#ifdef BUILD_LLM
#include "coco_llm.hpp"
#endif
#ifdef BUILD_FCM
#include "coco_fcm.hpp"
#include "fcm_server.hpp"
#endif
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
#ifdef BUILD_LLM
    LOG_DEBUG("Adding CoCo LLM module");
    cc.add_module<coco::coco_llm>(cc);
#endif
#ifdef BUILD_FCM
    LOG_DEBUG("Adding CoCo FCM module");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto &fcm = cc.add_module<coco::coco_fcm>(cc);
#endif

    LOG_DEBUG("Loading USPE configuration");
    coco::config(cc);

    LOG_DEBUG("Adding MQTT module");
    auto &mqtt = cc.add_module<coco::coco_mqtt>(cc);
    do
    { // wait for mqtt to connect
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (!mqtt.is_connected());

    LOG_INFO("Starting USPE server...");
    coco::coco_server srv(cc);
    srv.add_middleware<network::cors>(srv);
    LOG_DEBUG("Adding USPE server module");
    srv.add_module<uspe::uspe_server>(srv, uspe);
#ifdef BUILD_FCM
    LOG_DEBUG("Adding FCM server module");
    srv.add_module<coco::fcm_server>(srv, fcm);
#endif

    auto srv_ft = std::async(std::launch::async, [&srv]
                             { srv.start(); });

    return 0;
}
