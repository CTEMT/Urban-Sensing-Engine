#pragma once

#include "api.hpp"
#include "uspe_db.hpp"
#include "server.hpp"

namespace uspe
{
  /**
   * @brief Urban Sensing and Planning Engine (USPE) module.
   *
   * This module provides functionality for the Urban Sensing and Planning Engine (USPE).
   * The `uspe` class represents the main interface for interacting with the USPE module.
   */
  class uspe_server final : public coco::coco_core, public network::server
  {
  public:
    /**
     * @brief Constructs a new Urban Sensing and Planning Engine object with the given database.
     *
     * @param db A unique pointer to the `uspe_db` object that represents the Urban Sensing and Planning Engine database.
     */
    uspe_server(std::unique_ptr<uspe_db> &&db);

    /**
     * @brief Starts the server.
     *
     * This function starts the server by calling the `network::server::start()` function.
     */
    void start() { network::server::start(); }

  private:
    uspe_db &get_db() { return static_cast<uspe_db &>(*db); }

    std::unique_ptr<network::response> index(network::request &req);
    std::unique_ptr<network::response> assets(network::request &req);
    std::unique_ptr<network::response> open_api(network::request &req);
    std::unique_ptr<network::response> async_api(network::request &req);

    std::unique_ptr<network::response> login(network::request &req);
    std::unique_ptr<network::response> create_user(network::request &req);
    std::unique_ptr<network::response> update_user(network::request &req);
    std::unique_ptr<network::response> delete_user(network::request &req);

    void on_ws_open(network::ws_session &ws);
    void on_ws_message(network::ws_session &ws, const std::string &msg);
    void on_ws_close(network::ws_session &ws);
    void on_ws_error(network::ws_session &ws, const boost::system::error_code &);

  private:
    const json::json j_open_api = make_open_api();
    const json::json j_async_api = make_async_api();

    std::unordered_map<network::ws_session *, std::string> ws_to_user;
    std::unordered_map<std::string, std::unordered_set<network::ws_session *>> user_to_wss;
  };
} // namespace uspe
