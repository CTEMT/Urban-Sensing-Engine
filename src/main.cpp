#include "urban_sensing_engine.h"

int main(int argc, char const *argv[])
{
    std::string mqtt_host = "tcp://localhost";
    std::string mqtt_port = "1883";

    for (int i = 1; i < argc - 1; i++)
        if (strcmp(argv[i], "-mqtt_host") == 0)
            mqtt_host = "tcp://" + std::string(argv[++i]);
        else if (strcmp(argv[i], "-port") == 0)
            mqtt_port = std::string(argv[++i]);

    use::urban_sensing_engine use(mqtt_host + ":" + mqtt_port);

    return 0;
}
