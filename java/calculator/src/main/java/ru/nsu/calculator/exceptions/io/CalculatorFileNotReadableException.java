package ru.nsu.calculator.exceptions.io;

import java.io.File;

public class CalculatorFileNotReadableException extends CalculatorIOException {
    public CalculatorFileNotReadableException(File file) {
        super("File '%s' cannot be read".formatted(file.getPath()));
    }
}
