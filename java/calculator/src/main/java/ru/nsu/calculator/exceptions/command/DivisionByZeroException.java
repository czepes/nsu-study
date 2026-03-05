package ru.nsu.calculator.exceptions.command;

public class DivisionByZeroException extends CommandException {
    public DivisionByZeroException(String location, double left, double right) {
        super(location, "Divisin by zero: %f / %f".formatted(left, right));
    }
}
