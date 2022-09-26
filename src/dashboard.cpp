#include "dashboard.h"
#include "logging.h"

namespace dashboard
{
    dashboard::dashboard(const std::string &root, const std::string &host, const unsigned short port) : root(root), host(host), port(port)
    {
        app.loglevel(crow::LogLevel::Warning);

        CROW_ROUTE(app, "/")
        ([&root]()
         {
            crow::mustache::context ctx;
            ctx["title"] = root + " Dashboard";
            return crow::mustache::load("index.html").render(ctx); });
    }

    void dashboard::start()
    {
        LOG("Starting " + root + " Dashboard Web Server on http://" + host + ':' + std::to_string(port) + "..");
        app.bindaddr(host).port(port).run();
    }
    void dashboard::wait_for_server_start()
    {
        app.wait_for_server_start();
        LOG(root + " Dashboard Web Server started..");
    }
    void dashboard::stop()
    {
        LOG("Stopping " + root + " Dashboard Web Server..");
        app.stop();
    }
} // namespace dashboard
