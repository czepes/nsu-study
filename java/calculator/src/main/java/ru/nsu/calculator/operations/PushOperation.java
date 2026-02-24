package ru.nsu.calculator.operations;

import ru.nsu.calculator.components.Command;
import ru.nsu.calculator.components.ExecutionContext;
import ru.nsu.calculator.exceptions.command.InvalidArgumentException;

public class PushOperation extends Operation {
    public PushOperation(ExecutionContext context) {
        super(context);
    }

    @Override
    public void execute() throws InvalidArgumentException {
        super.execute();
        Command cmd = ctx.getLastCommand();

        if (cmd.countArgs() < 1) {
            throw new InvalidArgumentException("PUSH", "Expected a value");
        }

        String value = cmd.getArg(0);
        double number;

        if (ctx.containsAlias(value)) {
            number = ctx.getAlias(value);
        } else {
            try {
                number = Double.parseDouble(value);
            } catch (NumberFormatException e) {
                throw new InvalidArgumentException("PUSH", "Wrong value format '%s'".formatted(value));
            }
        }

        ctx.stackPush(number);
    }
}
