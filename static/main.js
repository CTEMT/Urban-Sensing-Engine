const monitoring_map = L.map('map');
const monitoring_tiles = L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
    attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
}).addTo(monitoring_map);

const buses = new Map();

const toast_container = document.getElementById('toast-container');

const solvers = new Map();

let ws;
setup_ws();

function init_map(lat, lng) {
    monitoring_map.setView([lat, lng], 16);
}

var participatory_data_1 = [
    { lat: 37.5078772, lng: 15.0830304, count: 3 },
    { lat: 37.5070772, lng: 15.0930304, count: 2 },
    { lat: 37.5060772, lng: 15.0920304, count: 2 },
    { lat: 37.5050772, lng: 15.0830304, count: 2 },
    { lat: 37.5050772, lng: 15.0920304, count: 2 },
    { lat: 37.5100772, lng: 15.0730304, count: 2 },
    { lat: 37.5200772, lng: 15.0720304, count: 2 },
    { lat: 37.5190772, lng: 15.0790304, count: 2 },
    { lat: 37.5290772, lng: 15.0770304, count: 2 }];

var participatory_data_2 = [
    { lat: 37.5078772, lng: 15.0970304, count: 3 },
    { lat: 37.5070772, lng: 15.0770304, count: 2 },
    { lat: 37.5060772, lng: 15.0820324, count: 2 },
    { lat: 37.5050772, lng: 15.0735324, count: 2 },
    { lat: 37.5050772, lng: 15.0925304, count: 2 },
    { lat: 37.5100772, lng: 15.0835334, count: 2 },
    { lat: 37.5200772, lng: 15.0825334, count: 2 }];

var participatory_cfg = {
    'radius': 0.001,
    'maxOpacity': .7,
    'scaleRadius': true,
    'useLocalExtrema': false,
    latField: 'lat',
    lngField: 'lng',
    valueField: 'count'
};
const participatory_layer = new HeatmapOverlay(participatory_cfg);
participatory_layer.addTo(monitoring_map);

const participatory_data_change = (event) => {
    const spa_1 = document.getElementById('show_participatory_data_1');
    const spa_2 = document.getElementById('show_participatory_data_2');
    const c_data = [];
    if (spa_1.checked) c_data.push(...participatory_data_1);
    if (spa_2.checked) c_data.push(...participatory_data_2);
    participatory_layer.setData({ max: 3, data: c_data });
};

document.getElementById('show_participatory_data_1').addEventListener('change', participatory_data_change);
document.getElementById('show_participatory_data_2').addEventListener('change', participatory_data_change);

document.getElementById('CO2Production').addEventListener('change', (event) => {
    if (event.currentTarget.checked)
        document.getElementById('buildings_img').src = 'static/figures/BS_co2.jpg';
});

document.getElementById('EnergyConsumption').addEventListener('change', (event) => {
    if (event.currentTarget.checked)
        document.getElementById('buildings_img').src = 'static/figures/BS_primary_energy.jpg';
});

function show_message(message) {
    const html = `<div class="toast" role="alert" aria-live="assertive" aria-atomic="true">
                      <div class="toast-header">
                          <strong class="me-auto">${document.title}</strong>
                          <button type="button" class="btn-close" data-bs-dismiss="toast" aria-label="Close"></button>
                      </div>
                      <div class="toast-body">${message}</div>
                  </div>`.trim();
    const template = document.createElement('template');
    template.innerHTML = html;
    const toast = template.content.firstChild;
    toast_container.appendChild(toast);
    new bootstrap.Toast(toast).show();
}

function show_map_message(lat, lng, message) {
    const popup = L.popup({ autoClose: false }).setLatLng([lat, lng]).setContent(message).openOn(monitoring_map);
}

function show_bus_message(bus, lat, lng, message) {
    if (buses.has(bus)) {
        const bus_marker = buses.get(bus);
        bus_marker.setLine([[lat, lng]]);
        bus_marker.bindTooltip(message);
        bus_marker.start();
    } else {
        const bus_icon = L.icon({ iconUrl: 'static/figures/bus.png' });
        bus_icon.options.iconSize = [32, 32];
        const bus_marker = L.animatedMarker([[lat, lng]], { icon: bus_icon, autoStart: false });
        bus_marker.bindTooltip(message);
        bus_marker.addTo(monitoring_map);
        buses.set(bus, bus_marker);
    }
}

function set_solvers(current_solvers) {
    const c_solvers = new Set();
    const solvers_pills = document.getElementById('pills-solvers');
    const solvers_content = document.getElementById('solvers-pills-tabContent');
    for (const slv of current_solvers) {
        c_solvers.add(slv);
        if (!solvers.has(slv)) {
            const timelines_pill_html = `
            <li class="nav-item" id="timelines-${slv}-li" role="presentation">
                <button class="nav-link active" id="timelines-${slv}-tab" data-bs-toggle="pill" data-bs-target="#slv-${slv}-timelines" role="tab" aria-controls="slv-${slv}-timelines" aria-selected="false"><i class="bi bi-bar-chart-steps"></i> Timelines</button>
            </li>`.trim();
            const timelines_pill_template = document.createElement('template');
            timelines_pill_template.innerHTML = timelines_pill_html;
            const timelines_pill = timelines_pill_template.content.firstChild;
            solvers_pills.appendChild(timelines_pill);

            const timelines_content_html = `
            <div class="tab-pane fade h-100 show active" id="slv-${slv}-timelines" role="tabpanel" aria-labelledby="timelines-${slv}-tab">
                <div class="h-100" id="timelines-${slv}"></div>
            </div>`.trim();
            const timelines_content_template = document.createElement('template');
            timelines_content_template.innerHTML = timelines_content_html;
            const timelines_content = timelines_content_template.content.firstChild;
            solvers_content.appendChild(timelines_content);

            const graph_pill_html = `
            <li class="nav-item" id="graph-${slv}-li" role="presentation">
                <button class="nav-link" id="graph-${slv}-tab" data-bs-toggle="pill" data-bs-target="#slv-${slv}-graph" role="tab" aria-controls="slv-${slv}-graph" aria-selected="false"><i class="bi bi-diagram-3"></i> Graph</button>
            </li>`.trim();
            const graph_pill_template = document.createElement('template');
            graph_pill_template.innerHTML = graph_pill_html;
            const graph_pill = graph_pill_template.content.firstChild;
            solvers_pills.appendChild(graph_pill);

            const graph_content_html = `
            <div class="tab-pane fade h-100" id="slv-${slv}-graph" role="tabpanel" aria-labelledby="graph-${slv}-tab">
                <div class="h-100" id="graph-${slv}"></div>
            </div>`.trim();
            const graph_content_template = document.createElement('template');
            graph_content_template.innerHTML = graph_content_html;
            const graph_content = graph_content_template.content.firstChild;
            solvers_content.appendChild(graph_content);

            solvers.set(slv, new ReasonerD3(`timelines-${slv}`, `graph-${slv}`));
        }
    }
    for (const [slv_id, r] of solvers)
        if (!c_solvers.has(slv_id)) {
            document.getElementById(`timelines-${slv_id}-li`).remove();
            document.getElementById(`${slv_id}-timelines`).remove();
            document.getElementById(`graph-${slv_id}-li`).remove();
            document.getElementById(`${slv_id}-graph`).remove();
        }
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

const flow_month_options = {
    chart: {
        type: 'heatmap'
    },
    title: {
        text: 'Distribuzione del traffico per POI e mese'
    },
    colors: ["#008FFB"],
    series: [{
        name: 'Piazza Vittorio Veneto',
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
        name: "Parco Villa Unità d'Italia",
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
        name: 'Via Lucana',
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
        name: 'Via XX Settembre',
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
        name: 'Chiesa di Santa Maria de Armenis',
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
        name: 'Chiesa di San Biagio',
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
    }]
};
const flow_month_chart = new ApexCharts(document.querySelector('#flow_month'), flow_month_options);
flow_month_chart.render();

const flow_day_options = {
    chart: {
        type: 'heatmap'
    },
    title: {
        text: 'Distribuzione del traffico per POI e giorno della settimana'
    },
    colors: ["#008FFB"],
    series: [{
        name: 'Piazza Vittorio Veneto',
        data: [{
            x: 'Lun',
            y: 2
        }, {
            x: 'Mar',
            y: 3
        }, {
            x: 'Mer',
            y: 5
        }, {
            x: 'Gio',
            y: 3
        }, {
            x: 'Ven',
            y: 5
        }, {
            x: 'Sab',
            y: 4
        }, {
            x: 'Dom',
            y: 5
        }]
    },
    {
        name: "Parco Villa Unità d'Italia",
        data: [{
            x: 'Lun',
            y: 15
        }, {
            x: 'Mar',
            y: 18
        }, {
            x: 'Mer',
            y: 13
        }, {
            x: 'Gio',
            y: 13
        }, {
            x: 'Ven',
            y: 13
        }, {
            x: 'Sab',
            y: 13
        }, {
            x: 'Dom',
            y: 13
        }]
    },
    {
        name: 'Via Lucana',
        data: [{
            x: 'Lun',
            y: 5
        }, {
            x: 'Mar',
            y: 6
        }, {
            x: 'Mer',
            y: 5
        }, {
            x: 'Gio',
            y: 7
        }, {
            x: 'Ven',
            y: 6
        }, {
            x: 'Sab',
            y: 7
        }, {
            x: 'Dom',
            y: 8
        }]
    },
    {
        name: 'Via XX Settembre',
        data: [{
            x: 'Lun',
            y: 2
        }, {
            x: 'Mar',
            y: 3
        }, {
            x: 'Mer',
            y: 2
        }, {
            x: 'Gio',
            y: 4
        }, {
            x: 'Ven',
            y: 6
        }, {
            x: 'Sab',
            y: 4
        }, {
            x: 'Dom',
            y: 5
        }]
    },
    {
        name: 'Chiesa di Santa Maria de Armenis',
        data: [{
            x: 'Lun',
            y: 3
        }, {
            x: 'Mar',
            y: 4
        }, {
            x: 'Mer',
            y: 3
        }, {
            x: 'Gio',
            y: 4
        }, {
            x: 'Ven',
            y: 6
        }, {
            x: 'Sab',
            y: 4
        }, {
            x: 'Dom',
            y: 3
        }]
    },
    {
        name: 'Chiesa di San Biagio',
        data: [{
            x: 'Lun',
            y: 13
        }, {
            x: 'Mar',
            y: 14
        }, {
            x: 'Mer',
            y: 13
        }, {
            x: 'Gio',
            y: 14
        }, {
            x: 'Ven',
            y: 16
        }, {
            x: 'Sab',
            y: 14
        }, {
            x: 'Dom',
            y: 13
        }]
    }]
};
const flow_day_chart = new ApexCharts(document.querySelector('#flow_day'), flow_day_options);
flow_day_chart.render();

function setup_ws() {
    ws = new WebSocket('ws://' + location.hostname + ':' + location.port + '/use');
    ws.onmessage = msg => {
        const c_msg = JSON.parse(msg.data);
        switch (c_msg.type) {
            case 'message':
                show_message(c_msg.content);
                break;
            case 'map_message':
                show_map_message(c_msg.location.lat, c_msg.location.lng, c_msg.content)
                break;
            case 'bus_message':
                show_bus_message(c_msg.bus, c_msg.location.lat, c_msg.location.lng, c_msg.content)
                break;
            case 'solvers':
                set_solvers(c_msg.solvers);
                break;
            case 'solution_found':
                solvers.get(c_msg.id).state_changed(c_msg);
                break;
            case 'graph':
                solvers.get(c_msg.id).graph(c_msg);
                break;
            case 'tick':
                solvers.get(c_msg.id).tick(c_msg);
                break;
            case 'start':
                solvers.get(c_msg.id).start(c_msg);
                break;
            case 'end':
                solvers.get(c_msg.id).end(c_msg);
                break;
            default:
                console.log(c_msg);
        }
    }
    ws.onclose = () => setTimeout(setup_ws, 1000);
}