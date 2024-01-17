(deftemplate user (slot user_id (type SYMBOL)) (slot user_role (type SYMBOL)) (slot first_name (type STRING)) (slot last_name (type STRING)) (slot email (type STRING)))
(deftemplate skill (slot user_id (type SYMBOL)) (slot name (type SYMBOL)))
(deftemplate intersection (slot intersection_id (type SYMBOL)) (multislot coordinates (type FLOAT)))
(deftemplate road (slot road_id (type SYMBOL)) (slot name (type STRING)) (slot source (type SYMBOL)) (slot target (type SYMBOL)) (slot s0 (default 0)) (slot s1 (default 0)) (slot s2 (default 0)) (slot s3 (default 0)) (slot road_state (default 0)) (slot road_avg_state (default 0)) (slot flow (default 0)) (slot congestion (default FALSE)) (slot probability (default 0)) (slot samples (default 0)))
(deftemplate building (slot building_id (type SYMBOL)) (slot name (type STRING)) (multislot coordinates (type FLOAT)) (slot s0 (default 0)) (slot s1 (default 0)) (slot s2 (default 0)) (slot s3 (default 0)) (slot building_state (default 0)) (slot building_avg_state (default 0)))
(deftemplate vehicle_type (slot vehicle_type_id (type SYMBOL)) (slot name (type STRING)) (slot description (type STRING)) (slot manufacturer (type STRING)))
(deftemplate vehicle (slot vehicle_id (type SYMBOL)) (slot vehicle_type_id (type SYMBOL)) (multislot coordinates (type FLOAT)))
(deftemplate occupancy (slot tile_id (type SYMBOL)) (slot latitude (type FLOAT)) (slot longitude (type FLOAT)) (slot italians (type INTEGER)) (slot foreigners (type INTEGER)) (slot extraregionals (type INTEGER)) (slot intraregionals (type INTEGER)) (slot total (type INTEGER)))

(deftemplate maintaining (slot subject_id (type SYMBOL)))
(deftemplate message (slot task_id (type INTEGER)) (slot message_id (type SYMBOL)) (slot answer (type STRING)))

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

(defrule start_road_maintenance_documents
    ?t <- (task (task_type RoadMaintenanceDocuments) (since 0))
    =>
    (bind ?task_id (fact-slot-value ?t id))
    (bind ?road_id (str-replace (nth$ 3 (fact-slot-value ?t vals)) "r_" ""))
    (bind ?tech_id (str-replace (nth$ 5 (fact-slot-value ?t vals)) "u_" ""))
    (do-for-fact ((?r road)) (eq ?r:road_id ?road_id)
        (println "Road " ?r:name " is in critical state, preparing maintenance documents")
        (bind ?m_id (send_message info ?tech_id (str-cat "La strada " ?r:name " è in uno stato critico. Hai preparato i documenti per la manutenzione?") (create$ "Sì" "No")))
        (println "Asserting message " ?m_id " for task " ?task_id)
        (assert (message (task_id ?task_id) (message_id ?m_id)))
    )
)

(defrule start_building_maintenance_documents
    ?t <- (task (task_type BuildingMaintenanceDocuments) (since 0))
    =>
    (bind ?task_id (fact-slot-value ?t id))
    (bind ?building_id (str-replace (nth$ 1 (fact-slot-value ?t vals)) "b_" ""))
    (bind ?tech_id (str-replace (nth$ 5 (fact-slot-value ?t vals)) "u_" ""))
    (do-for-fact ((?b building)) (eq ?b:building_id ?building_id)
        (println "Building " ?b:name " is in critical state, preparing maintenance documents")
        (bind ?m_id (send_message info ?tech_id (str-cat "L'edificio " ?b:name " è in uno stato critico. Hai preparato i documenti per la manutenzione?") (create$ "Sì" "No")))
        (println "Asserting message " ?m_id " for task " ?task_id)
        (assert (message (task_id ?task_id) (message_id ?m_id)))
    )
)

(defrule start_road_maintenance
    ?t <- (task (task_type RoadMaintenance) (since 0))
    =>
    (bind ?road_id (str-replace (nth$ 3 (fact-slot-value ?t vals)) "r_" ""))
    (do-for-fact ((?r road)) (eq ?r:road_id ?road_id)
        (println "Starting road maintenance on road " ?r:name)
    )
)

(defrule start_building_maintenance
    ?t <- (task (task_type BuildingMaintenance) (since 0))
    =>
    (bind ?building_id (str-replace (nth$ 1 (fact-slot-value ?t vals)) "b_" ""))
    (do-for-fact ((?b building)) (eq ?b:building_id ?building_id)
        (println "Starting building maintenance on building " ?b:name)
    )
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
        (adapt_script ?solver "goal bruna = new Bruna(start: 130.0, end: 150.0);")
        (return ?solver)
    )
)

(deffunction sensor_data (?sensor ?sensor_type ?time ?data)
    (if (eq (fact-slot-value ?sensor_type name) "participatory")
        then
        (bind ?status (nth$ 1 ?data))
        (bind ?subject_id (sym-cat (nth$ 2 ?data)))
        (bind ?user_id (sym-cat (nth$ 3 ?data)))
        (println "Received participatory data from user " ?user_id " on " ?subject_id " with status " ?status)
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
    (if (eq (fact-slot-value ?sensor_type name) "matera_station_type0")
        then
        (bind ?pm1 (nth$ 34 ?data))
        (bind ?pm10 (nth$ 35 ?data))
        (bind ?pm25 (nth$ 40 ?data))
        (if (>= ?pm10 40)
            then
            (println "Received data from sensor " ?sensor " with PM10 value " ?pm10)
            (do-for-all-facts ((?u user)) (eq ?u:user_role technician)
                (send_message warning ?u:user_id (str-cat "Il sensore " (fact-slot-value ?sensor name) " ha rilevato un valore di PM10 pari a " ?pm10 ".") (create$ "Ok"))
            )
        )
        (if (>= ?pm25 25)
            then
            (println "Received data from sensor " ?sensor " with PM2.5 value " ?pm25)
            (do-for-all-facts ((?u user)) (eq ?u:user_role technician)
                (send_message warning ?u:user_id (str-cat "Il sensore " (fact-slot-value ?sensor name) " ha rilevato un valore di PM2.5 pari a " ?pm25 ".") (create$ "Ok"))
            )
        )
    )
)

(deffunction ending (?solver_ptr ?id)
    (do-for-fact ((?q message)) (eq ?q:task_id ?id)
        (if (or (eq ?q:answer "") (eq ?q:answer "No"))
            then
            (return FALSE)
        )
    )
    (return TRUE)
)

(deffunction end (?solver_ptr ?id)
    (do-for-fact ((?t task)) (and (eq ?t:task_type RoadMaintenance) (eq ?t:id ?id))
        (bind ?road_id (str-replace (nth$ 3 ?t:vals) "r_" ""))
        (println "Road maintenance task " ?t:id " ended on road " ?road_id)
        (do-for-fact ((?r road)) (eq ?r:road_id ?road_id)
            (println "Road " ?r:name " is now in good state")
            (modify ?r (s0 0) (s1 0) (s2 0) (s3 0) (road_state 0) (road_avg_state 0.0))
            (update_road_state ?road_id ?r:road_avg_state)
        )
        (do-for-fact ((?tf maintaining)) (eq ?tf:subject_id ?road_id) (retract ?tf))
    )
    (do-for-fact ((?t task)) (and (eq ?t:task_type BuildingMaintenance) (eq ?t:id ?id))
        (bind ?building_id (str-replace (nth$ 1 ?t:vals) "b_" ""))
        (println "Building maintenance task " ?t:id " ended on building " ?building_id)
        (do-for-fact ((?b building)) (eq ?b:building_id ?building_id)
            (println "Building " ?b:name " is now in good state")
            (modify ?b (s0 0) (s1 0) (s2 0) (s3 0) (building_state 0) (building_avg_state 0.0))
            (update_building_state ?building_id ?b:building_avg_state)
        )
        (do-for-fact ((?tf maintaining)) (eq ?tf:subject_id ?building_id) (retract ?tf))
    )
    (remove_task ?solver_ptr ?id)
)

(deffunction answer_message (?m_id ?answer)
    (do-for-fact ((?q message)) (eq ?q:message_id ?m_id)
        (modify ?q (answer ?answer))
    )
)