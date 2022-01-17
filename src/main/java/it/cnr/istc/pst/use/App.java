package it.cnr.istc.pst.use;

import java.io.IOException;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Urban-Sensing-Enging
 *
 */
public class App {

    private static Logger LOG = Logger.getLogger(App.class.getName());
    private static Properties PROPS = new Properties();
    private static UrbanSensingEngine USE;

    public static void main(final String[] args) {
        LOG.info("Starting Urban Sensing Engine..");
        try {
            PROPS.load(App.class.getResourceAsStream("/config.properties"));
        } catch (final IOException e) {
            LOG.log(Level.SEVERE, "Cannot read config properties file..", e);
        }
        USE = new UrbanSensingEngine(PROPS);
        USE.load_rules();
        USE.connect();

        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            USE.disconnect();
        }));
    }
}
