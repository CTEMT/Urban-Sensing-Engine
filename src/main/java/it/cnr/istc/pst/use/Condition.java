package it.cnr.istc.pst.use;

import java.util.Collection;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSubTypes;
import com.fasterxml.jackson.annotation.JsonSubTypes.Type;
import com.fasterxml.jackson.annotation.JsonTypeInfo;

@JsonTypeInfo(use = JsonTypeInfo.Id.NAME, include = JsonTypeInfo.As.PROPERTY, property = "type")
@JsonSubTypes({ @Type(value = Condition.AndCondition.class, name = "and"),
        @Type(value = Condition.OrCondition.class, name = "or"),
        @Type(value = Condition.ObjectCondition.class, name = "object"),
        @Type(value = Condition.NumericCondition.class, name = "numeric") })
public abstract class Condition {

    public abstract boolean verify(final UrbanSensingEngine use);

    public static class AndCondition extends Condition {

        private final Collection<Condition> conditions;

        public AndCondition(@JsonProperty("conditions") final Collection<Condition> conditions) {
            this.conditions = conditions;
        }

        @Override
        public boolean verify(final UrbanSensingEngine use) {
            return conditions.stream().allMatch(cond -> cond.verify(use));
        }
    }

    public static class OrCondition extends Condition {

        private final Collection<Condition> conditions;

        public OrCondition(@JsonProperty("conditions") final Collection<Condition> conditions) {
            this.conditions = conditions;
        }

        @Override
        public boolean verify(final UrbanSensingEngine use) {
            return conditions.stream().anyMatch(cond -> cond.verify(use));
        }
    }

    public static class ObjectCondition extends Condition {

        private final String var_name, var_value;
        private final VarCondType op;

        public ObjectCondition(@JsonProperty("var_name") final String var_name,
                @JsonProperty("var_value") final String var_value,
                @JsonProperty("op") final VarCondType op) {
            this.var_name = var_name;
            this.op = op == null ? VarCondType.Eq : op;
            this.var_value = var_value;
        }

        @Override
        public boolean verify(final UrbanSensingEngine use) {
            final String[] vars = var_name.split(".");
            var ctx_value = use.state.get(vars[0]);
            for (int i = 1; i < vars.length; i++)
                ctx_value = use.state.get(vars[i]);
            switch (op) {
                case Eq:
                    return ctx_value != null && ctx_value.asText().equals(var_value);
                case NEq:
                    return ctx_value != null && !ctx_value.asText().equals(var_value);
                default:
                    throw new IllegalArgumentException(String.valueOf(op));
            }
        }

        public enum VarCondType {
            Eq, NEq
        }
    }

    public static class NumericCondition extends Condition {

        private final String var_name;
        private final double var_value;
        private final NumVarCondType op;

        public NumericCondition(@JsonProperty("var_name") final String var_name,
                @JsonProperty("var_value") final double var_value,
                @JsonProperty("op") final NumVarCondType op) {
            this.var_name = var_name;
            this.op = op == null ? NumVarCondType.Eq : op;
            this.var_value = var_value;
        }

        @Override
        public boolean verify(final UrbanSensingEngine use) {
            final String[] vars = var_name.split(".");
            var ctx_value = use.state.get(vars[0]);
            for (int i = 1; i < vars.length; i++)
                ctx_value = use.state.get(vars[i]);
            switch (op) {
                case Eq:
                    return ctx_value != null && ctx_value.asDouble() == var_value;
                case GEq:
                    return ctx_value != null && ctx_value.asDouble() >= var_value;
                case Gt:
                    return ctx_value != null && ctx_value.asDouble() > var_value;
                case LEq:
                    return ctx_value != null && ctx_value.asDouble() <= var_value;
                case Lt:
                    return ctx_value != null && ctx_value.asDouble() < var_value;
                case NEq:
                    return ctx_value != null && ctx_value.asDouble() != var_value;
                default:
                    throw new IllegalArgumentException(String.valueOf(op));
            }
        }

        public enum NumVarCondType {
            Lt, LEq, Eq, GEq, Gt, NEq
        }
    }
}
