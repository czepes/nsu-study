package ru.nsu.calculator;

import ru.nsu.calculator.exceptions.command.CommandException;
import ru.nsu.calculator.exceptions.io.CalculatorIOException;

import ru.nsu.calculator.components.OperationFactory;
import ru.nsu.calculator.components.Command;
import ru.nsu.calculator.components.CommandReader;
import ru.nsu.calculator.components.ExecutionContext;

import java.io.File;
import java.io.PrintStream;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Calculator {
    private static final Logger log = LoggerFactory.getLogger(Calculator.class);
    private static final PrintStream defaultOutput = System.out;

    private ExecutionContext context;
    private OperationFactory factory;

    public Calculator() {
        this(new ExecutionContext(), new OperationFactory());
    }

    public Calculator(ExecutionContext context, OperationFactory factory) {
        log.info("Creating Calculator", this);
        this.context = context;
        this.factory = factory;
        log.info("Calculator {} created", this);
    }

    public int execute(String line) {
        return execute(Command.parse(line));
    }

    public int execute(Command cmd) {
        log.info("Execute '{}'", cmd);
        context.update(cmd);
        switch (cmd.getType()) {
            case SKIP: {
                break;
            }
            case HELP: {
                factory.printUsage(defaultOutput);
                break;
            }
            case EXEC: {
                factory.build(context).execute();
                break;
            }
            case EXIT: {
                return 1;
            }
        }
        return 0;
    }

    public void run(File file) {
        run(file, false);
    }

    public void run(File file, boolean printUsage) {
        log.info("Running calculator");
        if (printUsage) {
            execute(Command.parse("help"));
        }
        try (CommandReader cr = CommandReader.from(file)) {
            while (cr.hasNext()) {
                try {
                    if (execute(cr.next()) != 0) {
                        return;
                    }
                } catch (CommandException e) {
                    log.error(e.getMessage(), e);
                    System.err.println(e.what());
                }
            }
        } catch (CalculatorIOException e) {
            System.err.println(e.what());
        }
    }
}
