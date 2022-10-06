const map = L.map('map');
const tiles = L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
    attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
}).addTo(map);

let ws;
setup_ws();

function init_map(lat, lng) {
    map.setView([lat, lng], 16);
}

function show_message(lat, lng, message) {
    const popup = L.popup().setLatLng([lat, lng]).setContent(message).openOn(map);
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