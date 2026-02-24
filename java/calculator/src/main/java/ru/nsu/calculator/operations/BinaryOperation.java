package ru.nsu.calculator.operations;

import ru.nsu.calculator.components.ExecutionContext;
import ru.nsu.calculator.exceptions.command.StackUnderflowException;

public abstract class BinaryOperation extends Operation {
    public BinaryOperation(ExecutionContext context) {
        super(context);
    }

    protected double[] pop(String op) throws StackUnderflowException {
        int size = ctx.stackSize();

        if (size < 2) {
            throw new StackUnderflowException(op, "Expected at least 2 operands, found %d".formatted(size));
        }

        double[] numbers = { ctx.stackPop(), ctx.stackPop() };
        return numbers;
    }
}
