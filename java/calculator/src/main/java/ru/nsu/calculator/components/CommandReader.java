package ru.nsu.calculator.components;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Iterator;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import ru.nsu.calculator.exceptions.io.CalculatorFileNotFoundException;
import ru.nsu.calculator.exceptions.io.CalculatorFileNotReadableException;
import ru.nsu.calculator.exceptions.io.CalculatorIOException;
import ru.nsu.calculator.exceptions.io.CalculatorNoNextCommandException;

public class CommandReader implements Iterator<Command>, AutoCloseable {
    private static final Logger log = LoggerFactory.getLogger(CommandReader.class);

    private BufferedReader br;
    private String nextLine;
    private int lineNumber = 0;
    private boolean closed = false;
    private boolean isStdin = false;

    private CommandReader(BufferedReader br, boolean isStdin) {
        this.br = br;
        this.isStdin = isStdin;
        log.info("Command Reader {} created", this);
    }

    public static CommandReader fromFile(File file)
            throws CalculatorFileNotFoundException {
        log.info("Creating Command Reader with file {}", file);
        if (!file.exists()) {
            throw new CalculatorFileNotFoundException(file);
        }
        if (!file.canRead()) {
            throw new CalculatorFileNotReadableException(file);
        }
        CommandReader cr;
        try {
            BufferedReader br = new BufferedReader(
                    new InputStreamReader(new FileInputStream(file)));
            cr = new CommandReader(br, false);
        } catch (FileNotFoundException e) {
            throw new CalculatorFileNotFoundException(file);
        }
        return cr;
    }

    public static CommandReader fromStdin() {
        log.info("Creating Command Reader with stdin");
        BufferedReader br = new BufferedReader(
                new InputStreamReader(System.in));
        CommandReader cr = new CommandReader(br, true);
        return cr;
    }

    public static CommandReader from(File file) {
        return file != null ? fromFile(file) : fromStdin();
    }

    private void advance() throws CalculatorIOException {
        log.info("Reading next line");
        if (closed) {
            return;
        }
        try {
            if (isStdin && nextLine == null) {
                System.out.print("[%d] $ ".formatted(lineNumber));
            }
            nextLine = br.readLine();
            if (nextLine != null) {
                lineNumber++;
            }
        } catch (IOException e) {
            log.error(e.getMessage(), e);
            throw new CalculatorIOException("Failed to read next command");
        }
    }

    public int getLineNumber() {
        return lineNumber;
    }

    @Override
    public boolean hasNext() {
        if (closed) {
            return false;
        }
        if (nextLine != null) {
            return true;
        }

        advance();

        return nextLine != null;
    }

    @Override
    public Command next() throws CalculatorIOException {
        if (!hasNext()) {
            throw new CalculatorNoNextCommandException();
        }
        Command cmd = Command.parse(nextLine);
        nextLine = null;
        return cmd;
    }

    @Override
    public void close() {
        log.info("Close Command Reader");
        if (closed) {
            return;
        }
        closed = true;
        if (isStdin) {
            return;
        }
        try {
            br.close();
        } catch (IOException e) {
            log.error(e.getMessage(), e);
            throw new CalculatorIOException("Failed to close input stream");
        }
    }

}
