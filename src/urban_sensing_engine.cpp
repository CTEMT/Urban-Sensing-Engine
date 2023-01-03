#include "urban_sensing_engine.h"

namespace use
{
    urban_sensing_engine::urban_sensing_engine(const std::string &root, const std::string &mongodb_uri, const std::string &mqtt_uri) : db(root, mongodb_uri), coco(db)
    {
        auto mqtt = std::make_unique<coco::mqtt_middleware>(coco);
        coco.add_middleware(std::move(mqtt));
    }
    urban_sensing_engine::~urban_sensing_engine() {}
} // namespace use
