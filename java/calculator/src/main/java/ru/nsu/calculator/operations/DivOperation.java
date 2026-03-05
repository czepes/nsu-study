package ru.nsu.calculator.operations;

import ru.nsu.calculator.exceptions.command.StackUnderflowException;
import ru.nsu.calculator.components.ExecutionContext;
import ru.nsu.calculator.exceptions.command.DivisionByZeroException;

public class DivOperation extends BinaryOperation {
    public DivOperation(ExecutionContext context) {
        super(context);
    }

    @Override
    public void execute() throws StackUnderflowException, DivisionByZeroException {
        super.execute();
        double[] numbers = super.pop("/");
        double right = numbers[0];
        double left = numbers[1];

        if (right == 0) {
            ctx.stackPush(left);
            ctx.stackPush(right);
            throw new DivisionByZeroException("/", left, right);
        }

        ctx.stackPush(left / right);
    }
}
