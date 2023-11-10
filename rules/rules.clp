(deftemplate user (slot user_id (type SYMBOL)) (slot user_role (type SYMBOL)) (slot first_name (type STRING)) (slot last_name (type STRING)) (slot email (type STRING)))
(deftemplate skill (slot user_id (type SYMBOL)) (slot name (type SYMBOL)))
(deftemplate road (slot road_id (type SYMBOL)) (slot name (type STRING)) (multislot coordinates (type FLOAT)) (slot s0 (default 1)) (slot s1 (default 1)) (slot s2 (default 1)) (slot s3 (default 1)) (slot road_state (default 1)) (slot local_time (default 0)) (slot road_avg_state (default 1)) (slot flow (default 0)) (slot congestion (default FALSE)) (slot probability (default 0)) (slot samples (default 0)))
(deftemplate building (slot building_id (type SYMBOL)) (slot name (type STRING)) (multislot coordinates (type FLOAT)) (slot s0 (default 1)) (slot s1 (default 1)) (slot s2 (default 1)) (slot s3 (default 1)) (slot building_state (default 1)) (slot local_time (default 0)) (slot building_avg_state (default 1)))
(deftemplate vehicle_type (slot vehicle_type_id (type SYMBOL)) (slot name (type STRING)) (slot description (type STRING)) (slot manufacturer (type STRING)))
(deftemplate vehicle (slot vehicle_id (type SYMBOL)) (slot vehicle_type_id (type SYMBOL)) (multislot coordinates (type FLOAT)))

(defrule r0
    ?strada_predizione <- (road (road_id ss114))
    (road (road_id 3O) (flow ?f3O))
    (test (> ?f3O 1.5))
    =>
    (modify ?strada_predizione (congestion TRUE) (probability 82.24) (samples 304))
)

(defrule r1
    ?strada_predizione <- (road (road_id ss114))
    (road (road_id 3O) (flow ?f3O))
    (road (road_id 2S) (flow ?f2S))
    (test (and (<= ?f3O 1.5) (<= ?f2S 2.5)))
    =>
    (modify ?strada_predizione (congestion FALSE) (probability 100.00) (samples 143))
)

(deffunction sensor_data (?sensor ?sensor_type ?time ?data)
    (if (eq (fact-slot-value ?sensor_type name) "participatory_sensing")
        then
        (bind ?status (nth$ 1 ?data))
        (bind ?subject_id (nth$ 2 ?data))
        (bind ?user_id (nth$ 3 ?data))
        (do-for-fact ((?r road)) (eq ?r:road_id ?subject_id)
            (modify ?r (local_time ?time) (s0 ?r:s1) (s1 ?r:s2) (s2 ?r:s3) (s3 ?r:road_state) (road_state ?status) (road_avg_state (/ (+ ?r:s1 ?r:s2 ?r:s3 ?r:road_state ?status) 5)))
            (update_road_state ?r:road_id ?r:road_avg_state)
        )
        (do-for-fact ((?b building)) (eq ?b:building_id ?subject_id)
            (modify ?b (local_time ?time) (s0 ?b:s1) (s1 ?b:s2) (s2 ?b:s3) (s3 ?b:building_state) (building_state ?status) (building_avg_state (/ (+ ?b:s1 ?b:s2 ?b:s3 ?b:building_state ?status) 5)))
            (update_building_state ?b:building_id ?b:building_avg_state)
        )
    )
)