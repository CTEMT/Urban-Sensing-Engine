(defrule pm2_5_high
    (EnvironmentalSensor (item_id ?id) (pm2_5 ?pm2_5))
    =>
    (do-for-all-facts ((?u User)) TRUE
        (bind ?user-id (fact-slot-value ?u item_id))
        (bind ?user-role (fact-slot-value ?u role))
        (if (neq ?user-role guest) then
            (if (and (>= ?pm2_5 30.0) (< ?pm2_5 50.0)) then (add_data ?user-id (create$ message) (create$ (str-cat "Attenzione: il sensore " ?id " ha rilevato un valore di PM2.5 moderato: " ?pm2_5 " µg/m³."))))
            (if (and (>= ?pm2_5 50.0) (< ?pm2_5 75.0)) then (add_data ?user-id (create$ message) (create$ (str-cat "Attenzione: il sensore " ?id " ha rilevato un valore di PM2.5 elevato: " ?pm2_5 " µg/m³."))))
            (if (>= ?pm2_5 75.0) then (add_data ?user-id (create$ message) (create$ (str-cat "Allarme: il sensore " ?id " ha rilevato un valore di PM2.5 molto elevato: " ?pm2_5 " µg/m³."))))
        )
    )
)