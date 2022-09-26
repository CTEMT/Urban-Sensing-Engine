#include "dashboard.h"
#include "logging.h"

namespace dashboard
{
    dashboard::dashboard(const std::string &host, const unsigned short port) : host(host), port(port)
    {
        app.loglevel(crow::LogLevel::Warning);

        CROW_ROUTE(app, "/")
        ([]()
         {
            crow::mustache::context ctx;
            ctx["title"] = "CTE-MT Dashboard";
            return crow::mustache::load("index.html").render(ctx); });
    }

    void dashboard::start()
    {
        LOG("Starting CTE-MT Dashboard Web Server on http://" + host + ':' + std::to_string(port) + "..");
        app.bindaddr(host).port(port).run();
    }
    void dashboard::wait_for_server_start()
    {
        app.wait_for_server_start();
        LOG("CTE-MT Dashboard Web Server started..");
    }
    void dashboard::stop()
    {
        LOG("Stopping CTE-MT Dashboard Web Server..");
        app.stop();
    }
} // namespace dashboard
