(defrule government_building
    (GovernmentBuilding (item_id ?id))
    =>
    (add_type ?id Building)
)