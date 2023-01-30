(deftemplate road
    (slot road_id)
    (slot road_name)
    (slot s0 (default 1))
    (slot s1 (default 1))
    (slot s2 (default 1))
    (slot s3 (default 1))
    (slot road_state (default 1))
    (slot road_avg_state (default 1))
)

(deftemplate building
    (slot building_id)
    (slot building_name)
    (slot building_address)
    (slot building_type)
    (slot building_road)
    (slot s0 (default 1))
    (slot s1 (default 1))
    (slot s2 (default 1))
    (slot s3 (default 1))
    (slot building_state (default 1))
    (slot building_avg_state (default 1))
)

(deftemplate green_area
    (slot green_area_id)
    (slot green_area_name)
    (slot green_area_address)
    (slot green_area_road)
    (slot s0 (default 1))
    (slot s1 (default 1))
    (slot s2 (default 1))
    (slot s3 (default 1))
    (slot green_area_state (default 1))
    (slot green_area_avg_state (default 1))
)

(deftemplate municipal_technician
    (slot technician_id)
    (slot technician_name)
    (slot technician_role)
)

(defrule execute_solved_problem
    (configuration (coco_ptr ?cc_ptr))
    (solver (solver_ptr ?slv_ptr) (solver_type main) (state idle))
    =>
    (start_execution ?cc_ptr ?slv_ptr)
)

(defrule maintenance_plan
    (configuration (coco_ptr ?cc_ptr))
    (solver (solver_ptr ?slv_ptr) (solver_type main) (state finished))
    (sensor_type (id ?part_type_id) (name "participatory_sensing"))
    (sensor_data (sensor_id ?id) (local_time ?c_time) (data ?lat ?lng ?road))
    ?part <- (sensor (id ?id) (sensor_type ?part_type_id))
    =>
    (send_map_message ?cc_ptr warning ?lat ?lng (str-cat "Attenzione! È stato segnalato un significativo deterioramento del manto stradale"))
    (adapt_files ?slv_ptr (create$ "extern/use/rules/urban_intelligence_02_06.rddl"))
    (modify ?part (location ?lat ?lng))
)

(defrule bus_position
    (configuration (coco_ptr ?cc_ptr))
    (sensor_type (id ?bus_type_id) (name "bus"))
    (sensor_data (sensor_id ?id) (local_time ?c_time) (data ?lat ?lng ?passengers))
    ?bus <- (sensor (id ?id) (sensor_type ?bus_type_id))
    =>
    (send_bus_message ?cc_ptr ?id ?c_time ?lat ?lng ?passengers)
    (modify ?bus (location ?lat ?lng))
)