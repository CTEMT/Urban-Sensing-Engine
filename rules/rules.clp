(deftemplate configuration (slot engine_ptr))
(deftemplate solver (slot solver_ptr) (slot solver_type))

(deftemplate sensor_type (slot name))
(deftemplate sensor (slot id) (slot sensor_type) (multislot location))

(deftemplate sensor_value (slot sensor_id) (slot local_time) (multislot val))

(deftemplate participatory_data (slot user_id) (slot participatory_type) (slot local_time) (multislot val))

(deftemplate task (slot command) (slot task_type) (multislot pars) (multislot vals))

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
    (configuration (engine_ptr ?ptr))
    ?p_data <- (participatory_data (participatory_type road_failure))
    (solver (solver_ptr ?slv) (solver_type maintenance))
    =>
    (send_map_message ?ptr warning 37.503768 15.095338 (str-cat "Attenzione! È stato segnalato un significativo deterioramento del manto stradale"))
    (read_files ?slv (create$ "rules/urban_intelligence_domain.rddl" "rules/urban_intelligence_02_06.rddl"))
    (retract ?p_data)
)

(defrule notify_bus_position
    (configuration (engine_ptr ?ptr))
    ?val <- (sensor_value (sensor_id ?id) (val ?lat ?lng))
    ?bus <- (sensor (id ?id) (sensor_type bus))
    =>
    (send_bus_message ?ptr ?id ?lat ?lng (str-cat "Nr. vettura: " ?id))
    (modify ?bus (location ?lat ?lng))
    (retract ?val)
)

(defrule air_quality_warning
    (configuration (engine_ptr ?ptr))
    ?val <- (sensor_value (sensor_id ?s) (val ?pm10 ?pm2_5))
    (sensor (id ?s) (sensor_type air_monitoring) (location ?lat ?lng))
    (test (or (> ?pm10 40) (> ?pm2_5 25)))
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

(defrule prepare_road_maintainance_documents
    (configuration (engine_ptr ?ptr))
    ?t <- (task (command start) (task_type RoadMaintainanceDocuments) (vals ?dur ?end ?mt ?r ?start ?tau))
    =>
    (send_message ?ptr warning (str-cat "Spettabile " ?mt ", lo stato della strada " ?r " sembra essere peggiorato significativamente nell'ultimo periodo. Al fine di provvedere al suo mantenimento, sono state pianificate un'insieme di attività che porteranno alla riparazione del manto stradale. Come primo passo dovrebbe preparare i documenti per il bando di gara per l'assegnazione delle attività di manutenzione."))
    (retract ?t)
)

(defrule start_road_tender
    (configuration (engine_ptr ?ptr))
    ?t <- (task (command start) (task_type RoadMaintainanceTender) (vals ?dur ?end ?mt ?r ?start ?tau))
    =>
    (send_message ?ptr warning (str-cat "Spettabile " ?mt ", come pianificato, è da oggi in corso la gara d'appalto per la manutenzione della strada " ?r "."))
    (retract ?t)
)

(defrule end_road_tender
    (configuration (engine_ptr ?ptr))
    ?t <- (task (command end) (task_type RoadMaintainanceTender) (vals ?dur ?end ?mt ?r ?start ?tau))
    =>
    (send_message ?ptr warning (str-cat "Spettabile " ?mt ", la gara d'appalto per la manutenzione della strada " ?r " è terminata."))
    (retract ?t)
)

(defrule start_road_maintainance
    (configuration (engine_ptr ?ptr))
    ?t <- (task (command start) (task_type RoadMaintainance) (vals ?dur ?end ?mt ?r ?start ?tau))
    =>
    (send_message ?ptr warning (str-cat "Spettabile " ?mt ", i lavori per la manutenzione della strada " ?r " sono appena cominciati."))
    (retract ?t)
)

(defrule end_road_maintainance
    (configuration (engine_ptr ?ptr))
    ?t <- (task (command end) (task_type RoadMaintainance) (vals ?dur ?end ?mt ?r ?start ?tau))
    =>
    (send_message ?ptr warning (str-cat "Spettabile " ?mt ", i lavori per la manutenzione della strada " ?r " sono appena terminati."))
    (retract ?t)
)

(defrule prepare_green_spaces_maintainance_documents
    (configuration (engine_ptr ?ptr))
    ?t <- (task (command start) (task_type GreenSpacesMaintainanceDocuments) (vals ?dur ?end ?gs ?mt ?start ?tau))
    =>
    (send_message ?ptr warning (str-cat "Spettabile " ?mt ", lo stato dell'area verde " ?gs " sembra essere peggiorato significativamente nell'ultimo periodo. Al fine di provvedere al suo mantenimento, sono state pianificate un'insieme di attività che porteranno alla riparazione del manto stradale. Come primo passo dovrebbe preparare i documenti per il bando di gara per l'assegnazione delle attività di manutenzione."))
    (retract ?t)
)

(defrule start_green_spaces_tender
    (configuration (engine_ptr ?ptr))
    ?t <- (task (command start) (task_type GreenSpacesMaintainanceTender) (vals ?dur ?end ?gs ?mt ?start ?tau))
    =>
    (send_message ?ptr warning (str-cat "Spettabile " ?mt ", come pianificato, è da oggi in corso la gara d'appalto per la manutenzione dell'area verde " ?gs "."))
    (retract ?t)
)

(defrule end_green_spaces_tender
    (configuration (engine_ptr ?ptr))
    ?t <- (task (command end) (task_type GreenSpacesMaintainanceTender) (vals ?dur ?end ?gs ?mt ?start ?tau))
    =>
    (send_message ?ptr warning (str-cat "Spettabile " ?mt ", la gara d'appalto per la manutenzione dell'area verde " ?gs " è terminata."))
    (retract ?t)
)

(defrule start_green_spaces_maintainance
    (configuration (engine_ptr ?ptr))
    ?t <- (task (command start) (task_type GreenSpacesMaintainance) (vals ?dur ?end ?gs ?mt ?start ?tau))
    =>
    (send_message ?ptr warning (str-cat "Spettabile " ?mt ", i lavori per la riqualificazione dell'area verde " ?gs " sono appena cominciati."))
    (retract ?t)
)

(defrule end_green_spaces_maintainance
    (configuration (engine_ptr ?ptr))
    ?t <- (task (command end) (task_type GreenSpacesMaintainance) (vals ?dur ?end ?gs ?mt ?start ?tau))
    =>
    (send_message ?ptr warning (str-cat "Spettabile " ?mt ", i lavori per la riqualificazione dell'area verde " ?gs " sono appena terminati."))
    (retract ?t)
)