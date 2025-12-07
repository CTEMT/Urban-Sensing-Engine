(defrule bus
    (Bus (item_id ?id))
    =>
    (add_type ?id Vehicle)
)