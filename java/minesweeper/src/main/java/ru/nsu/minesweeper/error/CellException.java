package ru.nsu.minesweeper.error;

public class CellException extends GameException {
    private int row, col, rows, cols;

    public CellException(String msg, int col, int row, int rows, int cols) {
        super(msg);
        this.row = row;
        this.col = col;
        this.rows = rows;
        this.cols = cols;
    }

    @Override
    public String getMessage() {
        return "Illegal board coordinates [" + row + ", " + col + "] in board " + rows + "x" + cols;
    }
}
