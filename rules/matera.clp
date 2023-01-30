(deffacts
    (road (road_id r0) (road_name "Via XX Settembre"))
    (road (road_id r1) (road_name "Via Lucana"))
    (road (road_id r2) (road_name "Via Roma"))
    (road (road_id r3) (road_name "Via San Biagio"))
    (road (road_id r4) (road_name "Piazza Vittorio Veneto"))
    (road (road_id r5) (road_name "Via Conservatorio"))
    (road (road_id r6) (road_name "Via Duomo"))
    (road (road_id r7) (road_name "Piazza Duomo"))
    (road (road_id r8) (road_name "Piazzetta Pascoli"))
    (road (road_id r9) (road_name "Piazza del Sedile"))
    (road (road_id r10) (road_name "Via San Potito"))
    (road (road_id r11) (road_name "Via del Castello"))
)

(deffacts
    (building (building_id b0) (building_name "Palazzo dell'Annunziata") (building_address "Piazza Vittorio Veneto") (building_road r4))
    (building (building_id b1) (building_name "Palazzo Bernardini") (building_address "Via Conservatorio") (building_road r5))
    (building (building_id b2) (building_name "Palazzo Bronzini") (building_address "Via Duomo, 2") (building_road r6))
    (building (building_id b3) (building_name "Palazzo Gattini") (building_address "Piazza Duomo, 13") (building_road r7))
    (building (building_id b4) (building_name "Palazzo Lanfranchi") (building_address "Piazzetta Pascoli, 1") (building_road r8))
    (building (building_id b5) (building_name "Palazzo Malvinni-Malvezzi") (building_address "Piazza Duomo, 14") (building_road r7))
    (building (building_id b6) (building_name "Palazzo Ridola") (building_address "Piazza Duomo") (building_road r6))
    (building (building_id b7) (building_name "Palazzo del Sedile") (building_address "Piazza del Sedile") (building_road r9))
    (building (building_id b8) (building_name "Palazzo Venusio") (building_address "Via San Potito") (building_road r10))
)

(deffacts
    (green_area (green_area_id g0) (green_area_name "Castello Tramontano") (green_area_address "Via del Castello") (green_area_road r11))
    (green_area (green_area_id g1) (green_area_name "Parco della Murgia Materana"))
)

(deffacts
    (municipal_technician (municipal_technician_id mt0) (municipal_technician_name "Mario Rossi") (technician_role "Responsabile dell'edilizia pubblica"))
    (municipal_technician (municipal_technician_id mt1) (municipal_technician_name "Giuseppe Verdi") (technician_role "Responsabile della manutenzione stradale"))
    (municipal_technician (municipal_technician_id mt2) (municipal_technician_name "Luigi Bianchi") (technician_role "Responsabile della manutenzione del verde pubblico"))
)

(defrule new_configuration
    (configuration (coco_ptr ?cc_ptr))
    =>
    (new_solver_files ?cc_ptr main (create$ "extern/use/rules/rules.rddl" "extern/use/rules/matera.rddl"))
)