package ru.nsu.calculator.operations;

import ru.nsu.calculator.components.ExecutionContext;
import ru.nsu.calculator.exceptions.command.StackUnderflowException;

public class PopOperation extends Operation {
    public PopOperation(ExecutionContext context) {
        super(context);
    }

    @Override
    public void execute() throws StackUnderflowException {
        super.execute();
        if (ctx.stackEmpty()) {
            throw new StackUnderflowException("POP", "Stack is empty");
        }
        ctx.stackPop();
    }
}
