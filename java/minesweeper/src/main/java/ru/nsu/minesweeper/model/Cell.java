package ru.nsu.minesweeper.model;

import java.io.Serializable;

public class Cell implements Serializable {
    private static final long serialVersionUID = 1L;

    private boolean isMine;
    private boolean isRevealed;
    private boolean isFlagged;
    private int adjacentMines;

    public Cell() {
        isMine = false;
        isRevealed = false;
        isFlagged = false;
        adjacentMines = 0;
    }

    public boolean isMine() {
        return isMine;
    }

    protected void setMine(boolean mine) {
        isMine = mine;
    }

    public boolean isRevealed() {
        return isRevealed;
    }

    protected boolean reveal() {
        if (isFlagged || isRevealed) {
            return false;
        }
        isRevealed = true;
        return true;
    }

    protected boolean forcedReveal() {
        if (isRevealed) {
            return false;
        }
        isRevealed = true;
        return true;
    }

    public boolean isFlagged() {
        return isFlagged;
    }

    protected boolean toggleFlag() {
        if (isRevealed) {
            return false;
        }
        isFlagged = !isFlagged;
        return isFlagged;
    }

    public int getAdjacentMines() {
        return adjacentMines;
    }

    protected void incrementAdjacentMines() {
        adjacentMines++;
    }

    protected void setAdjacentMines(int mines) {
        if (mines < 0 || mines > 8) {
            return;
        }
        adjacentMines = mines;
    }

    @Override
    public String toString() {
        if (isRevealed) {
            if (isMine) {
                return "💣";
            }
            return adjacentMines > 0 ? String.valueOf(adjacentMines) : "  ";
        }
        return isFlagged ? "🚩" : "##";
    }
}
