package ru.nsu.minesweeper.controller;

import java.util.List;

import ru.nsu.minesweeper.model.GameModel;

public class GameController {
    private HighscoreManager hsManager;
    private SaveManager svManager;
    private GameModel model;

    public GameController(GameModel model) {
        this.hsManager = new HighscoreManager();
        this.svManager = new SaveManager();
        this.model = model;
    }

    public void handleNewGame() {
        model.endGame();
        model.newGame();
    }

    public void handleNewGame(int rows, int cols, int mines) {
        model.endGame();
        model.newGame(rows, cols, mines);
    }

    public void handleCellTouch(int row, int col) {
        if (model.isGameOver()) {
            return;
        }
        model.revealCell(row, col);
    }

    public void handleFlagToggle(int row, int col) {
        if (model.isGameOver()) {
            return;
        }
        model.toggleFlag(row, col);
    }

    public void handleSave() {
        svManager.save(model);
    }

    public void handleLoad() {
        svManager.load(model);
    }

    public void handleExit() {
        model.endGame();
        System.exit(0);
    }

    public boolean isHighscore() {
        return hsManager.isHighscore(model.getElapsedSeconds());
    }

    public void saveHighscore(String playerName) {
        hsManager.addScore(playerName, model.getElapsedSeconds(), model.getRows(), model.getCols(),
                model.getTotalMines());
    }

    public List<Highscore> getHighscores() {
        return hsManager.getHighscores();
    }

}
