#include <mongocxx/instance.hpp>
#include "uspe.hpp"
#include "coco_auth.hpp"
#include "uspe_server.hpp"
#include "logging.hpp"
#include <thread>

int main()
{
    mongocxx::instance inst{}; // This should be done only once.

    uspe::uspe_db db; // the Urban Sensing and Planning Engine (USPE) database

    uspe::uspe uspe(db);                    // the Urban Sensing and Planning Engine (USPE) instance
    uspe.add_module<coco::coco_auth>(uspe); // add the authentication module

    uspe::uspe_server srv(uspe);            // the Urban Sensing and Planning Engine (USPE) server
    srv.add_module<coco::server_auth>(srv); // add the authentication module to the server
    srv.load_certificate("extern/coco/tests/cert.pem", "extern/coco/tests/key.pem");

    // Start the server in a separate thread
    auto srv_ft = std::async(std::launch::async, [&srv]
                             { srv.start(); });

#ifndef NDEBUG
    std::this_thread::sleep_for(std::chrono::seconds(300));

    db.drop();  // drop the database
    srv.stop(); // stop the server
#endif
    return 0;
}
