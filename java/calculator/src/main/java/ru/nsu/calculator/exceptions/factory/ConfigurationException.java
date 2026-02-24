package ru.nsu.calculator.exceptions.factory;

public class ConfigurationException extends FactoryException {
    public ConfigurationException(String msg) {
        super("[CONFIG] %s".formatted(msg));
    }
}
