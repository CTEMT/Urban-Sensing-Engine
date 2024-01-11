import csv
import json

tiles = []

with open('tiles_d.csv', newline='') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        tiles.append({ 'lat': row['Lat'], 'lon': row['Lon'] })

with open('tiles.json', 'w') as outfile:
    json.dump(tiles, outfile)