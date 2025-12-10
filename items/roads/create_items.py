import json
from pathlib import Path
import requests

session = requests.Session()
session.headers.update({'Connection': 'keep-alive'})

nodes = {}

path = Path('nodes.json')
with path.open('r', encoding='utf-8') as f:
    data = json.load(f)
    for feature in data[0]['features']:
        node = {'types': ['Location'],
                'properties': {'lat': feature['geometry']['coordinates'][1],
                               'lng': feature['geometry']['coordinates'][0]}}
        response = session.post('http://localhost:8080/items', json=node)
        response.raise_for_status()
        nodes[feature['properties']['osmid']] = response.text

path = Path('edges.json')
with path.open('r', encoding='utf-8') as f:
    data = json.load(f)
    for feature in data[0]['features']:
        road = {'types': ['RoadSegment'],
                'properties': {'from': nodes[feature['properties']['u']],
                               'to': nodes[feature['properties']['v']],
                               'name': feature['properties']['name'],
                               'length': feature['properties']['length']}}
        response = session.post('http://localhost:8080/items', json=road)
        response.raise_for_status()

session.close()