package ru.nsu.calculator.exceptions.command;

public class StackUnderflowException extends CommandException {
    public StackUnderflowException(String location, String msg) {
        super(location, msg);
    }
}
