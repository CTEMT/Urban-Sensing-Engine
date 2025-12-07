(defrule env_sensor
    (EnvironmentalSensor (item_id ?id))
    =>
    (add_type ?id Sensor)
)