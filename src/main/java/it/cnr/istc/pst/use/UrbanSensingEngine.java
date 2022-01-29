package it.cnr.istc.pst.use;

import java.io.IOException;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse.BodyHandlers;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Stream;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

public class UrbanSensingEngine implements MqttCallbackExtended, IMqttMessageListener {

    private static Logger LOG = Logger.getLogger(App.class.getName());
    private static ScheduledExecutorService EXECUTOR = Executors.newScheduledThreadPool(1);
    private static ObjectMapper OBJECT_MAPPER = new ObjectMapper();
    private MqttClient mqtt_client;
    private final MqttConnectOptions conn_opts = new MqttConnectOptions();
    private final HttpClient http_client = HttpClient.newBuilder().build();
    private final Properties props;
    private final Collection<Rule> rules = new ArrayList<>();
    private ScheduledFuture<?> check_handle;
    final Map<String, JsonNode> state = new HashMap<>();

    public UrbanSensingEngine(final Properties props) {
        this.props = props;
        try {
            mqtt_client = new MqttClient(props.getProperty("MQTT_URI"),
                    "UrbanSensingEngine",
                    new MemoryPersistence());
            conn_opts.setAutomaticReconnect(true);
            conn_opts.setCleanSession(true);
            conn_opts.setUserName(props.getProperty("MQTT_USERNAME"));
            conn_opts.setPassword(props.getProperty("MQTT_PASSWORD").toCharArray());
            mqtt_client.setCallback(this);
        } catch (final MqttException e) {
            LOG.log(Level.SEVERE, "MQTT connection failure..", e);
        }
    }

    public void load_rules() {
        rules.clear();
        LOG.info("Loading rules..");
        try (Stream<Path> paths = Files.walk(Paths.get("rules"))) {
            paths
                    .filter(Files::isRegularFile)
                    .forEach((Path p) -> {
                        LOG.info("Loading rule: " + p);
                        try {
                            rules.add(OBJECT_MAPPER.readValue(p.toFile(), Rule.class));
                        } catch (IOException e) {
                            LOG.log(Level.SEVERE, "Cannot load rule..", e);
                        }
                    });
        } catch (final IOException e) {
            LOG.log(Level.SEVERE, "Cannot load rules..", e);
        }
    }

    public List<String> get_sensors() {
        LOG.info("Retrieving sensors..");
        final var get_request = HttpRequest.newBuilder().GET()
                .uri(URI.create(props.getProperty("REST_URI") + "/list_collections?database=sensors")).build();
        try {
            final var response = http_client.send(get_request, BodyHandlers.ofString());
            if (response.statusCode() == 200)
                return OBJECT_MAPPER.readValue(response.body(), new TypeReference<List<String>>() {
                });
            else
                LOG.warning("Status code: " + response.statusCode() + "\n" + response.body());
        } catch (IOException | InterruptedException e) {
            LOG.log(Level.SEVERE, "Request failure..", e);
        }
        return Collections.emptyList();
    }

    public void connect() {
        try {
            LOG.info("Connecting to the MQTT broker..");
            mqtt_client.connect(conn_opts);
            LOG.info("Urban Sensing Engine connected to the MQTT broker..");
        } catch (final MqttException e) {
            LOG.log(Level.SEVERE, "MQTT connection failure..", e);
        }
    }

    public void disconnect() {
        try {
            LOG.info("Disconnecting from the MQTT broker..");
            mqtt_client.disconnect();
            LOG.info("Urban Sensing Engine disconnected from the MQTT broker..");
        } catch (final MqttException e) {
            LOG.log(Level.SEVERE, "MQTT connection failure..", e);
        }
    }

    @Override
    public void connectionLost(final Throwable cause) {
        LOG.log(Level.SEVERE, "MQTT connection lost..", cause);
        check_handle.cancel(true);
    }

    @Override
    public void messageArrived(final String topic, final MqttMessage message) throws Exception {
        LOG.fine("Message arrived:\n" + topic + "\n" + message.toString());
        if (state.containsKey(topic))
            try {
                state.put(topic, OBJECT_MAPPER.readTree(new String(message.getPayload())));
            } catch (final JsonProcessingException e) {
            }
    }

    @Override
    public void deliveryComplete(final IMqttDeliveryToken token) {
    }

    @Override
    public void connectComplete(final boolean reconnect, final String serverURI) {
        final var sensors = get_sensors();
        LOG.info("Found " + sensors.size() + " sensors..");
        for (final var sensor : sensors) {
            state.put(sensor, null);
            try {
                mqtt_client.subscribe(sensor, this);
            } catch (final MqttException e) {
                LOG.log(Level.SEVERE, "MQTT subscription failure..", e);
            }
        }
        check_handle = EXECUTOR.scheduleAtFixedRate(() -> {
            LOG.fine("check..");
            for (var rule : rules)
                if (rule.verify(this))
                    rule.apply(this);
        }, 1, 1, TimeUnit.SECONDS);
    }

    void printMessage(final String message) {
        LOG.info(message);
    }

    void publishMessage(final String topic, final String message) {
        try {
            mqtt_client.publish(topic, message.getBytes(), 1, false);
        } catch (MqttException e) {
            LOG.log(Level.SEVERE, "MQTT publishing failure..", e);
        }
    }
}
