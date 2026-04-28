package ru.nsu.minesweeper.model;

import java.io.Serializable;
import java.util.HashSet;
import java.util.Random;
import java.util.Set;

import ru.nsu.minesweeper.error.CellException;

public class Board implements Serializable {
    private static final long serialVersionUID = 1L;

    private Cell[][] cells;
    private int rows;
    private int cols;
    private int totalMines;
    private int remainingMines;
    private int revealedCells;

    protected Board(Board board) {
        this(board.rows, board.cols, board.totalMines);
    }

    protected Board(int rows, int cols, int mines) {
        this.rows = rows;
        this.cols = cols;
        this.totalMines = mines;
        this.remainingMines = mines;
        this.revealedCells = 0;
        initBoard();
    }

    private void initBoard() {
        cells = new Cell[rows][cols];
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cells[i][j] = new Cell();
            }
        }
    }

    public void placeMines(int safeRow, int safeCol) {
        Random random = new Random();
        Set<Coords> safeZone = getAdjacentCells(safeRow, safeCol);
        safeZone.add(new Coords(safeRow, safeCol));

        int minesPlaced = 0;
        while (minesPlaced < totalMines) {
            int row = random.nextInt(rows);
            int col = random.nextInt(cols);

            if (!safeZone.contains(new Coords(row, col)) && !cells[row][col].isMine()) {
                cells[row][col].setMine(true);
                minesPlaced++;
            }
        }

        calculateAdjacentMines();
    }

    private void calculateAdjacentMines() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (cells[i][j].isMine()) {
                    continue;
                }

                int count = 0;

                for (Coords adj : getAdjacentCells(i, j)) {
                    if (cells[adj.row()][adj.col()].isMine()) {
                        count++;
                    }
                }

                cells[i][j].setAdjacentMines(count);
            }
        }
    }

    private Set<Coords> getAdjacentCells(int row, int col) {
        Set<Coords> adjacentCells = new HashSet<>();

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) {
                    continue;
                }

                int adjRow = row + i;
                int adjCol = col + j;

                if (isValidCell(adjRow, adjCol)) {
                    adjacentCells.add(new Coords(adjRow, adjCol));
                }
            }
        }

        return adjacentCells;
    }

    protected void revealCell(int row, int col) {
        if (!isValidCell(row, col)) {
            return;
        }
        cells[row][col].reveal();
        revealedCells++;
    }

    protected boolean isValidCell(int row, int col) {
        return 0 <= row && row < rows && 0 <= col && col < cols;
    }

    protected boolean checkWin() {
        int totalCells = rows * cols;
        return revealedCells == totalCells - totalMines;
    }

    protected int getRows() {
        return rows;
    }

    protected int getCols() {
        return cols;
    }

    protected int getTotalMines() {
        return totalMines;
    }

    protected int getRemainingMines() {
        return remainingMines;
    }

    protected boolean isCellRevealed(int row, int col) throws CellException {
        if (!isValidCell(row, col)) {
            throw new CellException("", col, row, rows, cols);
        }
        return cells[row][col].isRevealed();
    }

    protected boolean isCellMine(int row, int col) throws CellException {
        if (!isValidCell(row, col)) {
            throw new CellException("", col, row, rows, cols);
        }
        return cells[row][col].isMine();
    }

    protected boolean isCellFlagged(int row, int col) throws CellException {
        if (!isValidCell(row, col)) {
            throw new CellException("", col, row, rows, cols);
        }
        return cells[row][col].isFlagged();
    }

    protected int getCellAdjacentMines(int row, int col) throws CellException {
        if (!isValidCell(row, col)) {
            throw new CellException("", col, row, rows, cols);
        }
        return cells[row][col].getAdjacentMines();
    }

    protected void toggleCellFlag(int row, int col) throws CellException {
        if (!isValidCell(row, col)) {
            throw new CellException("", col, row, rows, cols);
        }
        boolean isFlagged = cells[row][col].toggleFlag();
        remainingMines += isFlagged ? -1 : 1;
    }

    protected Cell getCell(int row, int col) throws CellException {
        if (!isValidCell(row, col)) {
            throw new CellException("", col, row, rows, cols);
        }
        return cells[row][col];
    }

    protected void revealAllMines() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (cells[i][j].isMine()) {
                    cells[i][j].forcedReveal();
                }
            }
        }
    }
}
