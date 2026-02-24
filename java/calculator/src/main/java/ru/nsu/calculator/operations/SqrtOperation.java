package ru.nsu.calculator.operations;

import ru.nsu.calculator.components.ExecutionContext;
import ru.nsu.calculator.exceptions.command.SqrtNegativeException;

public class SqrtOperation extends UnaryOperation {
    public SqrtOperation(ExecutionContext context) {
        super(context);
    }

    @Override
    public void execute() throws SqrtNegativeException {
        super.execute();
        double number = super.pop("SQRT");
        if (number < 0) {
            ctx.stackPush(number);
            throw new SqrtNegativeException(number);
        }
        ctx.stackPush(Math.sqrt(number));
    }
}
