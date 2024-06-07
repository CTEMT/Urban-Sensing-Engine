#pragma once

#include "coco_core.hpp"
#include "graph.hpp"
#include "agent.hpp"
#include "state_variable.hpp"
#include "reusable_resource.hpp"
#include "consumable_resource.hpp"
#include "logging.hpp"

namespace uspe
{
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
  const json::json schemas{"schemas",
                           {
                               coco::parameter_schema,
                               coco::integer_parameter_schema,
                               coco::real_parameter_schema,
                               coco::boolean_parameter_schema,
                               coco::symbol_parameter_schema,
                               coco::string_parameter_schema,
                               coco::array_parameter_schema,
                               coco::coco_type_schema,
                               coco::coco_item_schema,
                               coco::data_schema,
                               coco::coco_rule_schema,
                               ratio::rational_schema,
                               ratio::inf_rational_schema,
                               ratio::value_schema,
                               ratio::bool_value_schema,
                               ratio::int_value_schema,
                               ratio::real_value_schema,
                               ratio::time_value_schema,
                               ratio::string_value_schema,
                               ratio::enum_value_schema,
                               ratio::object_value_schema,
                               ratio::item_schema,
                               ratio::atom_schema,
                               ratio::solver_state_schema,
                               ratio::timeline_schema,
                               ratio::solver_timeline_schema,
                               ratio::flaw_schema,
                               ratio::resolver_schema,
                               ratio::solver_graph_schema,
                               ratio::agent_timeline_schema,
                               ratio::state_variable_timeline_value_schema,
                               ratio::state_variable_timeline_schema,
                               ratio::reusable_resource_timeline_value_schema,
                               ratio::reusable_resource_timeline_schema,
                               ratio::consumable_resource_timeline_value_schema,
                               ratio::consumable_resource_timeline_schema,
                               coco::error_schema,
                           }};
  const json::json messages{{"messages",
                             {ratio::executor::new_executor_message,
                              ratio::executor::executor_state_message,
                              ratio::executor::tick_message,
                              ratio::executor::state_message,
                              ratio::graph_message,
                              ratio::flaw_created_message,
                              ratio::flaw_state_changed_message,
                              ratio::flaw_cost_changed_message,
                              ratio::flaw_position_changed_message,
                              ratio::current_flaw_message,
                              ratio::resolver_created_message,
                              ratio::resolver_state_changed_message,
                              ratio::current_resolver_message,
                              ratio::causal_link_added_message}}};

  const json::json users_path{"/users",
                              {{"post",
                                {{"summary", "Create User"},
                                 {"description", "Create User endpoint"},
                                 {"requestBody",
                                  {{"content",
                                    {{"application/json",
                                      {{"schema",
                                        {{"type", "object"},
                                         {"properties",
                                          {{"first_name", {{"type", "string"}}},
                                           {"last_name", {{"type", "string"}}},
                                           {"role", {{"type", "string"}, {"enum", {"Admin", "Decision Maker", "Technician", "Citizen"}}}},
                                           {"email", {{"type", "string"}}},
                                           {"password", {{"type", "string"}}}}},
                                         {"required", {"first_name", "last_name", "role", "email", "password"}}}}}}}}}},
                                 {"responses",
                                  {{"200",
                                    {{"description", "Successful login"},
                                     {"content", {{"application/json", {{"schema", {{"type", "object"}, {"properties", {{"token", {{"type", "string"}, {"format", "uuid"}}}}}}}}}}}}},
                                   {"400", {{"description", "Bad Request"}}}}}}}}};
  const json::json users_id_path{"/users/{user_id}",
                                 {{"put",
                                   {{"summary", "Update User"},
                                    {"description", "Update User endpoint"},
                                    {"requestBody",
                                     {{"content",
                                       {{"application/json",
                                         {{"schema",
                                           {{"type", "object"},
                                            {"properties",
                                             {{"first_name", {{"type", "string"}}},
                                              {"last_name", {{"type", "string"}}},
                                              {"role", {{"type", "string"}, {"enum", {"Admin", "Decision Maker", "Technician", "Citizen"}}}},
                                              {"email", {{"type", "string"}}},
                                              {"password", {{"type", "string"}}}}},
                                            {"required", {"first_name", "last_name", "role", "email", "password"}}}}}}}}}},
                                    {"parameters",
                                     {{{"name", "token"},
                                       {"in", "header"},
                                       {"required", true},
                                       {"schema", {{"type", "string"}, {"format", "uuid"}}}},
                                      {{"name", "user_id"},
                                       {"in", "path"},
                                       {"required", true},
                                       {"schema", {{"type", "string"}, {"format", "uuid"}}}}}},
                                    {"responses",
                                     {{"200", {{"description", "Successful update"}}},
                                      {"400", {{"description", "Bad Request"}}},
                                      {"401", {{"description", "Unauthorized"}}}}}}}},
                                 {{"delete",
                                   {{"summary", "Delete User"},
                                    {"description", "Delete User endpoint"},
                                    {"parameters",
                                     {{{"name", "token"},
                                       {"in", "header"},
                                       {"required", true},
                                       {"schema", {{"type", "string"}, {"format", "uuid"}}}},
                                      {{"name", "user_id"},
                                       {"in", "path"},
                                       {"required", true},
                                       {"schema", {{"type", "string"}, {"format", "uuid"}}}}}},
                                    {"responses",
                                     {{"200", {{"description", "Successful delete"}}},
                                      {"400", {{"description", "Bad Request"}}},
                                      {"401", {{"description", "Unauthorized"}}}}}}}}};

  inline json::json make_open_api() noexcept
  {
    auto types_path = coco::types_path["/types"];
    types_path["get"]["parameters"] = std::vector<json::json>{{{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}}};
    types_path["post"]["parameters"] = std::vector<json::json>{{{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}}};

    auto types_id_path = coco::types_id_path["/types/{type_id}"];
    static_cast<std::vector<json::json>>(types_id_path["delete"]["parameters"]).push_back({{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}});

    auto items_path = coco::items_path["/items"];
    items_path["get"]["parameters"] = std::vector<json::json>{{{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}}};
    items_path["post"]["parameters"] = std::vector<json::json>{{{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}}};

    auto items_id_path = coco::items_id_path["/items/{item_id}"];
    static_cast<std::vector<json::json>>(items_id_path["delete"]["parameters"]).push_back({{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}});

    auto data_path = coco::data_path["/data/{item_id}"];
    static_cast<std::vector<json::json>>(data_path["get"]["parameters"]).push_back({{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}});
    static_cast<std::vector<json::json>>(data_path["post"]["parameters"]).push_back({{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}});

    auto reactive_rules_path = coco::reactive_rules_path["/reactive_rules"];
    reactive_rules_path["get"]["parameters"] = std::vector<json::json>{{{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}}};
    reactive_rules_path["post"]["parameters"] = std::vector<json::json>{{{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}}};

    auto reactive_rules_id_path = coco::reactive_rules_id_path["/reactive_rules/{rule_id}"];
    static_cast<std::vector<json::json>>(reactive_rules_id_path["delete"]["parameters"]).push_back({{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}});

    auto deliberative_rules_path = coco::deliberative_rules_path["/deliberative_rules"];
    deliberative_rules_path["get"]["parameters"] = std::vector<json::json>{{{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}}};
    deliberative_rules_path["post"]["parameters"] = std::vector<json::json>{{{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}}};

    auto deliberative_rules_id_path = coco::deliberative_rules_id_path["/deliberative_rules/{rule_id}"];
    static_cast<std::vector<json::json>>(deliberative_rules_id_path["delete"]["parameters"]).push_back({{"name", "token"}, {"in", "header"}, {"required", true}, {"schema", {{"type", "string"}, {"format", "uuid"}}}});

    json::json open_api{
        {"openapi", "3.0.0"},
        {"info",
         {{"title", "Urban Sensing and Planning Engine API"},
          {"description", "The Urban Sensing and Planning Engine (USPE) API"},
          {"version", "1.0"}}},
        {"servers", std::vector<json::json>{{"url", "http://" SERVER_HOST ":" + std::to_string(SERVER_PORT)}}},
        {"paths",
         {{"/",
           {{"get",
             {{"summary", "Index"},
              {"description", "Index page"},
              {"responses",
               {{"200", {{"description", "Index page"}}}}}}}}},
          {"/assets/{file}",
           {{"get",
             {{"summary", "Assets"},
              {"description", "Assets"},
              {"parameters", std::vector<json::json>{{{"name", "file"}, {"in", "path"}, {"required", true}, {"schema", {{"type", "string"}}}}}},
              {"responses",
               {{"200", {{"description", "Index page"}}}}}}}}},
          {"/open_api",
           {{"get",
             {{"summary", "Retrieve OpenAPI Specification"},
              {"description", "Endpoint to fetch the OpenAPI Specification document"},
              {"responses",
               {{"200", {{"description", "Successful response with OpenAPI Specification document"}}}}}}}}},
          {"/async_api",
           {{"get",
             {{"summary", "Retrieve AsyncAPI Specification"},
              {"description", "Endpoint to fetch the AsyncAPI Specification document"},
              {"responses",
               {{"200", {{"description", "Successful response with AsyncAPI Specification document"}}}}}}}}},
          {"/login",
           {{"post",
             {{"summary", "Login"},
              {"description", "Login endpoint"},
              {"requestBody",
               {{"content",
                 {{"application/json",
                   {{"schema",
                     {{"type", "object"},
                      {"properties",
                       {{"email", {{"type", "string"}}},
                        {"password", {{"type", "string"}}}}},
                      {"required", std::vector<json::json>{"email", "password"}}}}}}}}}},
              {"responses",
               {{"200",
                 {{"description", "Successful login"},
                  {"content", {{"application/json", {{"schema", {{"type", "object"}, {"properties", {{"token", {{"type", "string"}, {"format", "uuid"}}}}}}}}}}}}},
                {"400", {{"description", "Bad Request"}}},
                {"401", {{"description", "Unauthorized"}}}}}}}}},
          users_path,
          users_id_path,
          {"/types", types_path},
          {"/types/{type_id}", types_id_path},
          {"/items", items_path},
          {"/items/{item_id}", items_id_path},
          {"/data/{item_id}", data_path},
          {"/reactive_rules", reactive_rules_path},
          {"/reactive_rules/{rule_id}", reactive_rules_id_path},
          {"/deliberative_rules", deliberative_rules_path},
          {"/deliberative_rules/{rule_id}", deliberative_rules_id_path}}},
        {"components", schemas}};
    return open_api;
  }

  inline json::json make_async_api() noexcept
  {
    json::json async_api{
        {"asyncapi", "3.0.0"},
        {"info", {{"title", "Urban Sensing & Planning Engine WebSocket API"}, {"description", "The Urban Sensing & Planning Engine (USPE) WebSocket API"}, {"version", "1.0"}}},
        {"servers", {"uspe", {{"host", SERVER_HOST ":" + std::to_string(SERVER_PORT)}, {"pathname", "/uspe"}, {"protocol", "ws"}}}},
        {"channels", {{"uspe", {{"address", "/"}}}}},
        {"components", {schemas, messages}}};
    return async_api;
  }

  inline int role_to_int(const std::string &role) noexcept
  {
    if (role == "Citizen")
      return 0;
    if (role == "Technician")
      return 1;
    if (role == "Decision Maker")
      return 2;
    if (role == "Admin")
      return 3;
    return -1;
  }
} // namespace uspe
