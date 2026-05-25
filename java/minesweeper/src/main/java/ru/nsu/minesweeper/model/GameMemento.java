package ru.nsu.minesweeper.model;

import java.io.Serializable;

public class GameMemento implements Serializable {
    private static final long serialVersionUID = 1L;

    final Board board;
    final GameState state;
    final int elapsedSeconds;

    protected GameMemento(Board board, GameState state,
            int elapsedSeconds) {
        this.elapsedSeconds = elapsedSeconds;
        this.state = state;
        this.board = board;
    }
}
