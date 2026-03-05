package ru.nsu.calculator.operations;

import ru.nsu.calculator.components.ExecutionContext;
import ru.nsu.calculator.exceptions.command.StackUnderflowException;

public class PrintOperation extends Operation {
    public PrintOperation(ExecutionContext context) {
        super(context);
    }

    @Override
    public void execute() throws StackUnderflowException {
        super.execute();
        if (ctx.stackSize() < 1) {
            throw new StackUnderflowException("PRINT", "Stack is empty");
        }

        System.out.println(ctx.stackPeek());
    }
}
