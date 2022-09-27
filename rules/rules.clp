(deftemplate configuration (slot engine_ptr))

(deftemplate sensor_type (slot name))
(deftemplate sensor (slot id) (slot sensor_type) (slot location))

(deftemplate sensor_value (slot sensor_id) (slot local_time) (multislot val))


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
    (s0_vals (average ?avg&:(>= ?avg 37.5)))
    (configuration (engine_ptr ?ptr))
    =>
    (send_message ?ptr warning (str-cat "La temperatura media del sensore 's0', di " ?avg ", ha superato la soglia di guardia di 37.5"))
)

(deffacts initial_facts
    (s0_vals)
)