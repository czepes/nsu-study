package ru.nsu.calculator.operations;

import ru.nsu.calculator.components.ExecutionContext;
import ru.nsu.calculator.exceptions.command.StackUnderflowException;

public class SumOperation extends BinaryOperation {
    public SumOperation(ExecutionContext context) {
        super(context);
    }

    @Override
    public void execute() throws StackUnderflowException {
        super.execute();
        double[] numbers = super.pop("+");
        double right = numbers[0];
        double left = numbers[1];
        ctx.stackPush(left + right);
    }
}
