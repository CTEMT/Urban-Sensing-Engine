(defrule pm10_high
    (EnvironmentalSensor (item_id ?id) (pm10 ?pm10))
    =>
    (do-for-all-facts ((?u User)) TRUE
        (bind ?user-id (fact-slot-value ?u item_id))
        (bind ?user-role (fact-slot-value ?u role))
        (if (neq ?user-role guest) then
            (if (and (>= ?pm10 50.0) (< ?pm10 90.0)) then (add_data ?user-id (create$ message) (create$ (str-cat "Attenzione: il sensore " ?id " ha rilevato un valore di PM10 elevato: " ?pm10 " µg/m³."))))
            (if (>= ?pm10 90.0) then (add_data ?user-id (create$ alert) (create$ (str-cat "Allarme: il sensore " ?id " ha rilevato un valore di PM10 molto elevato: " ?pm10 " µg/m³."))))
        )
    )
)