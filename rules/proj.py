from pyproj import Proj, transform

# Define the original projection (UTM)
original_projection = Proj(init='epsg:your_epsg_code')  # Replace 'your_epsg_code' with the appropriate EPSG code for your UTM zone.

# Convert the coordinates to latitude and longitude
lon, lat = transform(original_projection, Proj(init='epsg:4326'), 634320.81153, 4503914.07979)

print(f"Latitude: {lat}, Longitude: {lon}")
