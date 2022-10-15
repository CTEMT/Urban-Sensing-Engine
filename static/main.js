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
    'radius': 0.001,
    'maxOpacity': .7,
    'scaleRadius': true,
    'useLocalExtrema': false,
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
    'radius': 0.001,
    'maxOpacity': .7,
    'scaleRadius': true,
    'useLocalExtrema': false,
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

const flow_type_options = {
    chart: {
        type: 'bar',
        stacked: true,
        stackType: '100%'
    },
    title: {
        text: 'Distribuzione del tipo di flusso sui POI'
    },
    plotOptions: {
        bar: {
            horizontal: true
        }
    },
    series: [{
        name: 'Pedonale',
        data: [{
            x: 'Piazza Vittorio Veneto',
            y: 15
        }, {
            x: "Parco Villa Unità d'Italia",
            y: 18
        }, {
            x: 'Via Lucana',
            y: 13
        }, {
            x: 'Via XX Settembre',
            y: 13
        }, {
            x: 'Chiesa di Santa Maria de Armenis',
            y: 13
        }, {
            x: 'Chiesa di San Biagio',
            y: 13
        }]
    }, {
        name: 'Viecolare',
        data: [{
            x: 'Piazza Vittorio Veneto',
            y: 5
        }, {
            x: "Parco Villa Unità d'Italia",
            y: 6
        }, {
            x: 'Via Lucana',
            y: 5
        }, {
            x: 'Via XX Settembre',
            y: 7
        }, {
            x: 'Chiesa di Santa Maria de Armenis',
            y: 6
        }, {
            x: 'Chiesa di San Biagio',
            y: 5
        }]
    }, {
        name: 'Altro',
        data: [{
            x: 'Piazza Vittorio Veneto',
            y: 2
        }, {
            x: "Parco Villa Unità d'Italia",
            y: 2
        }, {
            x: 'Via Lucana',
            y: 3
        }, {
            x: 'Via XX Settembre',
            y: 2
        }, {
            x: 'Chiesa di Santa Maria de Armenis',
            y: 1
        }, {
            x: 'Chiesa di San Biagio',
            y: 2
        }]
    }]
};
const flow_type_chart = new ApexCharts(document.querySelector('#flow_type'), flow_type_options);
flow_type_chart.render();

const flow_type_month_options = {
    chart: {
        type: 'bar',
        stacked: true,
        stackType: '100%'
    },
    title: {
        text: 'Distribuzione del  flusso per tipo di veicolo e mese'
    },
    plotOptions: {
        bar: {
            horizontal: true
        }
    },
    series: [{
        name: 'Autoarticolato',
        data: [{
            x: 'Gen',
            y: 2
        }, {
            x: 'Feb',
            y: 3
        }, {
            x: 'Mar',
            y: 5
        }, {
            x: 'Apr',
            y: 3
        }, {
            x: 'Mag',
            y: 5
        }, {
            x: 'Giu',
            y: 4
        }, {
            x: 'Lug',
            y: 5
        }, {
            x: 'Ago',
            y: 3
        }, {
            x: 'Set',
            y: 4
        }, {
            x: 'Ott',
            y: 5
        }, {
            x: 'Nov',
            y: 2
        }, {
            x: 'Dic',
            y: 3
        }]
    },
    {
        name: 'Autovettura',
        data: [{
            x: 'Gen',
            y: 15
        }, {
            x: 'Feb',
            y: 18
        }, {
            x: 'Mar',
            y: 13
        }, {
            x: 'Apr',
            y: 13
        }, {
            x: 'Mag',
            y: 13
        }, {
            x: 'Giu',
            y: 13
        }, {
            x: 'Lug',
            y: 13
        }, {
            x: 'Ago',
            y: 13
        }, {
            x: 'Set',
            y: 13
        }, {
            x: 'Ott',
            y: 13
        }, {
            x: 'Nov',
            y: 13
        }, {
            x: 'Dic',
            y: 13
        }]
    },
    {
        name: 'Autovettura con rimorchio',
        data: [{
            x: 'Gen',
            y: 5
        }, {
            x: 'Feb',
            y: 6
        }, {
            x: 'Mar',
            y: 5
        }, {
            x: 'Apr',
            y: 7
        }, {
            x: 'Mag',
            y: 6
        }, {
            x: 'Giu',
            y: 7
        }, {
            x: 'Lug',
            y: 8
        }, {
            x: 'Ago',
            y: 8
        }, {
            x: 'Set',
            y: 7
        }, {
            x: 'Ott',
            y: 6
        }, {
            x: 'Nov',
            y: 5
        }, {
            x: 'Dic',
            y: 8
        }]
    },
    {
        name: 'Bus',
        data: [{
            x: 'Gen',
            y: 2
        }, {
            x: 'Feb',
            y: 3
        }, {
            x: 'Mar',
            y: 2
        }, {
            x: 'Apr',
            y: 4
        }, {
            x: 'Mag',
            y: 6
        }, {
            x: 'Giu',
            y: 4
        }, {
            x: 'Lug',
            y: 5
        }, {
            x: 'Ago',
            y: 6
        }, {
            x: 'Set',
            y: 5
        }, {
            x: 'Ott',
            y: 2
        }, {
            x: 'Nov',
            y: 3
        }, {
            x: 'Dic',
            y: 4
        }]
    },
    {
        name: 'Camion',
        data: [{
            x: 'Gen',
            y: 3
        }, {
            x: 'Feb',
            y: 4
        }, {
            x: 'Mar',
            y: 3
        }, {
            x: 'Apr',
            y: 4
        }, {
            x: 'Mag',
            y: 6
        }, {
            x: 'Giu',
            y: 4
        }, {
            x: 'Lug',
            y: 3
        }, {
            x: 'Ago',
            y: 2
        }, {
            x: 'Set',
            y: 3
        }, {
            x: 'Ott',
            y: 2
        }, {
            x: 'Nov',
            y: 3
        }, {
            x: 'Dic',
            y: 2
        }]
    },
    {
        name: 'Motocicletta',
        data: [{
            x: 'Gen',
            y: 13
        }, {
            x: 'Feb',
            y: 14
        }, {
            x: 'Mar',
            y: 13
        }, {
            x: 'Apr',
            y: 14
        }, {
            x: 'Mag',
            y: 16
        }, {
            x: 'Giu',
            y: 14
        }, {
            x: 'Lug',
            y: 13
        }, {
            x: 'Ago',
            y: 12
        }, {
            x: 'Set',
            y: 13
        }, {
            x: 'Ott',
            y: 12
        }, {
            x: 'Nov',
            y: 13
        }, {
            x: 'Dic',
            y: 12
        }]
    },
    {
        name: 'Van',
        data: [{
            x: 'Gen',
            y: 3
        }, {
            x: 'Feb',
            y: 6
        }, {
            x: 'Mar',
            y: 4
        }, {
            x: 'Apr',
            y: 6
        }, {
            x: 'Mag',
            y: 7
        }, {
            x: 'Giu',
            y: 8
        }, {
            x: 'Lug',
            y: 6
        }, {
            x: 'Ago',
            y: 7
        }, {
            x: 'Set',
            y: 8
        }, {
            x: 'Ott',
            y: 2
        }, {
            x: 'Nov',
            y: 3
        }, {
            x: 'Dic',
            y: 8
        }]
    },
    {
        name: 'Altro',
        data: [{
            x: 'Gen',
            y: 3
        }, {
            x: 'Feb',
            y: 4
        }, {
            x: 'Mar',
            y: 3
        }, {
            x: 'Apr',
            y: 4
        }, {
            x: 'Mag',
            y: 6
        }, {
            x: 'Giu',
            y: 4
        }, {
            x: 'Lug',
            y: 3
        }, {
            x: 'Ago',
            y: 2
        }, {
            x: 'Set',
            y: 3
        }, {
            x: 'Ott',
            y: 2
        }, {
            x: 'Nov',
            y: 3
        }, {
            x: 'Dic',
            y: 2
        }]
    }]
};
const flow_type_month_chart = new ApexCharts(document.querySelector('#flow_type_month'), flow_type_month_options);
flow_type_month_chart.render();

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