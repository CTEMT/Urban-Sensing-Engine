import sys
import requests
import logging

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

handler = logging.StreamHandler(sys.stdout)
handler.setLevel(logging.DEBUG)
logger.addHandler(handler)


def create_types(session: requests.Session, url: str):
    # Create the 'Sensor' type..
    response = session.post(url + '/types', json={
        'name': 'Sensor'
    })
    if response.status_code != 204:
        logger.error('Failed to create Sensor type')
        return

    # Create the 'FixedSensor' type
    response = session.post(url + '/types', json={
        'name': 'FixedSensor',
        'parents': ['Sensor'],
        'static_properties': {
            'lat': {'type': 'float'},
            'lng': {'type': 'float'}
        }
    })
    if response.status_code != 204:
        logger.error('Failed to create FixedSensor type')
        return

    # Create the 'MobileSensor' type
    response = session.post(url + '/types', json={
        'name': 'MobileSensor',
        'parents': ['Sensor'],
        'dynamic_properties': {
            'lat': {'type': 'float'},
            'lng': {'type': 'float'}
        }
    })
    if response.status_code != 204:
        logger.error('Failed to create MobileSensor type')
        return


if __name__ == '__main__':
    url = sys.argv[1] if len(sys.argv) > 1 else 'https://localhost:8080'
    session = requests.Session()

    create_types(session, url)
