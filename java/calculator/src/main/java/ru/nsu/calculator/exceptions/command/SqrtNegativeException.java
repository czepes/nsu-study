package ru.nsu.calculator.exceptions.command;

public class SqrtNegativeException extends CommandException {
    public SqrtNegativeException(double value) {
        super("SQRT", "Cannot take square root of negative value %f".formatted(value));
    }
}
