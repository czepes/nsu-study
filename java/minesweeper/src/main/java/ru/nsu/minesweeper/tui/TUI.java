package ru.nsu.minesweeper.tui;

import java.beans.PropertyChangeEvent;
import java.util.List;
import java.util.Scanner;

import ru.nsu.minesweeper.controller.GameController;
import ru.nsu.minesweeper.controller.Highscore;
import ru.nsu.minesweeper.model.CellUpdate;
import ru.nsu.minesweeper.model.GameModel;
import ru.nsu.minesweeper.model.GameState;
import ru.nsu.minesweeper.view.GameView;

public class TUI implements GameView {
    private final Scanner scanner;
    private GameModel model;
    private GameController controller;

    public TUI(GameModel model, GameController controller) {
        this.scanner = new Scanner(System.in);
        this.model = model;
        this.controller = controller;
    }

    public String next() {
        return scanner.nextLine().trim();
    }

    /*
     * GameView implementation
     */

    @Override
    public void showAbout() {
        System.out.println("""
                === ABOUT THE MINESWEEPER ===
                Minesweeper - classical logical game
                Version: 0.1
                Architecture: MVC (Model-View-Controller)
                Author: czepes
                Rules:
                - Game field has mines.
                - Objective: open all cells without touching whose mines.
                - Numbers in cells display amount of mines on adjacent cells.
                - Use flags to mark possible mine locations.
                - If you open a cell with a mine - GAME OVER.
                """);
    }

    @Override
    public void fullUpdate(GameModel model) {
        this.model = model;
        printGameField();
    }

    @Override
    public void cellUpdate(CellUpdate update) {
        printGameField();
    }

    @Override
    public void updateMinesCount(int remainingMines) {
        // in status line
    }

    @Override
    public void updateTimer(int seconds) {
        // in status line
    }

    @Override
    public void showGameOver(boolean won, int seconds) {
        if (won) {
            System.out.println();
            System.out.println("=== CONGRATULATIONS! YOU HAVE WON! ===");
            System.out.println("Time: " + seconds + " seconds");
            System.out.println();
        } else {
            System.out.println();
            System.out.println("=== GAME OVER! YOU HIT A MINE! ===");
            System.out.println();
        }
    }

    @Override
    public void showHighscorePrompt() {
        System.out.println();
        System.out.println("=== NEW HIGHSCORE! ===");
        System.out.println();
    }

    @Override
    public void showHighscores(List<Highscore> highscores) {
        System.out.println();
        System.out.println("=== HIGHSCORE TABLE ===");

        if (highscores.isEmpty()) {
            System.out.println("No highscore records.");
            System.out.println();
            return;
        }

        System.out.println("================================================================");
        System.out.printf("%-3s %-15s %-10s %-15s %-15s\n", "#id", "Player Name", "Time", "Difficulty", "Date");
        System.out.println("================================================================");

        int index = 1;
        for (Highscore highscore : highscores) {
            System.out.printf("%-3s %-15s %-10s %-15s %-15s\n",
                    index++,
                    highscore.getPlayerName(),
                    highscore.getTimeInSeconds() + " sec.",
                    highscore.getDifficulty(),
                    highscore.getDate());
        }
        System.out.println("================================================================");
        System.out.println();
    }

    @Override
    public void showMessage(String msg) {
        System.out.print(msg);
    }

    @Override
    public void showError(String msg) {
        System.err.print("ERROR: " + msg);
    }

    @Override
    public String requestPlayerName() {
        System.out.print("Enter your name:\n> ");
        return next();
    }

    /**
     * Graphics
     */

    private void printCommands() {
        System.out.print("""
                Commands: [o]pen | [f]lag | [n]ew | [h]ighscores
                          [s]ave | [l]oad | [a]bout | [e]xit | help
                """);
    }

    private void printStatus() {
        System.out.println("Status:          " + getGameStateString());
        System.out.println("Remaining Mines: " + model.getRemainingMines());
        System.out.println("Time:            " + model.getElapsedSeconds() + " sec.");
        System.out.println();
    }

    private String getGameStateString() {
        return switch (model.getState()) {
            case PRE -> "NOT STARTED";
            case ACTIVE -> "ACTIVE";
            case WON -> "WON!";
            case LOST -> "LOST!";
        };
    }

    public void printGameField() {
        printStatus();
        printCommands();
        printBoard();
    }

    private void printBoard() {
        if (model == null) {
            return;
        }

        int rows = model.getRows();
        int cols = model.getCols();

        System.out.println();

        System.out.print("    ");
        for (int j = 0; j < cols; j++) {
            System.out.printf("%4d", j);
        }
        System.out.println();

        System.out.print("    ");
        for (int j = 0; j < cols; j++) {
            System.out.print("----");
        }
        System.out.println();

        for (int i = 0; i < rows; i++) {
            System.out.printf("%2d |", i);
            for (int j = 0; j < cols; j++) {
                System.out.printf("%4s", getCellSymbol(i, j));
            }
            System.out.println();
        }
        System.out.println();
    }

    private String getCellSymbol(int row, int col) {
        if (model.isCellRevealed(row, col)) {
            int mines = model.getCellAdjacentMines(row, col);
            if (model.isCellMine(row, col)) {
                return "💣";
            } else if (mines > 0) {
                return String.valueOf(mines);
            }
            return "  ";
        }

        if (model.isCellFlagged(row, col)) {
            return "🚩";
        }

        return "##";
    }

    @Override
    public void propertyChange(PropertyChangeEvent evt) {
        switch (evt.getPropertyName()) {
            case GameModel.BOARD_CHANGE -> fullUpdate(model);
            case GameModel.CELL_REVEAL -> {
                CellUpdate update = (CellUpdate) evt.getNewValue();
                cellUpdate(update);
            }
            case GameModel.FLAG_TOGGLE -> {
                CellUpdate update = (CellUpdate) evt.getNewValue();
                cellUpdate(update);
            }
            case GameModel.MINES_UPDATE -> {
                int mines = (int) evt.getNewValue();
                updateMinesCount(mines);
            }
            case GameModel.TIMER_UPDATE -> {
                int seconds = (int) evt.getNewValue();
                updateTimer(seconds);
            }
            case GameModel.STATE_CHANGE -> {
                GameState state = (GameState) evt.getNewValue();
                if (!model.isGameOver()) {
                    return;
                }

                model.endGame();
                fullUpdate(model);

                boolean won = state == GameState.WON;
                showGameOver(won, model.getElapsedSeconds());

                if (won && controller.isHighscore()) {
                    String name = requestPlayerName();
                    if (name != null && !name.isEmpty()) {
                        controller.saveHighscore(name);
                    }
                }
            }
        }
    }

}
