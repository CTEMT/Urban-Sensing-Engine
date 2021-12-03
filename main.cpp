#include "urban_sensing_engine.h"

using namespace use;

#define REST_URI "https://matera-rest-api.na.icar.cnr.it/"
#define MQTT_URI "150.146.65.22"

int main(int, char **)
{
    urban_sensing_engine use(REST_URI, MQTT_URI);
    use.start();
}
