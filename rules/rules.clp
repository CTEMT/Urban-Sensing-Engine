(deftemplate configuration (slot engine_ptr))
(deftemplate solver (slot solver_ptr) (slot solver_type))

(deftemplate sensor_type (slot name))
(deftemplate sensor (slot id) (slot sensor_type) (multislot location))

(deftemplate sensor_value (slot sensor_id) (slot local_time) (multislot val))
(deftemplate participatory (slot participatory_type) (slot local_time) (multislot location) (multislot val))

(deftemplate average_temp
    (slot sensor_id)
    (slot v0 (default 0))
    (slot v1 (default 0))
    (slot v2 (default 0))
    (slot v3 (default 0))
    (slot local_time (default 0))
    (slot average (default 0))
)

(defrule new_configuration
    (configuration (engine_ptr ?engine_ptr))
    =>
    (assert (solver (solver_ptr (new_solver ?engine_ptr)) (solver_type maintenance)))
)

(defrule maintenance_request
    (participatory (participatory_type road_failure))
    (solver (solver_ptr ?slv) (solver_type maintenance))
    =>
    (read_problem ?slv (create$ "rules/urban_intelligence_domain.rddl" "rules/urban_intelligence_02_03.rddl"))
)

(defrule notify_bus_position
    ?val <- (sensor_value (sensor_id ?id) (val ?lat ?lng))
    ?bus <- (sensor (id ?id) (sensor_type bus))
    =>
    (println (str-cat "L'autobus " ?id " si trova alle coordinate [" ?lat ", " ?lng "]"))
    (modify ?bus (location ?lat ?lng))
    (retract ?val)
)

(defrule air_quality_warning
    ?val <- (sensor_value (sensor_id ?s) (val ?pm10 ?pm2_5))
    (sensor (id ?s) (sensor_type air_monitoring) (location ?lat ?lng))
    (test (or (> ?pm10 40) (> ?pm2_5 25)))
    (configuration (engine_ptr ?ptr))
    =>
    (send_map_message ?ptr warning ?lat ?lng (str-cat "Attenzione! La qualità dell'aria percepita dal sensore " ?s " ha superato i limiti di soglia"))
    (retract ?val)
)

(defrule new_temp_sensor
    (sensor (id ?s) (sensor_type temperature))
    =>
    (assert (average_temp (sensor_id ?s)))
)

(defrule compute_average_temp
    ?val <- (sensor_value (sensor_id ?s) (local_time ?t) (val ?v))
    ?vals <- (average_temp (sensor_id ?s) (v0 ?v0) (v1 ?v1) (v2 ?v2) (v3 ?v3))
    =>
    (modify ?vals (v0 ?v1) (v1 ?v2) (v2 ?v3) (v3 ?v) (local_time ?t) (average (/ (+ ?v1 ?v2 ?v3 ?v) 4)))
    (retract ?val)
)

(defrule high_temp
    (sensor (id ?s) (location ?lat ?lng))
    (average_temp (sensor_id ?s) (average ?avg))
    (test (>= ?avg 37.5))
    (configuration (engine_ptr ?ptr))
    =>
    (send_map_message ?ptr warning ?lat ?lng (str-cat "La temperatura media del sensore '" ?s "', di " ?avg "°, ha superato la soglia di guardia di 37.5°"))
)