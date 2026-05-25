package ru.nsu.minesweeper.error;

public class BoardException extends GameException {
    private int rows, cols, mines;

    public BoardException(String msg, int rows, int cols, int mines) {
        super(msg);
        this.rows = rows;
        this.cols = cols;
        this.mines = mines;
    }

    @Override
    public String getMessage() {
        return "Illegal board parameters " + rows + "x" + cols + " with " + mines + " mines ";
    }
}
