package it.cnr.istc.pst.use;

import java.util.Collection;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSubTypes;
import com.fasterxml.jackson.annotation.JsonSubTypes.Type;
import com.fasterxml.jackson.annotation.JsonTypeInfo;

@JsonTypeInfo(use = JsonTypeInfo.Id.NAME, include = JsonTypeInfo.As.PROPERTY, property = "type")
@JsonSubTypes({ @Type(value = Effect.AndEffect.class, name = "and"),
        @Type(value = Effect.TextEffect.class, name = "text"),
        @Type(value = Effect.MQTTEffect.class, name = "mqtt") })
public abstract class Effect {

    public abstract void apply(final UrbanSensingEngine use);

    public static class AndEffect extends Effect {

        private final Collection<Effect> effects;

        public AndEffect(@JsonProperty("effects") final Collection<Effect> effects) {
            this.effects = effects;
        }

        @Override
        public void apply(UrbanSensingEngine use) {
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
}
