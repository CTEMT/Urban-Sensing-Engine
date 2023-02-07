(deftemplate road
    (slot road_id)
    (slot road_name)
    (multislot road_coordinates)
    (slot s0 (default 1))
    (slot s1 (default 1))
    (slot s2 (default 1))
    (slot s3 (default 1))
    (slot road_state (default 1))
    (slot local_time (default 0))
    (slot road_avg_state (default 1))
    (slot planned_maintenance (default FALSE))
)

(deftemplate building
    (slot building_id)
    (slot building_name)
    (slot building_address)
    (multislot building_coordinates)
    (slot building_type)
    (slot building_road)
    (slot s0 (default 1))
    (slot s1 (default 1))
    (slot s2 (default 1))
    (slot s3 (default 1))
    (slot building_state (default 1))
    (slot local_time (default 0))
    (slot building_avg_state (default 1))
    (slot planned_maintenance (default FALSE))
)

(deftemplate green_area
    (slot green_area_id)
    (slot green_area_name)
    (slot green_area_address)
    (multislot green_area_coordinates)
    (slot green_area_road)
    (slot s0 (default 1))
    (slot s1 (default 1))
    (slot s2 (default 1))
    (slot s3 (default 1))
    (slot green_area_state (default 1))
    (slot local_time (default 0))
    (slot green_area_avg_state (default 1))
    (slot planned_maintenance (default FALSE))
)

(deftemplate municipal_technician
    (slot technician_id)
    (slot technician_name)
    (slot technician_role)
)

(defrule message_sender
    (configuration (coco_ptr ?cc_ptr))
    (sensor_type (id ?message_sender_type_id) (name "message_sender"))
    (sensor_data (sensor_id ?id) (local_time ?c_time) (data ?message))
    =>
    (send_message ?cc_ptr warning ?message)
)

(defrule execute_solved_problem
    (configuration (coco_ptr ?cc_ptr))
    (solver (solver_ptr ?slv_ptr) (solver_type main) (state idle))
    =>
    (start_execution ?cc_ptr ?slv_ptr)
)

(defrule compute_average_road_state
    ?sd <- (sensor_data (sensor_id ?id) (local_time ?t) (data ?status ?subject_id))
    ?road <- (road (road_id ?subject_id) (s0 ?s0) (s1 ?s1) (s2 ?s2) (s3 ?s3) (road_state ?state))
    =>
    (modify ?road (local_time ?t) (s0 ?s1) (s1 ?s2) (s2 ?s3) (s3 ?state) (road_state ?status) (road_avg_state (/ (+ ?s0 ?s1 ?s2 ?s3 ?state ?status) 6)))
    (retract ?sd)
)

(defrule trigger_road_manintenance
    (configuration (coco_ptr ?cc_ptr))
    (solver (solver_ptr ?slv_ptr) (solver_type main) (state idle|executing|finished))
    ?road <- (road (road_id ?id) (road_name ?name) (road_coordinates ?lat ?lng) (road_avg_state ?avg_state) (planned_maintenance FALSE))
    (test (<= ?avg_state 0.2))
    =>
    (send_map_message ?cc_ptr warning ?lat ?lng (str-cat "Attenzione! È stato segnalato un significativo deterioramento del manto stradale in " ?name "."))
    (adapt_script ?slv_ptr (str-cat "goal rm_" ?id " = new agnt.RoadMaintainance(r:" ?id ");"))
    (modify ?road (planned_maintenance TRUE))
)

(defrule road_maintenance_documents
    (configuration (coco_ptr ?cc_ptr))
    (task (command start) (id ?id) (task_type RoadMaintainanceDocuments) (vals ?duration ?end ?mt ?r ?start ?tau))
    (road (road_id ?r) (road_name ?name) (road_coordinates ?lat ?lng))
    (municipal_technician (technician_id ?mt) (technician_name ?mt_name))
    =>
    (send_message ?cc_ptr info (str-cat "Inizio preparazione documenti per la gara d'appalto relativa alla manutenzione stradale in " ?name ". Il compito è stato assegnato a " ?mt_name "."))
)

(defrule road_maintenance_tender
    (configuration (coco_ptr ?cc_ptr))
    (task (command start) (id ?id) (task_type RoadMaintainanceTender) (vals ?duration ?end ?mt ?r ?start ?tau))
    (road (road_id ?r) (road_name ?name) (road_coordinates ?lat ?lng))
    (municipal_technician (technician_id ?mt) (technician_name ?mt_name))
    =>
    (send_message ?cc_ptr info (str-cat "Inizio gara d'appalto relativa alla manutenzione stradale in " ?name ". Il compito è stato assegnato a " ?mt_name "."))
)

(defrule road_maintenance
    (configuration (coco_ptr ?cc_ptr))
    (task (command start) (id ?id) (task_type RoadMaintainance) (vals ?duration ?end ?mt ?r ?start ?tau))
    (road (road_id ?r) (road_name ?name) (road_coordinates ?lat ?lng))
    (municipal_technician (technician_id ?mt) (technician_name ?mt_name))
    =>
    (send_message ?cc_ptr info (str-cat "Inizio manutenzione stradale in " ?name ". Il compito è stato assegnato a " ?mt_name "."))
)

(defrule road_maintenance_finished
    (configuration (coco_ptr ?cc_ptr))
    (task (command end) (id ?id) (task_type RoadMaintainance) (vals ?duration ?end ?mt ?r ?start ?tau))
    ?road <- (road (road_id ?r) (road_name ?name) (road_coordinates ?lat ?lng))
    =>
    (send_message ?cc_ptr info (str-cat "Fine manutenzione stradale in " ?name "."))
    (modify ?road (planned_maintenance FALSE))
)

(defrule compute_average_building_state
    ?sd <- (sensor_data (sensor_id ?id) (local_time ?t) (data ?status ?subject_id))
    ?building <- (building (building_id ?subject_id) (s0 ?s0) (s1 ?s1) (s2 ?s2) (s3 ?s3) (building_state ?state))
    =>
    (modify ?building (local_time ?t) (s0 ?s1) (s1 ?s2) (s2 ?s3) (s3 ?state) (building_state ?status) (building_avg_state (/ (+ ?s0 ?s1 ?s2 ?s3 ?state ?status) 6)))
    (retract ?sd)
)

(defrule trigger_building_manintenance
    (configuration (coco_ptr ?cc_ptr))
    (solver (solver_ptr ?slv_ptr) (solver_type main) (state idle|executing|finished))
    ?building <- (building (building_id ?id) (building_name ?name) (building_address ?address) (building_coordinates ?lat ?lng) (building_avg_state ?avg_state) (planned_maintenance FALSE))
    (<= ?avg_state 0.2)
    =>
    (send_map_message ?cc_ptr warning ?lat ?lng (str-cat "Attenzione! È stato segnalato un significativo deterioramento di " ?name ", in " ?address "."))
    (adapt_script ?slv_ptr (str-cat "goal bm_" ?id " = new agnt.BuildingMaintainance(b:" ?id ");"))
    (modify ?building (planned_maintenance TRUE))
)

(defrule compute_average_green_area_state
    ?sd <- (sensor_data (sensor_id ?id) (local_time ?t) (data ?status ?subject_id))
    ?green_area <- (green_area (green_area_id ?subject_id) (s0 ?s0) (s1 ?s1) (s2 ?s2) (s3 ?s3) (green_area_state ?state))
    =>
    (modify ?green_area (local_time ?t) (s0 ?s1) (s1 ?s2) (s2 ?s3) (s3 ?state) (green_area_state ?status) (green_area_avg_state (/ (+ ?s0 ?s1 ?s2 ?s3 ?state ?status) 6)))
    (retract ?sd)
)

(defrule trigger_green_area_manintenance
    (configuration (coco_ptr ?cc_ptr))
    (solver (solver_ptr ?slv_ptr) (solver_type main) (state idle|executing|finished))
    ?green_area <- (green_area (green_area_id ?id) (green_area_name ?name) (green_area_address ?address) (green_area_coordinates ?lat ?lng) (green_area_avg_state ?avg_state) (planned_maintenance FALSE))
    (<= ?avg_state 0.2)
    =>
    (send_map_message ?cc_ptr warning ?lat ?lng (str-cat "Attenzione! È stato segnalato un significativo deterioramento di " ?name ", in " ?address "."))
    (adapt_script ?slv_ptr (str-cat "goal gam_" ?id " = new agnt.GreenAreaMaintainance(ga:" ?id ");"))
    (modify ?green_area (planned_maintenance TRUE))
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