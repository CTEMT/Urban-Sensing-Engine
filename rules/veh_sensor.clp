(defrule veh_sensor
    (VehicularSensor (item_id ?id))
    =>
    (add_type ?id Sensor)
)