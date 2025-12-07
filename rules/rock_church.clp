(defrule rock_church
    (RockChurch (item_id ?id))
    =>
    (add_type ?id Church)
)