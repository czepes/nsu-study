
package ru.nsu.minesweeper.controller;

import java.io.Serializable;
import java.util.Date;

public class Highscore implements Serializable {
    private static final long serialVersionUID = 1L;

    private final String playerName;
    private final int timeInSeconds;
    private final int rows;
    private final int cols;
    private final int mines;
    private final Date date;

    public Highscore(String playerName, int timeInSeconds, int rows, int cols, int mines) {
        this.playerName = playerName;
        this.timeInSeconds = timeInSeconds;
        this.rows = rows;
        this.cols = cols;
        this.mines = mines;
        this.date = new Date();
    }

    public String getPlayerName() {
        return playerName;
    }

    public int getTimeInSeconds() {
        return timeInSeconds;
    }

    public int getRows() {
        return rows;
    }

    public int getCols() {
        return cols;
    }

    public int getMines() {
        return mines;
    }

    public Date getDate() {
        return date;
    }

    public String getDifficulty() {
        return rows + "x" + cols + ", " + mines + " mines";
    }

    @Override
    public String toString() {
        return String.format("%-15s | %-15s | %3d sec | %s",
                playerName, getDifficulty(), timeInSeconds, date);
    }
}
