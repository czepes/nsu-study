package ru.nsu.calculator.components;

import java.util.Arrays;

public class Command {
    private CommandType type;
    private String cmd = "";
    private String[] args = {};

    public static Command parse(String line) {
        if (line == null) {
            return null;
        }
        return new Command(line.split(" "));
    }

    public Command(String[] args) {
        if (args != null) {
            this.cmd = args[0].trim().toLowerCase();
            if (args.length >= 1) {
                this.args = Arrays.copyOfRange(args, 1, args.length);
            }
        }
        this.type = switch (this.cmd) {
            case "", "#" -> CommandType.SKIP;
            case "help" -> CommandType.HELP;
            case "quit", "exit" -> CommandType.EXIT;
            default -> CommandType.EXEC;
        };
    }

    public CommandType getType() {
        return type;
    }

    public String getCommandName() {
        return cmd;
    }

    public String getArg(int idx) {
        return args == null ? null : args[idx];
    }

    public int countArgs() {
        return args == null ? 0 : args.length;
    }

    public String toString() {
        return "%s %s".formatted(cmd, String.join(" ", args));
    }
}
