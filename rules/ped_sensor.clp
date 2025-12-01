(defrule bus
    (PedestrianSensor (item_id ?id))
    =>
    (add_type ?id Sensor)
)