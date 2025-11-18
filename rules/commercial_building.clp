(defrule commercial_building
    (CommercialBuilding (item_id ?id))
    =>
    (add_type ?id Building)
)