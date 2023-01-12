(defrule bus_position
    (configuration (coco_ptr ?ptr))
    (sensor_data (sensor_id ?id) (local_time ?c_time) (data ?lat ?lng ?passengers))
    (sensor_type (id ?bus_type_id) (name "bus"))
    ?bus <- (sensor (id ?id) (sensor_type ?bus_type_id))
    =>
    (send_bus_message ?ptr ?id ?c_time ?lat ?lng ?passengers)
    (modify ?bus (location ?lat ?lng))
)