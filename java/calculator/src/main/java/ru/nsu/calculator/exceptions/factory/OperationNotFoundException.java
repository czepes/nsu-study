package ru.nsu.calculator.exceptions.factory;

public class OperationNotFoundException extends FactoryException {
    public OperationNotFoundException(String cmd) {
        super("[INPUT] Command '%s' not found".formatted(cmd));
    }
}
