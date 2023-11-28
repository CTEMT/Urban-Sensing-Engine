(deftemplate user (slot user_id (type SYMBOL)) (slot user_role (type SYMBOL)) (slot first_name (type STRING)) (slot last_name (type STRING)) (slot email (type STRING)))
(deftemplate skill (slot user_id (type SYMBOL)) (slot name (type SYMBOL)))
(deftemplate intersection (slot intersection_id (type SYMBOL)) (multislot coordinates (type FLOAT)))
(deftemplate road (slot road_id (type SYMBOL)) (slot name (type STRING)) (slot source (type SYMBOL)) (slot target (type SYMBOL)) (slot s0 (default 0)) (slot s1 (default 0)) (slot s2 (default 0)) (slot s3 (default 0)) (slot road_state (default 0)) (slot road_avg_state (default 0)) (slot flow (default 0)) (slot congestion (default FALSE)) (slot probability (default 0)) (slot samples (default 0)))
(deftemplate building (slot building_id (type SYMBOL)) (slot name (type STRING)) (multislot coordinates (type FLOAT)) (slot s0 (default 0)) (slot s1 (default 0)) (slot s2 (default 0)) (slot s3 (default 0)) (slot building_state (default 0)) (slot building_avg_state (default 0)))
(deftemplate vehicle_type (slot vehicle_type_id (type SYMBOL)) (slot name (type STRING)) (slot description (type STRING)) (slot manufacturer (type STRING)))
(deftemplate vehicle (slot vehicle_id (type SYMBOL)) (slot vehicle_type_id (type SYMBOL)) (multislot coordinates (type FLOAT)))

(deftemplate maintaining (slot subject_id (type SYMBOL)))
(deftemplate maintenance_task (slot task_id (type SYMBOL)) (slot subject_id (type SYMBOL)))

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

(defrule start_solver_when_idle
    (solver (solver_ptr ?sp) (state idle))
    =>
    (start_execution ?sp)
)

(defrule delete_solver_when_finished
    (solver (solver_ptr ?sp) (state finished))
    =>
    (delete_solver ?sp)
)

(deffunction get_maintenance_solver ()
    (if (any-factp ((?slv solver)) (eq ?slv:solver_type maintenance))
        then
        (do-for-fact ((?slv solver)) (eq ?slv:solver_type maintenance) (return ?slv:solver_ptr))
        else
        (println "Creating new maintenance solver")
        (bind ?solver (new_solver_files maintenance (create$ "extern/use/rules/maintenance.rddl")))
        (adapt_script ?solver "MaintenanceOffice maintenance_office = new MaintenanceOffice();")
        (adapt_script ?solver (generate_riddle_users))
        (adapt_script ?solver (generate_riddle_roads))
        (adapt_script ?solver (generate_riddle_buildings))
        (return ?solver)
    )
)

(deffunction sensor_data (?sensor ?sensor_type ?time ?data)
    (if (eq (fact-slot-value ?sensor_type name) "participatory")
        then
        (bind ?status (nth$ 1 ?data))
        (bind ?subject_id (sym-cat (nth$ 2 ?data)))
        (bind ?user_id (sym-cat (nth$ 3 ?data)))
        (do-for-fact ((?r road)) (eq ?r:road_id ?subject_id)
            (modify ?r (s0 ?r:s1) (s1 ?r:s2) (s2 ?r:s3) (s3 ?r:road_state) (road_state ?status) (road_avg_state (/ (+ ?r:s1 ?r:s2 ?r:s3 ?r:road_state ?status) 5)))
            (update_road_state ?r:road_id ?r:road_avg_state)
            (if (and (not (any-factp ((?tf maintaining)) (eq ?tf:subject_id ?subject_id))) (>= ?r:road_avg_state 4))
                then
                (println "Road " ?subject_id " is in critical state, requesting maintenance")
                (adapt_script (get_maintenance_solver) (str-cat "goal rm = new maintenance_office.RoadMaintenance(r: r_" ?subject_id ");"))
                (assert (maintaining (subject_id ?subject_id)))
            )
        )
        (do-for-fact ((?b building)) (eq ?b:building_id ?subject_id)
            (modify ?b (s0 ?b:s1) (s1 ?b:s2) (s2 ?b:s3) (s3 ?b:building_state) (building_state ?status) (building_avg_state (/ (+ ?b:s1 ?b:s2 ?b:s3 ?b:building_state ?status) 5)))
            (update_building_state ?b:building_id ?b:building_avg_state)
            (if (and (not (any-factp ((?tf maintaining)) (eq ?tf:subject_id ?subject_id))) (>= ?b:building_avg_state 4))
                then
                (println "Building " ?subject_id " is in critical state, requesting maintenance")
                (adapt_script (get_maintenance_solver) (str-cat "goal bm = new maintenance_office.BuildingMaintenance(b: b_" ?subject_id ");"))
                (assert (maintaining (subject_id ?subject_id)))
            )
        )
    )
)

(deffunction start (?solver_ptr ?id ?task_type ?pars ?vals)
    (if (eq ?task_type RoadMaintenance)
        then
        (bind ?duration (nth$ 1 ?vals))
        (bind ?end (nth$ 2 ?vals))
        (bind ?r (str-replace (nth$ 3 ?vals) "r_" ""))
        (bind ?start (nth$ 4 ?vals))
        (bind ?t (nth$ 5 ?vals))
        (println "Road maintenance task " ?id " started on road " ?r)
        (assert (maintenance_task (task_id ?id) (subject_id ?r)))
    )
    (if (eq ?task_type BuildingMaintenance)
        then
        (bind ?b (str-replace (nth$ 1 ?vals) "b_" ""))
        (bind ?duration (nth$ 2 ?vals))
        (bind ?end (nth$ 3 ?vals))
        (bind ?start (nth$ 4 ?vals))
        (bind ?t (nth$ 5 ?vals))
        (println "Building maintenance task " ?id " started on building " ?b)
        (assert (maintenance_task (task_id ?id) (subject_id ?b)))
    )
)

(deffunction end (?solver_ptr ?id)
    (do-for-fact ((?mt maintenance_task)) (eq ?mt:task_id ?id)
        (do-for-fact ((?r road)) (eq ?r:road_id ?mt:subject_id)
            (println "Road maintenance task " ?id " ended on road " ?mt:subject_id)
            (modify ?r (s0 0) (s1 0) (s2 0) (s3 0) (road_state 0) (road_avg_state 0.0))
            (update_road_state ?r:road_id ?r:road_avg_state)
        )
        (do-for-fact ((?b building)) (eq ?b:building_id ?mt:subject_id)
            (println "Building maintenance task " ?id " ended on building " ?mt:subject_id)
            (modify ?b (s0 0) (s1 0) (s2 0) (s3 0) (building_state 0) (building_avg_state 0.0))
            (update_building_state ?b:building_id ?b:building_avg_state)
        )
        (do-for-fact ((?m maintaining)) (eq ?m:subject_id ?mt:subject_id) (retract ?m))
        (retract ?mt)
    )
)