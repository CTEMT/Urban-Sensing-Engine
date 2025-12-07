(defrule residence
    (Residence (item_id ?id))
    =>
    (add_type ?id Building)
)