#include "urban_sensing_engine.h"
#include <iostream>

using namespace use;

#define MQTT_URI "150.146.65.22"

int main(int, char **)
{
    urban_sensing_engine use(MQTT_URI);

    while (std::tolower(std::cin.get()) != 'q')
        ;
}
