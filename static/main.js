const map = L.map('map');

function init_map(latitude, longitude) {
    map.setView([latitude, longitude], 16);
    const tiles = L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
        maxZoom: 19,
        attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
    }).addTo(map);
}

function show_message(latitude, longitude, message) {
    const popup = L.popup().setLatLng([latitude, longitude]).setContent(message).openOn(map);
}