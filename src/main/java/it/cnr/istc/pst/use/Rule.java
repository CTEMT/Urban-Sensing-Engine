package it.cnr.istc.pst.use;

import com.fasterxml.jackson.annotation.JsonProperty;

public class Rule {

    private final Condition condition;
    private final Effect effect;

    public Rule(@JsonProperty("condition") final Condition condition, @JsonProperty("effect") final Effect effect) {
        this.condition = condition;
        this.effect = effect;
    }

    public boolean verify(final UrbanSensingEngine use) {
        return condition.verify(use);
    }

    public void apply(final UrbanSensingEngine use) {
        effect.apply(use);
    }
}
