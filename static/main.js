const monitoring_map = L.map('map');
const monitoring_tiles = L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
    attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
}).addTo(monitoring_map);

let ws;
setup_ws();

function init_map(lat, lng) {
    monitoring_map.setView([lat, lng], 16);
}

var participatory_data_1 = {
    max: 3,
    data: [
        { lat: 37.5078772, lng: 15.0830304, count: 3 },
        { lat: 37.5070772, lng: 15.0930304, count: 2 },
        { lat: 37.5060772, lng: 15.0920304, count: 2 },
        { lat: 37.5050772, lng: 15.0830304, count: 2 },
        { lat: 37.5050772, lng: 15.0920304, count: 2 },
        { lat: 37.5100772, lng: 15.0730304, count: 2 },
        { lat: 37.5200772, lng: 15.0720304, count: 2 }]
};

var participatory_cfg_1 = {
    "radius": 0.001,
    "maxOpacity": .7,
    "scaleRadius": true,
    "useLocalExtrema": false,
    latField: 'lat',
    lngField: 'lng',
    valueField: 'count'
};
const participatory_layer_1 = new HeatmapOverlay(participatory_cfg_1);
participatory_layer_1.setData(participatory_data_1);

var participatory_data_2 = {
    max: 3,
    data: [
        { lat: 37.5078772, lng: 15.0970304, count: 3 },
        { lat: 37.5070772, lng: 15.0770304, count: 2 },
        { lat: 37.5060772, lng: 15.0820324, count: 2 },
        { lat: 37.5050772, lng: 15.0735324, count: 2 },
        { lat: 37.5050772, lng: 15.0925304, count: 2 },
        { lat: 37.5100772, lng: 15.0835334, count: 2 },
        { lat: 37.5200772, lng: 15.0825334, count: 2 }]
};

var participatory_cfg_2 = {
    "radius": 0.001,
    "maxOpacity": .7,
    "scaleRadius": true,
    "useLocalExtrema": false,
    latField: 'lat',
    lngField: 'lng',
    valueField: 'count'
};
const participatory_layer_2 = new HeatmapOverlay(participatory_cfg_2);
participatory_layer_2.setData(participatory_data_2);

document.getElementById('show_participatory_data_1').addEventListener('change', (event) => {
    if (event.currentTarget.checked) {
        participatory_layer_1.addTo(monitoring_map);
    } else {
        participatory_layer_1.removeFrom(monitoring_map);
    }
});

document.getElementById('show_participatory_data_2').addEventListener('change', (event) => {
    if (event.currentTarget.checked) {
        participatory_layer_2.addTo(monitoring_map);
    } else {
        participatory_layer_2.removeFrom(monitoring_map);
    }
});

document.getElementById('CO2Production').addEventListener('change', (event) => {
    if (event.currentTarget.checked)
        document.getElementById('buildings_img').src = 'static/figures/BS_co2.jpg';
});

document.getElementById('EnergyConsumption').addEventListener('change', (event) => {
    if (event.currentTarget.checked)
        document.getElementById('buildings_img').src = 'static/figures/BS_primary_energy.jpg';
});

function show_message(lat, lng, message) {
    const popup = L.popup().setLatLng([lat, lng]).setContent(message).openOn(monitoring_map);
}

function setup_ws() {
    ws = new WebSocket('ws://' + location.hostname + ':' + location.port + '/use');
    ws.onmessage = msg => {
        const c_msg = JSON.parse(msg.data);
        switch (c_msg.type) {
            case 'warning':
                show_message(c_msg.location.lat, c_msg.location.lng, c_msg.content)
                break;
            default:
                console.log(c_msg);
        }
    }
    ws.onclose = () => setTimeout(setup_ws, 1000);
}