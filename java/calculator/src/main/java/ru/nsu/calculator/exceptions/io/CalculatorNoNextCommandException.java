package ru.nsu.calculator.exceptions.io;

public class CalculatorNoNextCommandException extends CalculatorIOException {
    public CalculatorNoNextCommandException() {
        super("No more commands available");
    }
}
