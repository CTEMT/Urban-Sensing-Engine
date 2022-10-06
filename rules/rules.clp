(deftemplate configuration (slot engine_ptr))

(deftemplate sensor_type (slot name))
(deftemplate sensor (slot id) (slot sensor_type) (multislot location))

(deftemplate sensor_value (slot sensor_id) (slot local_time) (multislot val))

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

(deftemplate s0_vals
    (slot v0 (default 0))
    (slot v1 (default 0))
    (slot v2 (default 0))
    (slot v3 (default 0))
    (slot local_time (default 0))
    (slot average (default 0))
)

(defrule compute_s0_average
    ?val <- (sensor_value (sensor_id s0) (local_time ?t) (val ?v))
    ?vals <- (s0_vals (v0 ?v0) (v1 ?v1) (v2 ?v2) (v3 ?v3))
    =>
    (modify ?vals (v0 ?v1) (v1 ?v2) (v2 ?v3) (v3 ?v) (local_time ?t) (average (/ (+ ?v1 ?v2 ?v3 ?v) 4)))
    (retract ?val)
)

(defrule s0_high
    (sensor (id s0) (location ?lat ?lng))
    (s0_vals (average ?avg))
    (test (>= ?avg 37.5))
    (configuration (engine_ptr ?ptr))
    =>
    (println (str-cat "La temperatura media del sensore 's0', di " ?avg ", ha superato la soglia di guardia di 37.5"))
    (send_map_message ?ptr warning ?lat ?lng (str-cat "La temperatura media del sensore 's0', di " ?avg ", ha superato la soglia di guardia di 37.5"))
)

(deffacts initial_facts
    (s0_vals)
)