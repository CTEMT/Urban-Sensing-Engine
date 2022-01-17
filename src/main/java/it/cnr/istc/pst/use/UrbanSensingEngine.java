package it.cnr.istc.pst.use;

import java.io.IOException;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse.BodyHandlers;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

public class UrbanSensingEngine implements MqttCallback, IMqttMessageListener {

    private static Logger LOG = Logger.getLogger(App.class.getName());
    private static ObjectMapper OBJECT_MAPPER = new ObjectMapper();
    private MqttClient mqtt_client;
    private MqttConnectOptions conn_opts = new MqttConnectOptions();
    private final HttpClient http_client = HttpClient.newBuilder().build();
    private final Properties props;
    private final Map<String, JsonNode> state = new HashMap<>();

    public UrbanSensingEngine(Properties props) {
        this.props = props;
        try {
            mqtt_client = new MqttClient(props.getProperty("MQTT_URI"),
                    "UrbanSensingEngine",
                    new MemoryPersistence());
            conn_opts.setCleanSession(false);
            conn_opts.setUserName(props.getProperty("MQTT_USERNAME"));
            conn_opts.setPassword(props.getProperty("MQTT_PASSWORD").toCharArray());
            mqtt_client.setCallback(this);
        } catch (MqttException e) {
            LOG.log(Level.SEVERE, "MQTT connection failure..", e);
        }
    }

    public List<String> get_sensors() {
        LOG.info("Retrieving sensors..");
        var get_request = HttpRequest.newBuilder().GET()
                .uri(URI.create(props.getProperty("REST_URI") + "/list_collections?database=sensors")).build();
        try {
            var response = http_client.send(get_request, BodyHandlers.ofString());
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

            var sensors = get_sensors();
            LOG.info("Found " + sensors.size() + " sensors..");
            for (var sensor : sensors) {
                state.put(sensor, null);
                mqtt_client.subscribe(sensor, this);
            }
        } catch (MqttException e) {
            LOG.log(Level.SEVERE, "MQTT connection failure..", e);
        }
    }

    public void disconnect() {
        try {
            LOG.info("Disconnecting from the MQTT broker..");
            mqtt_client.disconnect();
            LOG.info("Urban Sensing Engine disconnected from the MQTT broker..");
        } catch (MqttException e) {
            LOG.log(Level.SEVERE, "MQTT connection failure..", e);
        }
    }

    @Override
    public void connectionLost(Throwable cause) {
        LOG.log(Level.SEVERE, "MQTT connection lost..", cause);
        connect();
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        LOG.fine("Message arrived:\n" + topic + "\n" + message.toString());
        if (state.containsKey(topic))
            try {
                state.put(topic, OBJECT_MAPPER.readTree(new String(message.getPayload())));
            } catch (JsonProcessingException e) {
            }
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
    }
}
