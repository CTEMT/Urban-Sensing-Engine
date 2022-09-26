#include "urban_sensing_engine.h"
#include "dashboard.h"

int main(int argc, char const *argv[])
{
    std::string root = "CTE-MT";
    std::string mqtt_host = "tcp://127.0.0.1";
    std::string mqtt_port = "1883";
    std::string dashboard_host = "127.0.0.1";

    for (int i = 1; i < argc - 1; i++)
        if (strcmp(argv[i], "-mqtt_host") == 0)
            mqtt_host = "tcp://" + std::string(argv[++i]);
        else if (strcmp(argv[i], "-mqtt_port") == 0)
            mqtt_port = std::string(argv[++i]);
        else if (strcmp(argv[i], "-dashboard_host") == 0)
            dashboard_host = std::string(argv[++i]);
        else if (strcmp(argv[i], "-root") == 0)
            dashboard_host = std::string(argv[++i]);

    use::urban_sensing_engine use(root, mqtt_host + ":" + mqtt_port);
    use.connect();

    dashboard::dashboard srv(root, dashboard_host);
    auto srv_st = std::async(std::launch::async, [&]
                             { srv.start(); });
    srv.wait_for_server_start();

    while (std::tolower(std::cin.get()) != 'q')
        ;

    use.disconnect();
    srv.stop();

    return 0;
}
