package ru.nsu.minesweeper.error;

public abstract class GameException extends RuntimeException {
    GameException(String msg) {
        super(msg);
    }
}
