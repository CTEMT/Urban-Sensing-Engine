(deftemplate sensor_type (slot name))
(deftemplate sensor (slot id) (slot sensor_type) (slot location))

(deftemplate sensor_value (slot sensor_id) (slot local_time) (multislot val))


(deftemplate s1_vals
    (slot v0 (default 0))
    (slot v1 (default 0))
    (slot v2 (default 0))
    (slot v3 (default 0))
    (slot average (default 0))
)

(defrule compute_s1_average
    (sensor_value (sensor_id s1) (val ?v))
    ?vals <- (s1_vals (v0 ?v0) (v1 ?v1) (v2 ?v2) (v3 ?v3))
    =>
    (modify ?vals (v0 ?v1) (v1 ?v2) (v2 ?v3) (v3 ?v) (average (/ (+ ?v1 ?v2 ?v3 ?v) 4)))
)

(defrule s1_high
    (s1_vals (average ?avg))
    (>= ?avg 37.5)
    =>
    (send_message warning "La temperatura del sensore s1 ha superato la soglia di guardia di 37.5")
)

(deffacts initial
    (s1)
)