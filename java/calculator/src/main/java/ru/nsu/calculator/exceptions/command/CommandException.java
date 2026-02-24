package ru.nsu.calculator.exceptions.command;

import ru.nsu.calculator.exceptions.CalculatorException;

public class CommandException extends CalculatorException {
    public CommandException(String location, String msg) {
        super("[%s] %s".formatted(location, msg));
    }
}
