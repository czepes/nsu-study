package ru.nsu.minesweeper.error;

public class SaveException extends GameException {
    private final String src;

    public SaveException(String src, String msg) {
        super(msg);
        this.src = src;
    }

    @Override
    public String getMessage() {
        return "Failed to load " + src + ": " + super.getMessage();
    }
}
