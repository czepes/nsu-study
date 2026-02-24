package ru.nsu.calculator.exceptions.command;

public class InvalidArgumentException extends CommandException {
    public InvalidArgumentException(String location, String msg) {
        super(location, msg);
    }
}
