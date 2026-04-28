package ru.nsu.minesweeper.gui;

import ru.nsu.minesweeper.controller.GameController;
import ru.nsu.minesweeper.controller.Highscore;
import ru.nsu.minesweeper.model.*;
import ru.nsu.minesweeper.view.GameView;

import javax.swing.*;
import java.awt.*;
import java.beans.PropertyChangeEvent;
import java.util.List;

public class GUI extends JFrame implements GameView {
    private BoardPanel boardPanel;
    private StatusPanel statusPanel;
    private GameController controller;
    private GameModel model;

    private static final int CELL_SIZE = 25;

    public GUI(GameModel model, GameController controller) {
        super("Minesweeper");

        this.model = model;
        this.controller = controller;

        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(false);

        JPanel rootPanel = new JPanel(new GridBagLayout());
        setContentPane(rootPanel);

        createMenuBar();
        createStatusPanel(rootPanel);

        createBoardPanel(model.getRows(), model.getCols());

        pack();
        setLocationRelativeTo(null);
    }

    private void createMenuBar() {
        JMenuBar menuBar = new JMenuBar();

        JMenu gameMenu = new JMenu("Game");
        gameMenu.add(createMenuItem("New Game", controller::handleNewGame));
        gameMenu.addSeparator();
        gameMenu.add(createMenuItem("High Scores", () -> showHighscores(controller.getHighscores())));
        gameMenu.addSeparator();
        gameMenu.add(createMenuItem("Save", controller::handleSave));
        gameMenu.addSeparator();
        gameMenu.add(createMenuItem("Load", controller::handleLoad));
        gameMenu.addSeparator();
        gameMenu.add(createMenuItem("Exit", controller::handleExit));

        JMenu helpMenu = new JMenu("Help");
        helpMenu.add(createMenuItem("About", this::showAbout));

        menuBar.add(gameMenu);
        menuBar.add(helpMenu);
        setJMenuBar(menuBar);
    }

    private JMenuItem createMenuItem(String text, Runnable action) {
        JMenuItem item = new JMenuItem(text);
        item.addActionListener(e -> action.run());
        return item;
    }

    private void createStatusPanel(JPanel rootPanel) {
        statusPanel = new StatusPanel();
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.weightx = 1.0;
        gbc.weighty = 0.0;
        gbc.fill = GridBagConstraints.BOTH;
        gbc.insets = new Insets(4, 4, 4, 4);
        rootPanel.add(statusPanel, gbc);
    }

    private void createBoardPanel(int rows, int cols) {
        JPanel rootPanel = (JPanel) getContentPane();

        if (boardPanel != null) {
            rootPanel.remove(boardPanel);
        }

        boardPanel = new BoardPanel(rows, cols, CELL_SIZE,
                (r, c) -> controller.handleCellTouch(r, c),
                (r, c) -> controller.handleFlagToggle(r, c));

        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = 1;
        gbc.weightx = 1.0;
        gbc.weighty = 1.0;
        gbc.fill = GridBagConstraints.BOTH;
        gbc.insets = new Insets(4, 4, 4, 4);
        rootPanel.add(boardPanel, gbc);

        rootPanel.revalidate();
        rootPanel.repaint();
    }

    @Override
    public void fullUpdate(GameModel model) {
        this.model = model;
        int rows = model.getRows();
        int cols = model.getCols();

        if (boardPanel == null || boardPanel.getRows() != rows
                || boardPanel.getCols() != cols) {
            createBoardPanel(rows, cols);
        }

        boardPanel.updateAll(model);
        statusPanel.update(model);
        pack();
    }

    @Override
    public void cellUpdate(CellUpdate update) {
        if (boardPanel != null) {
            boardPanel.cellUpdate(update);
        }
        if (model != null) {
            statusPanel.update(model);
        }
    }

    @Override
    public void updateMinesCount(int remainingMines) {
        statusPanel.updateMinesCount(remainingMines);
    }

    @Override
    public void updateTimer(int seconds) {
        statusPanel.updateTimer(seconds);
    }

    @Override
    public void showGameOver(boolean won, int timeInSeconds) {
        String title = won ? "Congratulations!" : "Game Over";
        String message = won
                ? "You won!\nTime: " + timeInSeconds + " seconds"
                : "You hit a mine!";
        int type = won ? JOptionPane.INFORMATION_MESSAGE : JOptionPane.ERROR_MESSAGE;
        JOptionPane.showMessageDialog(this, message, title, type);
    }

    @Override
    public void showHighscorePrompt() {
        JOptionPane.showMessageDialog(this,
                "New Highscore!",
                "Highscore", JOptionPane.INFORMATION_MESSAGE);
    }

    @Override
    public void showHighscores(List<Highscore> highscores) {
        if (highscores.isEmpty()) {
            JOptionPane.showMessageDialog(this, "No records yet.",
                    "High Scores", JOptionPane.INFORMATION_MESSAGE);
            return;
        }

        StringBuilder sb = new StringBuilder();
        sb.append(String.format("%-4s %-15s %-8s %-15s%n",
                "№", "Name", "Time", "Difficulty"));
        sb.append("─".repeat(45)).append("\n");

        int i = 1;
        for (Highscore hs : highscores) {
            sb.append(String.format("%-4d %-15s %-6d %-15s%n",
                    i++, hs.getPlayerName(), hs.getTimeInSeconds(),
                    hs.getDifficulty()));
        }

        JTextArea textArea = new JTextArea(sb.toString());
        textArea.setEditable(false);
        textArea.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 12));
        JScrollPane scrollPane = new JScrollPane(textArea);
        scrollPane.setPreferredSize(new Dimension(450, 300));

        JOptionPane.showMessageDialog(this, scrollPane,
                "High Scores", JOptionPane.INFORMATION_MESSAGE);
    }

    @Override
    public void showAbout() {
        JOptionPane.showMessageDialog(this,
                """
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
                        """,
                "About", JOptionPane.INFORMATION_MESSAGE);
    }

    @Override
    public void showError(String message) {
        JOptionPane.showMessageDialog(this, message,
                "Error", JOptionPane.ERROR_MESSAGE);
    }

    @Override
    public void showMessage(String message) {
        JOptionPane.showMessageDialog(this, message,
                "Info", JOptionPane.INFORMATION_MESSAGE);
    }

    @Override
    public String requestPlayerName() {
        String name = (String) JOptionPane.showInputDialog(
                null,
                "Congratulations! New highscore!\nEnter your name:",
                "Highscore",
                JOptionPane.PLAIN_MESSAGE,
                null, null,
                System.getProperty("user.name"));
        return (name != null && !name.trim().isEmpty()) ? name.trim() : null;
    }

    @Override
    public void propertyChange(PropertyChangeEvent evt) {
        switch (evt.getPropertyName()) {
            case GameModel.BOARD_CHANGE -> SwingUtilities.invokeLater(() -> fullUpdate(model));
            case GameModel.CELL_REVEAL -> SwingUtilities.invokeLater(() -> {
                CellUpdate update = (CellUpdate) evt.getNewValue();
                cellUpdate(update);
            });
            case GameModel.FLAG_TOGGLE -> SwingUtilities.invokeLater(() -> {
                CellUpdate update = (CellUpdate) evt.getNewValue();
                cellUpdate(update);
            });
            case GameModel.MINES_UPDATE -> SwingUtilities.invokeLater(() -> {
                int mines = (int) evt.getNewValue();
                updateMinesCount(mines);
            });
            case GameModel.TIMER_UPDATE -> SwingUtilities.invokeLater(() -> {
                int seconds = (int) evt.getNewValue();
                updateTimer(seconds);
            });
            case GameModel.STATE_CHANGE -> SwingUtilities.invokeLater(() -> {
                if (!model.isGameOver()) {
                    return;
                }

                fullUpdate(model);

                boolean won = model.isWin();
                showGameOver(won, model.getElapsedSeconds());

                if (won && controller.isHighscore()) {
                    String name = requestPlayerName();
                    if (name != null && !name.isEmpty()) {
                        controller.saveHighscore(name);
                    }
                }
            });
        }
    }
}
