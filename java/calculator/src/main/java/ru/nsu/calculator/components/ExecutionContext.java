package ru.nsu.calculator.components;

import java.util.ArrayDeque;
import java.util.Deque;
import java.util.HashMap;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ExecutionContext {
    private static final Logger log = LoggerFactory.getLogger(ExecutionContext.class);

    private final Map<String, Double> aliases;
    private final Deque<Double> stack;
    private Command lastCmd;
    private double lastResult;

    public ExecutionContext() {
        this(new HashMap<>(), new ArrayDeque<>());
    }

    public ExecutionContext(Map<String, Double> aliases, Deque<Double> stack) {
        log.info("Creating Execution Context");
        this.aliases = aliases;
        this.stack = stack;
        this.lastCmd = null;
        log.info("Execution Context {} created", this);
    }

    public void update(Command cmd) {
        log.info("Context command updated with '{}'", cmd);
        this.lastCmd = cmd;
    }

    public final Command getLastCommand() {
        return lastCmd;
    }

    public double getLastResult() {
        return lastResult;
    }

    public Double stackPop() {
        lastResult = stack.pop();
        return lastResult;
    }

    public void stackPush(double value) {
        stack.push(value);
        lastResult = value;
    }

    public double stackPeek() {
        lastResult = stack.peekFirst();
        return lastResult;
    }

    public int stackSize() {
        return stack.size();
    }

    public boolean stackEmpty() {
        return stack.isEmpty();
    }

    public double getAlias(String alias) {
        lastResult = aliases.get(alias);
        return lastResult;
    }

    public boolean containsAlias(String alias) {
        return aliases.containsKey(alias);
    }

    public void setAlias(String alias, double value) {
        aliases.put(alias, value);
        lastResult = value;
    }
}
