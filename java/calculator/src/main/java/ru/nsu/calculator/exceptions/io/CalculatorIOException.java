package ru.nsu.calculator.exceptions.io;

import ru.nsu.calculator.exceptions.CalculatorException;

public class CalculatorIOException extends CalculatorException {
    public CalculatorIOException(String msg) {
        super("[IO] %s".formatted(msg));
    }
}
