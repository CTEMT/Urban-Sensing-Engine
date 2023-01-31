(defrule new_configuration
    (configuration (coco_ptr ?cc_ptr))
    =>
    (new_solver_files ?cc_ptr main (create$ "extern/use/rules/rules.rddl" "extern/use/rules/matera.rddl"))
)

(deffacts roads
    (road (road_id r0) (road_name "Via XX Settembre") (road_coordinates 40.66886987440025 16.606201033431592))
    (road (road_id r1) (road_name "Via Lucana") (road_coordinates 40.66184045053739 16.610004133290108))
    (road (road_id r2) (road_name "Via Roma") (road_coordinates 40.667417126550916 16.604441555519582))
    (road (road_id r3) (road_name "Via San Biagio") (road_coordinates 40.667882003624854 16.608006069013957))
    (road (road_id r4) (road_name "Piazza Vittorio Veneto") (road_coordinates 40.6669187521497 16.606513082508386))
    (road (road_id r5) (road_name "Vico Conservatorio") (road_coordinates 40.668147065962714 16.611608001257064))
    (road (road_id r6) (road_name "Via Duomo") (road_coordinates 40.66624791599131 16.61030855551953))
    (road (road_id r7) (road_name "Piazza Duomo") (road_coordinates 40.66679155634107 16.611154282508345))
    (road (road_id r8) (road_name "Piazzetta Pascoli") (road_coordinates 40.663425300341075 16.610178297849036))
    (road (road_id r9) (road_name "Piazza del Sedile") (road_coordinates 40.6660429407916 16.60974561148296))
    (road (road_id r10) (road_name "Via San Potito") (road_coordinates 40.666770004486914 16.61219574017874))
    (road (road_id r11) (road_name "Via del Castello") (road_coordinates 40.662020781572956 16.60025755551943))
    (road (road_id r12) (road_name "Via T. Stigliani") (road_coordinates 40.669571057618185 16.60734906901401))
)

(deffacts buildings
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

(deffacts green_areas
    (green_area (green_area_id g0) (green_area_name "Castello Tramontano") (green_area_address "Via del Castello") (green_area_road r11))
    (green_area (green_area_id g1) (green_area_name "Parco della Murgia Materana"))
    (green_area (green_area_id g2) (green_area_name "Villa dell'Unità d'Italia") (green_area_address "Via T. Stigliani, 50") (green_area_road r12))
)

(deffacts municipal_technicians
    (municipal_technician (technician_id mt0) (technician_name "Mario Rossi") (technician_role "Responsabile dell'edilizia pubblica"))
    (municipal_technician (technician_id mt1) (technician_name "Giuseppe Verdi") (technician_role "Responsabile della manutenzione stradale"))
    (municipal_technician (technician_id mt2) (technician_name "Luigi Bianchi") (technician_role "Responsabile della manutenzione del verde pubblico"))
)