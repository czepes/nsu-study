package ru.nsu.calculator.operations;

import ru.nsu.calculator.components.Command;
import ru.nsu.calculator.components.ExecutionContext;
import ru.nsu.calculator.exceptions.command.InvalidArgumentException;

public class DefineOperation extends Operation {
    public DefineOperation(ExecutionContext context) {
        super(context);
    }

    @Override
    public void execute() throws InvalidArgumentException {
        super.execute();

        Command cmdline = ctx.getLastCommand();

        if (cmdline.countArgs() < 2) {
            throw new InvalidArgumentException("DEFINE", "Expected an alias and a value");
        }

        String alias = cmdline.getArg(0);
        if (!alias.chars().allMatch(Character::isLetter)) {
            throw new InvalidArgumentException(
                    "DEFINE", "Wrong alias: '%s', only alphabetic characters allowed".formatted(alias));
        }

        String value = cmdline.getArg(1);
        double number;
        try {
            number = Double.parseDouble(value);
        } catch (NumberFormatException e) {
            throw new InvalidArgumentException("DEFINE", "Wrong value format '%s'".formatted(value));
        }

        ctx.setAlias(alias, number);
    }
}
