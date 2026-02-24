package ru.nsu.calculator.operations;

import ru.nsu.calculator.components.ExecutionContext;
import ru.nsu.calculator.exceptions.command.InvalidArgumentException;

public class MulOperation extends BinaryOperation {
    public MulOperation(ExecutionContext context) {
        super(context);
    }

    @Override
    public void execute() throws InvalidArgumentException {
        super.execute();
        double[] numbers = super.pop("+");
        double right = numbers[0];
        double left = numbers[1];
        ctx.stackPush(left * right);
    }
}
