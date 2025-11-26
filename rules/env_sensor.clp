(defrule bus
    (EnvironmentalSensor (item_id ?id))
    =>
    (add_type ?id Sensor)
)