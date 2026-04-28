package ru.nsu.minesweeper.error;

public class LoadException extends GameException {
    private final String src;

    public LoadException(String src, String msg) {
        super(msg);
        this.src = src;
    }

    @Override
    public String getMessage() {
        return "Failed to load " + src + ": " + super.getMessage();
    }
}
