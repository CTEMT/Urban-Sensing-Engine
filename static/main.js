const monitoring_map = L.map('map');
const monitoring_tiles = L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
    attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
}).addTo(monitoring_map);

monitoring_map.on('click', (e) => L.popup().setLatLng(e.latlng).setContent("You clicked the map at " + e.latlng.toString()).openOn(monitoring_map));

const buses = new Map();

const toast_container = document.getElementById('toast-container');

const solvers = new Map();

let ws;
setup_ws();

function init_map(lat, lng) {
    monitoring_map.setView([lat, lng], 16);
}

const degraded_road_surface = [
    { lat: 37.504054, lng: 15.095713, count: 2 },
    { lat: 37.503949, lng: 15.095316, count: 3 },
    { lat: 37.504821, lng: 15.081968, count: 3 },
    { lat: 37.507453, lng: 15.087594, count: 2 },
    { lat: 37.507495, lng: 15.088255, count: 1 },
    { lat: 37.507579, lng: 15.088758, count: 2 },
    { lat: 37.507650, lng: 15.089625, count: 1 }];

const architecutral_barriers = [
    { lat: 37.509425, lng: 15.085690, count: 2 },
    { lat: 37.509257, lng: 15.085796, count: 3 },
    { lat: 37.509048, lng: 15.085928, count: 2 },
    { lat: 37.503908, lng: 15.088758, count: 3 },
    { lat: 37.502279, lng: 15.083475, count: 2 },
    { lat: 37.503683, lng: 15.089063, count: 2 }];

const damaged_street_forniture = [
    { lat: 37.511670, lng: 15.083204, count: 2 },
    { lat: 37.510957, lng: 15.082781, count: 2 },
    { lat: 37.507883, lng: 15.091744, count: 2 },
    { lat: 37.507600, lng: 15.090551, count: 2 }];

const identity_and_memory_places = [
    { lat: 37.507407, lng: 15.085563, count: 2 },
    { lat: 37.504544, lng: 15.081036, count: 2 },
    { lat: 37.504328, lng: 15.080604, count: 2 },
    { lat: 37.502944, lng: 15.082703, count: 3 },
    { lat: 37.502865, lng: 15.083737, count: 3 },
    { lat: 37.504321, lng: 15.085058, count: 2 },
    { lat: 37.503996, lng: 15.084837, count: 2 },
    { lat: 37.503554, lng: 15.084768, count: 3 },
    { lat: 37.503768, lng: 15.087284, count: 3 },
    { lat: 37.504358, lng: 15.090551, count: 3 },
    { lat: 37.502218, lng: 15.086984, count: 3 },
    { lat: 37.502431, lng: 15.087046, count: 2 },
    { lat: 37.502406, lng: 15.087200, count: 3 },
    { lat: 37.501689, lng: 15.087595, count: 3 },
    { lat: 37.499274, lng: 15.084127, count: 3 },
    { lat: 37.498913, lng: 15.084907, count: 3 },
    { lat: 37.500915, lng: 15.093043, count: 2 },
    { lat: 37.501022, lng: 15.093701, count: 2 }];

const cultural_places = [
    { lat: 37.504215, lng: 15.090217, count: 2 },
    { lat: 37.504229, lng: 15.090011, count: 2 },
    { lat: 37.504122, lng: 15.090138, count: 3 },
    { lat: 37.503564, lng: 15.086726, count: 2 },
    { lat: 37.502079, lng: 15.087664, count: 2 },
    { lat: 37.503340, lng: 15.086606, count: 2 },
    { lat: 37.503571, lng: 15.079660, count: 2 },
    { lat: 37.503836, lng: 15.080036, count: 1 },
    { lat: 37.503736, lng: 15.079160, count: 2 },
    { lat: 37.503637, lng: 15.092923, count: 2 },
    { lat: 37.502943, lng: 15.090241, count: 3 },
    { lat: 37.502803, lng: 15.090064, count: 1 },
    { lat: 37.503773, lng: 15.087568, count: 3 },
    { lat: 37.502585, lng: 15.084373, count: 2 }];

const places_of_focialization = [
    { lat: 37.510373, lng: 15.083056, count: 2 },
    { lat: 37.510305, lng: 15.083437, count: 1 },
    { lat: 37.510190, lng: 15.082992, count: 3 },
    { lat: 37.509843, lng: 15.082937, count: 2 },
    { lat: 37.507360, lng: 15.085950, count: 1 },
    { lat: 37.507404, lng: 15.086731, count: 3 },
    { lat: 37.505917, lng: 15.086793, count: 3 },
    { lat: 37.504973, lng: 15.086856, count: 2 },
    { lat: 37.504877, lng: 15.086668, count: 1 },
    { lat: 37.504353, lng: 15.086820, count: 2 },
    { lat: 37.503697, lng: 15.086983, count: 2 },
    { lat: 37.503661, lng: 15.087269, count: 3 },
    { lat: 37.503487, lng: 15.087059, count: 3 },
    { lat: 37.502595, lng: 15.087596, count: 2 },
    { lat: 37.502659, lng: 15.087891, count: 2 },
    { lat: 37.501605, lng: 15.088173, count: 2 },
    { lat: 37.501241, lng: 15.087901, count: 2 },
    { lat: 37.504229, lng: 15.090334, count: 3 },
    { lat: 37.504460, lng: 15.090752, count: 2 }];

const identity_core = [
    [37.50756, 15.085387],
    [37.506555, 15.085671],
    [37.506444, 15.084873],
    [37.503279, 15.084807],
    [37.503043, 15.082521],
    [37.502248, 15.082534],
    [37.502519, 15.084768],
    [37.50182, 15.084973],
    [37.50149, 15.083512],
    [37.499736, 15.083913],
    [37.498714, 15.083742],
    [37.498284, 15.084231],
    [37.498413, 15.085146],
    [37.499239, 15.085194],
    [37.499618, 15.084748],
    [37.500868, 15.084781],
    [37.50113, 15.085179],
    [37.501394, 15.085059],
    [37.501512, 15.085458],
    [37.501819, 15.086439],
    [37.501591, 15.086585],
    [37.50188, 15.087069],
    [37.502236, 15.086976],
    [37.50235, 15.087504],
    [37.503407, 15.087236],
    [37.503457, 15.087532],
    [37.503902, 15.087426],
    [37.50383, 15.086812],
    [37.503394, 15.086931]
];

const cultural_core = [
    [37.503037, 15.080876],
    [37.502097, 15.08104],
    [37.502733, 15.087351],
    [37.503408, 15.087187],
    [37.503388, 15.08706],
    [37.503749, 15.086956],
    [37.503799, 15.087353],
    [37.503426, 15.087457],
    [37.503389, 15.08732],
    [37.502798, 15.08747],
    [37.503213, 15.09086],
    [37.504361, 15.0907]
];

const socialization_core = [
    [37.50979, 15.082891],
    [37.50911, 15.082763],
    [37.5079, 15.083075],
    [37.50736, 15.084144],
    [37.5076, 15.086235],
    [37.50171, 15.087721],
    [37.50114, 15.087672],
    [37.50096, 15.088088],
    [37.50139, 15.08842],
    [37.50169, 15.088327]
];

const participatory_cfg = {
    'radius': 0.001,
    'maxOpacity': .7,
    'scaleRadius': true,
    'useLocalExtrema': false,
    latField: 'lat',
    lngField: 'lng',
    valueField: 'count'
};
const heatmap_layer = new HeatmapOverlay(participatory_cfg);
heatmap_layer.addTo(monitoring_map);

const heatmap_data_change = (event) => {
    const spa_1 = document.getElementById('show_degraded_road_surface');
    const spa_2 = document.getElementById('show_architecutral_barriers');
    const spa_3 = document.getElementById('show_damaged_street_forniture');
    const spa_4 = document.getElementById('show_identity_and_memory_places');
    const spa_5 = document.getElementById('show_cultural_places');
    const spa_6 = document.getElementById('show_places_of_focialization');
    const c_data = [];
    if (spa_1.checked) c_data.push(...degraded_road_surface);
    if (spa_2.checked) c_data.push(...architecutral_barriers);
    if (spa_3.checked) c_data.push(...damaged_street_forniture);
    if (spa_4.checked) c_data.push(...identity_and_memory_places);
    if (spa_5.checked) c_data.push(...cultural_places);
    if (spa_6.checked) c_data.push(...places_of_focialization);
    heatmap_layer.setData({ max: 3, data: c_data });
};

var identity_core_polyline = L.polyline(identity_core, { color: 'red' });
var cultural_core_polyline = L.polyline(cultural_core, { color: 'green' });
var socialization_core_polyline = L.polyline(socialization_core, { color: 'blue' });

document.getElementById('show_degraded_road_surface').addEventListener('change', heatmap_data_change);
document.getElementById('show_architecutral_barriers').addEventListener('change', heatmap_data_change);
document.getElementById('show_damaged_street_forniture').addEventListener('change', heatmap_data_change);
document.getElementById('show_identity_and_memory_places').addEventListener('change', heatmap_data_change);
document.getElementById('show_cultural_places').addEventListener('change', heatmap_data_change);
document.getElementById('show_places_of_focialization').addEventListener('change', heatmap_data_change);

document.getElementById('show_identity_core').addEventListener('change', (event) => {
    if (event.currentTarget.checked)
        monitoring_map.addLayer(identity_core_polyline);
    else
        monitoring_map.removeLayer(identity_core_polyline);
});

document.getElementById('show_cultural_core').addEventListener('change', (event) => {
    if (event.currentTarget.checked)
        monitoring_map.addLayer(cultural_core_polyline);
    else
        monitoring_map.removeLayer(cultural_core_polyline);
});

document.getElementById('show_socialization_core').addEventListener('change', (event) => {
    if (event.currentTarget.checked)
        monitoring_map.addLayer(socialization_core_polyline);
    else
        monitoring_map.removeLayer(socialization_core_polyline);
});

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