# Topics and data format

## Topics

| Topic | Description |
| --- | --- |
| `<root>/sensors` | Updates the sensor network |

## Sensor network

| Description | Format |
| --- | --- |
| Updates the sensor network | |
| Adds a new sensor type | `{"type": "new_sensor_type", "name": <name>, "description": <description>}` |
| Updates an existing sensor type | `{"type": "update_sensor_type", "id": <id>[, "name": <name>][, "description": <description>]}` |
| Deletes an existing sensor type | `{"type": "delete_sensor_type", "id": <id>}` |
| Adds a new sensor | `{"type": "new_sensor", "type": <type_id> [, "location": {"lat": <lat>, "lng": <lng>}]}` |
| Updates an existing sensor | `{"type": "update_sensor", "id": <id>[, "type": <type_id>][, "location": {"lat": <lat>, "lng": <lng>}]}` |
| Deletes an existing sensor | `{"type": "delete_sensor", "id": <id>}` |