package ru.nsu.calculator.exceptions;

public class CalculatorException extends RuntimeException {
    private String msg;

    public CalculatorException(String msg) {
        this.msg = "[CALCULATOR]%s".formatted(msg);
    }

    public String what() {
        return msg;
    }
}
