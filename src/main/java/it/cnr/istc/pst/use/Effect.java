package it.cnr.istc.pst.use;

import java.util.Collection;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSubTypes;
import com.fasterxml.jackson.annotation.JsonSubTypes.Type;
import com.fasterxml.jackson.annotation.JsonTypeInfo;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.ObjectNode;

@JsonTypeInfo(use = JsonTypeInfo.Id.NAME, include = JsonTypeInfo.As.PROPERTY, property = "type")
@JsonSubTypes({ @Type(value = Effect.AndEffect.class, name = "and"),
        @Type(value = Effect.TextEffect.class, name = "text"),
        @Type(value = Effect.MQTTEffect.class, name = "mqtt"),
        @Type(value = Effect.IncreaseEffect.class, name = "increase"),
        @Type(value = Effect.AverageEffect.class, name = "average") })
public abstract class Effect {

    public abstract void apply(final UrbanSensingEngine use);

    public static class AndEffect extends Effect {

        private final Collection<Effect> effects;

        public AndEffect(@JsonProperty("effects") final Collection<Effect> effects) {
            this.effects = effects;
        }

        @Override
        public void apply(final UrbanSensingEngine use) {
            effects.stream().forEach(eff -> eff.apply(use));
        }
    }

    public static class TextEffect extends Effect {

        private final String message;

        public TextEffect(@JsonProperty("message") final String message) {
            this.message = message;
        }

        @Override
        public void apply(final UrbanSensingEngine use) {
            use.printMessage(use.contextualize(message));
        }
    }

    public static class MQTTEffect extends Effect {

        private final String topic;
        private final String message;

        public MQTTEffect(@JsonProperty("topic") final String topic, @JsonProperty("message") final String message) {
            this.topic = topic;
            this.message = message;
        }

        @Override
        public void apply(final UrbanSensingEngine use) {
            use.publishMessage(topic, use.contextualize(message));
        }
    }

    public static class SetEffect extends Effect {

        private static Logger LOG = Logger.getLogger(SetEffect.class.getName());
        private final String field;
        private final double amount;

        public SetEffect(@JsonProperty("field") final String field, @JsonProperty("amount") final double amount) {
            this.field = field;
            this.amount = amount;
        }

        @Override
        public void apply(final UrbanSensingEngine use) {
            try {
                use.set(field, UrbanSensingEngine.OBJECT_MAPPER
                        .readTree(Double.toString(amount)));
            } catch (final JsonProcessingException e) {
                LOG.log(Level.SEVERE, "Parsing failure..", e);
            }
        }
    }

    public static class IncreaseEffect extends Effect {

        private static Logger LOG = Logger.getLogger(IncreaseEffect.class.getName());
        private final String field;
        private final double amount;

        public IncreaseEffect(@JsonProperty("field") final String field, @JsonProperty("amount") final double amount) {
            this.field = field;
            this.amount = amount;
        }

        @Override
        public void apply(final UrbanSensingEngine use) {
            try {
                if (use.get(field) == null)
                    use.set(field, UrbanSensingEngine.OBJECT_MAPPER.readTree(Double.toString(0)));
                else
                    use.set(field, UrbanSensingEngine.OBJECT_MAPPER
                            .readTree(Double.toString(use.get(field).asDouble() + amount)));
            } catch (final JsonProcessingException e) {
                LOG.log(Level.SEVERE, "Parsing failure..", e);
            }
        }
    }

    public static class AverageEffect extends Effect {

        private static Logger LOG = Logger.getLogger(AverageEffect.class.getName());
        private final String field;
        private final String average_field;
        private final int buffer_size;

        public AverageEffect(@JsonProperty("field") final String field,
                @JsonProperty("average_field") final String average_field,
                @JsonProperty("buffer_size") final int buffer_size) {
            this.field = field;
            this.average_field = average_field;
            this.buffer_size = buffer_size;
        }

        @Override
        public void apply(UrbanSensingEngine use) {
            try {
                if (use.get(average_field) == null)
                    use.set(average_field, UrbanSensingEngine.OBJECT_MAPPER.readTree("{\"buffer\":[], \"average\":0}"));
                else {
                    var av_node = use.get(average_field).deepCopy();
                    var buffer = av_node.get("buffer");
                    if (buffer.size() == buffer_size)
                        ((ArrayNode) buffer).remove(0);
                    ((ArrayNode) buffer).add(use.get(field));
                    double avg = 0;
                    for (var n : buffer)
                        avg += n.asDouble();
                    avg /= buffer.size();
                    ((ObjectNode) av_node).replace("average", UrbanSensingEngine.OBJECT_MAPPER
                            .readTree(Double.toString(avg)));
                }
            } catch (final JsonProcessingException e) {
                LOG.log(Level.SEVERE, "Parsing failure..", e);
            }
        }
    }
}
