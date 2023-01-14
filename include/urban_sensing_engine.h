#pragma once

#include "coco_listener.h"

namespace use
{
  class urban_sensing_engine_listener;

  class urban_sensing_engine : public coco::coco_listener
  {
    friend class urban_sensing_engine_listener;

  public:
    urban_sensing_engine(coco::coco_core &cc);
    ~urban_sensing_engine();

  private:
    friend void send_bus_message(Environment *env, UDFContext *udfc, UDFValue *out);

  private:
    void new_solver(const coco::coco_executor &exec) override;

    void started_solving(const coco::coco_executor &exec) override;
    void solution_found(const coco::coco_executor &exec) override;
    void inconsistent_problem(const coco::coco_executor &exec) override;

    void message_arrived(const std::string &topic, json::json &msg) override;

    void tick(const coco::coco_executor &exec, const semitone::rational &time) override;

    void start(const coco::coco_executor &exec, const std::unordered_set<ratio::core::atom *> &atoms) override;
    void end(const coco::coco_executor &exec, const std::unordered_set<ratio::core::atom *> &atoms) override;

    void update_bus_data(const std::string &bus_id, const long &time, const double &lat, const double &lng, const long &passengers);

  private:
    std::vector<urban_sensing_engine_listener *> listeners; // the urban sensing engine listeners..
  };
} // namespace use
