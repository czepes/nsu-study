package ru.nsu.calculator.exceptions.factory;

public class OperationCreationException extends FactoryException {
    public OperationCreationException(String cmd, String className) {
        super("[INPUT] Failed to create instance of '%s' for command '%s'".formatted(className, cmd));
    }
}
