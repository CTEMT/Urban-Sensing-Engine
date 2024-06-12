#include "geometry_parameter.hpp"

namespace uspe
{
    bool geometry_parameter::validate(const json::json &value) const
    {
        if (!value.contains("type"))
            return false;
        auto type = value["type"];
        if (type != "Point" && type != "LineString" && type != "Polygon" && type != "MultiPoint" && type != "MultiLineString" && type != "MultiPolygon" && type != "GeometryCollection")
            return false;
        if (type == "Point" && !value.contains("coordinates"))
            return false;
        if (type == "LineString" && !value.contains("coordinates"))
            return false;
        if (type == "Polygon" && !value.contains("coordinates"))
            return false;
        if (type == "MultiPoint" && !value.contains("coordinates"))
            return false;
        if (type == "MultiLineString" && !value.contains("coordinates"))
            return false;
        if (type == "MultiPolygon" && !value.contains("coordinates"))
            return false;
        if (type == "GeometryCollection" && !value.contains("geometries"))
            return false;
        return true;
    }

    bsoncxx::builder::basic::document geometry_parameter_converter::to_bson(const coco::coco_parameter &t) const
    {
        auto &p = dynamic_cast<const geometry_parameter &>(t);
        bsoncxx::builder::basic::document doc;
        doc.append(bsoncxx::builder::basic::kvp("name", p.get_name()));
        doc.append(bsoncxx::builder::basic::kvp("type", p.get_type()));
        return doc;
    }

    std::unique_ptr<coco::coco_parameter> geometry_parameter_converter::from_bson(const bsoncxx::v_noabi::document::view &doc) const
    {
        auto name = doc["name"].get_string().value.to_string();
        return std::make_unique<geometry_parameter>(name);
    }
} // namespace uspe
