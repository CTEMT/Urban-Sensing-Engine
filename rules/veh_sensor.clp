(defrule bus
    (VehicularSensor (item_id ?id))
    =>
    (add_type ?id Sensor)
)