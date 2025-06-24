import sys
import requests
import logging
import warnings
from urllib3.exceptions import InsecureRequestWarning

# Suppress InsecureRequestWarning from urllib3
warnings.filterwarnings('ignore', category=InsecureRequestWarning)

# Configure logging
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

handler = logging.StreamHandler(sys.stdout)
handler.setLevel(logging.DEBUG)
logger.addHandler(handler)


def create_types(session: requests.Session, url: str):
    # Create the 'Vehicle' type
    response = requests.post(url + '/types', json={
        'name': 'Vehicle',
        'static_properties': {
            'vehicle_id': {'type': 'string'},
            'vehicle_type': {'type': 'string'}
        }
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create Vehicle type')
        return

    # Create the 'Location' type
    response = requests.post(url + '/types', json={
        'name': 'Location',
        'static_properties': {
            'lat': {'type': 'float'},
            'lng': {'type': 'float'},
            'name': {'type': 'string'}
        }
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create Location type')
        return

    # Create the 'TrafficLight' type
    response = requests.post(url + '/types', json={
        'name': 'TrafficLight',
        'static_properties': {
            'location': {'type': 'item', 'domain': 'Location'}
        },
        'dynamic_properties': {
            'state': {'type': 'string', 'enum': ['red', 'green', 'yellow']}
        }
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create TrafficLight type')
        return

    # Create the 'PointOfInterest' type
    response = requests.post(url + '/types', json={
        'name': 'PointOfInterest',
        'static_properties': {
            'name': {'type': 'string'},
            'location': {'type': 'item', 'domain': 'Location'},
            'category': {'type': 'symbol', 'values': ['church', 'hotel', 'museum', 'historical_site', 'palace', 'facility', 'square', 'monastery', 'convent', 'terrace', 'park', 'library', 'castle', 'tower', 'mall', 'abandoned_populated_place', 'restaurant', 'cafe', 'fast_food', 'cultural_site']}
        }
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create PointOfInterest type')
        return

    # Create the 'BusStop' type
    response = requests.post(url + '/types', json={
        'name': 'BusStop',
        'static_properties': {
            'name': {'type': 'string'},
            'location': {'type': 'item', 'domain': 'Location'},
            'stop_id': {'type': 'string'}
        }
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create BusStop type')
        return

    # Create the 'Road' type
    response = requests.post(url + '/types', json={
        'name': 'Road',
        'static_properties': {
            'name': {'type': 'string'},
            'length': {'type': 'float'},
            'start': {'type': 'item', 'domain': 'Location'},
            'end': {'type': 'item', 'domain': 'Location'}
        }
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create Road type')
        return

    # Create the 'Sensor' type..
    response = requests.post(url + '/types', json={
        'name': 'Sensor'
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create Sensor type')
        return

    # Create the 'FixedSensor' type
    response = requests.post(url + '/types', json={
        'name': 'FixedSensor',
        'parents': ['Sensor'],
        'static_properties': {
            'location': {'type': 'item', 'domain': 'Location'}
        }
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create FixedSensor type')
        return

    # Create the 'EnvironmentalSensor' type
    response = requests.post(url + '/types', json={
        'name': 'EnvironmentalSensor',
        'parents': ['FixedSensor']
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create EnvironmentalSensor type')
        return

    # Create the 'TemperatureSensor' type
    response = requests.post(url + '/types', json={
        'name': 'TemperatureSensor',
        'parents': ['EnvironmentalSensor'],
        'dynamic_properties': {
            'temperature': {'type': 'float'}
        }
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create TemperatureSensor type')
        return

    # Create the 'MobileSensor' type
    response = requests.post(url + '/types', json={
        'name': 'MobileSensor',
        'parents': ['Sensor'],
        'dynamic_properties': {
            'lat': {'type': 'float'},
            'lng': {'type': 'float'}
        }
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create MobileSensor type')
        return

    # Create the 'TrackedVehicle' type
    response = requests.post(url + '/types', json={
        'name': 'TrackedVehicle',
        'parents': ['Vehicle'],
        'static_properties': {
            'gps': {'type': 'item', 'domain': 'MobileSensor'}
        }
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create TrackedVehicle type')
        return

    # Create the 'Bus' type
    response = requests.post(url + '/types', json={
        'name': 'Bus',
        'parents': ['TrackedVehicle'],
        'static_properties': {
            'capacity': {'type': 'int'}
        },
        'dynamic_properties': {
            'route': {'type': 'string'},
            'current_stop': {'type': 'string'},
            'next_stop': {'type': 'string'}
        }
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create Bus type')
        return

    # Create the 'Event' type
    response = requests.post(url + '/types', json={
        'name': 'Event'
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create Event type')
        return

    # Create the 'Service' type
    response = requests.post(url + '/types', json={
        'name': 'Service'
    }, verify=False)
    if response.status_code != 204:
        logger.error('Failed to create Service type')
        return


if __name__ == '__main__':
    url = sys.argv[1] if len(sys.argv) > 1 else 'https://localhost:8080'
    session = requests.Session()

    create_types(session, url)
