import json
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


def import_data(session: requests.Session, url: str, locations_url: str = None, roads_url: str = None):
    locations_response = requests.get(locations_url)
    if locations_response.status_code != 200:
        logger.error('Failed to fetch locations from %s', locations_url)
        return
    locations = locations_response.json()[0]["features"]
    logger.info('Fetched %d locations', len(locations))

    osmid_to_id = {}

    for location in locations:
        response = requests.post(url + '/items', json={
            'type': 'Location',
            'properties': {
                'lat': location['properties']['y'],
                'lng': location['properties']['x']
            }}, verify=False)
        if response.status_code == 201:
            item_id = response.text
            osmid = location['properties']['osmid']
            osmid_to_id[osmid] = item_id
        else:
            logger.error('Failed to create location for osmid %s',
                         location['properties']['osmid'])

    # roads_response = requests.get(roads_url)
    # if roads_response.status_code != 200:
    #     logger.error('Failed to fetch roads from %s', roads_url)
    #     return
    # roads = roads_response.json()[0]["features"]
    # logger.info('Fetched %d roads', len(roads))

    # for road in roads:
    #     response = requests.post(url + '/items', json={
    #         'type': 'Road',
    #         'properties': {
    #             'name': road['properties']['name'],
    #             'start': osmid_to_id.get(road['properties']['u']),
    #             'end': osmid_to_id.get(road['properties']['v']),
    #             'length': road['properties']['length']
    #         }}, verify=False)
    #     if response.status_code != 201:
    #         logger.error('Failed to create road for u %s, v %s',
    #                      road['properties']['u'],
    #                      road['properties']['v'])

    pois = json.load(open('matera_POI.geojson'))
    logger.info('Fetched %d POIs', len(pois['features']))
    for poi in pois['features']:
        osmid = poi['properties']['osm_id']
        if osmid not in osmid_to_id:
            response = requests.post(url + '/items', json={
                'type': 'Location',
                'properties': {
                    'lat': poi['properties']['latitude'],
                    'lng': poi['properties']['longitude']
                }}, verify=False)
            if response.status_code == 201:
                item_id = response.text
                osmid = location['properties']['osm_id']
                osmid_to_id[osmid] = item_id
            else:
                logger.error('Failed to create location for osmid %s',
                             location['properties']['osm_id'])

        if poi['properties']['poitype'] == 'historical site':
            poi['properties']['poitype'] = 'historical_site'
        elif poi['properties']['poitype'] == 'abandoned populated place':
            poi['properties']['poitype'] = 'abandoned_populated_place'
        elif poi['properties']['poitype'] == 'Cafe':
            poi['properties']['poitype'] = 'cafe'
        elif poi['properties']['poitype'] == 'Fast food':
            poi['properties']['poitype'] = 'fast_food'
        elif poi['properties']['poitype'] == 'cultural site':
            poi['properties']['poitype'] = 'cultural_site'

        response = requests.post(url + '/items', json={
            'type': 'PointOfInterest',
            'properties': {
                'name': poi['properties']['name'],
                'location': osmid_to_id.get(poi['properties']['osm_id']),
                'category': poi['properties']['poitype']
            }}, verify=False)
        if response.status_code != 201:
            logger.error('Failed to create POI for osmid %s',
                         poi['properties']['osm_id'])


if __name__ == '__main__':
    url = sys.argv[1] if len(sys.argv) > 1 else 'https://localhost:8080'
    locations_url = sys.argv[2] if len(
        sys.argv) > 2 else 'https://matera-rest-api.na.icar.cnr.it/search?db=matera_graph2&collection=matera_nodes'
    roads_url = sys.argv[3] if len(
        sys.argv) > 3 else 'https://matera-rest-api.na.icar.cnr.it/search?db=matera_graph2&collection=matera_edges'
    session = requests.Session()

    import_data(session, url, locations_url, roads_url)
