package ru.nsu.calculator.exceptions.io;

import java.io.File;

public class CalculatorFileNotFoundException extends CalculatorIOException {
    public CalculatorFileNotFoundException(File file) {
        super("File '%s' not found".formatted(file.getPath()));
    }
}
