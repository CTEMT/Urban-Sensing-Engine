(defrule env_sensor_high
    (EnvironmentalSensor (item_id ?id) (temperature ?temp&:(> ?temp 45.0)))
    =>
    (do-for-all-facts ((?u User)) TRUE
        (bind ?user-id (fact-slot-value ?u item_id))
        (add_data ?user-id (create$ message) (create$ (str-cat "Alert: High temperature detected by sensor " ?id ": " ?temp "°C")))
    )
)