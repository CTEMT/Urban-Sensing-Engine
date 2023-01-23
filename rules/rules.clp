(defrule new_configuration
    (configuration (coco_ptr ?cc_ptr))
    =>
    (new_solver_files ?cc_ptr main (create$ "extern/use/rules/urban_intelligence_domain.rddl"))
)

(defrule execute_solved_problem
    (configuration (coco_ptr ?cc_ptr))
    (solver (solver_ptr ?slv_ptr) (solver_type main) (state idle))
    =>
    (start_execution ?cc_ptr ?slv_ptr)
)

(defrule maintenance_plan
    (configuration (coco_ptr ?cc_ptr))
    (solver (solver_ptr ?slv_ptr) (solver_type main))
    (sensor_data (sensor_id ?id) (local_time ?c_time) (data ?lat ?lng ?road))
    (sensor_type (id ?part_type_id) (name "participatory_sensing"))
    ?part <- (sensor (id ?id) (sensor_type ?part_type_id))
    =>
    (send_map_message ?cc_ptr warning ?lat ?lng (str-cat "Attenzione! È stato segnalato un significativo deterioramento del manto stradale"))
    (adapt_files ?slv_ptr (create$ "extern/use/rules/urban_intelligence_02_06.rddl"))
    (modify ?part (location ?lat ?lng))
)

(defrule bus_position
    (configuration (coco_ptr ?cc_ptr))
    (sensor_data (sensor_id ?id) (local_time ?c_time) (data ?lat ?lng ?passengers))
    (sensor_type (id ?bus_type_id) (name "bus"))
    ?bus <- (sensor (id ?id) (sensor_type ?bus_type_id))
    =>
    (send_bus_message ?cc_ptr ?id ?c_time ?lat ?lng ?passengers)
    (modify ?bus (location ?lat ?lng))
)