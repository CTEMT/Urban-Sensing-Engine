#include "urban_sensing_engine.h"

namespace use
{
    urban_sensing_engine::urban_sensing_engine(const std::string &server_uri, const std::string &client_id) : mqtt_client(server_uri, client_id) {}
} // namespace use
