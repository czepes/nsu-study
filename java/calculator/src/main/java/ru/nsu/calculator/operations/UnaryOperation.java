package ru.nsu.calculator.operations;

import ru.nsu.calculator.components.ExecutionContext;
import ru.nsu.calculator.exceptions.command.StackUnderflowException;

public abstract class UnaryOperation extends Operation {
    public UnaryOperation(ExecutionContext context) {
        super(context);
    }

    protected double pop(String op) throws StackUnderflowException {
        super.execute();
        if (ctx.stackSize() < 1) {
            throw new StackUnderflowException(op, "Missing an operand");
        }
        return ctx.stackPop();
    }
}
