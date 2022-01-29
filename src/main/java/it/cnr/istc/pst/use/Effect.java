package it.cnr.istc.pst.use;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSubTypes;
import com.fasterxml.jackson.annotation.JsonSubTypes.Type;
import com.fasterxml.jackson.annotation.JsonTypeInfo;

@JsonTypeInfo(use = JsonTypeInfo.Id.NAME, include = JsonTypeInfo.As.PROPERTY, property = "type")
@JsonSubTypes({ @Type(value = Effect.TextEffect.class, name = "text"),
        @Type(value = Effect.MQTTEffect.class, name = "mqtt") })
public abstract class Effect {

    public abstract void apply(final UrbanSensingEngine use);

    public static class TextEffect extends Effect {

        private final String message;

        public TextEffect(@JsonProperty("message") final String message) {
            this.message = message;
        }

        @Override
        public void apply(final UrbanSensingEngine use) {
            use.printMessage(message);
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
            use.publishMessage(topic, message);
        }
    }
}
