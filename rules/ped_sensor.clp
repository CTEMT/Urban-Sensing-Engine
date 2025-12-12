(defrule ped_sensor
    (PedestrianSensor (item_id ?id))
    =>
    (add_type ?id Sensor)
)