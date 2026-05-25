package ru.nsu.minesweeper.model;

public record CellUpdate(int row, int col,
        boolean isRevealed,
        boolean isFlagged,
        boolean isMine,
        int adjacentMines) {
}
