#pragma once

#include "coco_db.hpp"
#include "coco_core.hpp"
#include "server.hpp"

namespace uspe
{
  class uspe final : public coco::coco_core, public network::server
  {
  public:
    uspe();
  };

  const json::json geometry_schema{{"geometry",
                                    {{"oneOf", std::vector<json::json>{
                                                   {"$ref", "#/components/schemas/point"},
                                                   {"$ref", "#/components/schemas/line_string"},
                                                   {"$ref", "#/components/schemas/polygon"},
                                                   {"$ref", "#/components/schemas/multi_point"},
                                                   {"$ref", "#/components/schemas/multi_line_string"},
                                                   {"$ref", "#/components/schemas/multi_polygon"},
                                                   {"$ref", "#/components/schemas/geometry_collection"}}}}}};
  const json::json coordinate_schema{"coordinates",
                                     {{"type", "array"},
                                      {"description", "An array of two or three numbers representing the x, y, and optionally z coordinates of the position."},
                                      {"items", {{"type", "number"}}}}};
  const json::json linear_ring_schema{"linear_ring",
                                      {{"type", "array"},
                                       {"description", "A linear ring is a closed LineString with four or more positions. The first and last positions are equivalent (they represent equivalent points). It MUST follow the right-hand rule with respect to the area it bounds, i.e., exterior rings are counterclockwise, and holes are clockwise."},
                                       {"items", {{"$ref", "#/components/schemas/coordinates"}}}}};
  const json::json point_schema{"point",
                                {{"type", "object"},
                                 {"description", "A point is a position in coordinate space."},
                                 {"properties",
                                  {{"type", {{"type", "string"}, {"enum", {"Point"}}}},
                                   {"coordinates", {{"$ref", "#/components/schemas/coordinates"}}}}}}};
  const json::json line_string_schema{"line_string",
                                      {{"type", "object"},
                                       {"properties",
                                        {{"type", {{"type", "string"}, {"enum", {"LineString"}}}},
                                         {"coordinates", {{"type", "array"}, {"items", {{"$ref", "#/components/schemas/coordinates"}}}}}}}}};
  const json::json polygon_schema{"polygon",
                                  {{"type", "object"},
                                   {"properties",
                                    {{"type", {{"type", "string"}, {"enum", {"Polygon"}}}},
                                     {"coordinates", {{"type", "array"}, {"items", {{"$ref", "#/components/schemas/linear_ring"}}}}}}}}};
  const json::json multi_point_schema{"multi_point",
                                      {{"type", "object"},
                                       {"properties",
                                        {{"type", {{"type", "string"}, {"enum", {"MultiPoint"}}}},
                                         {"coordinates", {{"type", "array"}, {"items", {{"$ref", "#/components/schemas/coordinates"}}}}}}}}};
  const json::json multi_line_string_schema{"multi_line_string",
                                            {{"type", "object"},
                                             {"properties",
                                              {{"type", {{"type", "string"}, {"enum", {"MultiLineString"}}}},
                                               {"coordinates", {{"type", "array"}, {"items", {{"type", "array"}, {"items", {{"$ref", "#/components/schemas/coordinates"}}}}}}}}}}};
  const json::json multi_polygon_schema{"multi_polygon",
                                        {{"type", "object"},
                                         {"properties",
                                          {{"type", {{"type", "string"}, {"enum", {"MultiPolygon"}}}},
                                           {"coordinates", {{"type", "array"}, {"items", {{"type", "array"}, {"items", {{"$ref", "#/components/schemas/linear_ring"}}}}}}}}}}};
  const json::json geometry_collection_schema{"geometry_collection",
                                              {{"type", "object"},
                                               {"properties",
                                                {{"type", {{"type", "string"}, {"enum", {"GeometryCollection"}}}},
                                                 {"geometries", {{"type", "array"}, {"items", {{"$ref", "#/components/schemas/geometry"}}}}}}}}};
  const json::json feature_schema{"feature",
                                  {{"type", "object"},
                                   {"properties",
                                    {{"type", {{"type", "string"}, {"enum", {"Feature"}}}},
                                     {"geometry", {{"$ref", "#/components/schemas/geometry"}}},
                                     {"properties", {{"type", "object"}}}}}}};
  const json::json feature_collection_schema{"feature_collection",
                                             {{"type", "object"},
                                              {"properties",
                                               {{"type", {{"type", "string"}, {"enum", {"FeatureCollection"}}}},
                                                {"features", {{"type", "array"}, {"items", {{"$ref", "#/components/schemas/feature"}}}}}}}}};
} // namespace uspe
