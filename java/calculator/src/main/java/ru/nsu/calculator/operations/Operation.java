package ru.nsu.calculator.operations;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import ru.nsu.calculator.components.ExecutionContext;

public class Operation {
    protected final Logger log = LoggerFactory.getLogger(getClass());

    ExecutionContext ctx;

    public Operation(ExecutionContext context) {
        log.info("Creating Executable Command");
        this.ctx = context;
        log.info("Executable Command {} created", this);
    }

    public void execute() {
        log.info("Execute '{}'", ctx.getLastCommand());
    };
}
