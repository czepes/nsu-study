package ru.nsu.calculator;

import java.io.File;
import java.io.PrintStream;

import ru.nsu.calculator.exceptions.factory.FactoryException;

public class App {
    public static void printHelp(PrintStream out) {
        out.println("""
                Stack Calculator Program
                usage: calc [input.calc = stdin] [-c | --commands] [-h | --help]
                    [input.calc]      - input file with sequence of commands,
                                        must have .calc extension,
                                        if no file was given - interactive mode
                    [-c | --commands] - print commands list
                    [-h | --help]     - print this message
                """);
    }

    public static void main(String[] args) {
        File file = null;
        boolean printCommands = false;

        for (String arg : args) {
            if (arg.equals("-h") || arg.equals("--help")) {
                printHelp(System.out);
            }
            if (arg.equals("-c") || arg.equals("--commands")) {
                printCommands = true;
            } else if (arg.endsWith(".calc")) {
                file = new File(arg);
            }
        }

        Calculator calculator;

        try {
            calculator = new Calculator();
        } catch (FactoryException e) {
            System.err.println(e.what());
            return;
        }

        calculator.run(file, printCommands);
    }
}
