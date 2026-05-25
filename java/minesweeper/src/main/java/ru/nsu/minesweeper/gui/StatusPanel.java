package ru.nsu.minesweeper.gui;

import ru.nsu.minesweeper.model.GameModel;
import ru.nsu.minesweeper.model.GameState;

import javax.swing.*;
import javax.swing.border.EmptyBorder;
import java.awt.*;

public class StatusPanel extends JPanel {
    private final JLabel minesLabel;
    private final JLabel timerLabel;
    private final JLabel stateLabel;

    public StatusPanel() {
        setLayout(new GridLayout(1, 3, 10, 0));
        setBorder(new EmptyBorder(5, 10, 5, 10));
        setBackground(new Color(0xEE, 0xEE, 0xEE));

        minesLabel = new JLabel("💣 10", SwingConstants.CENTER);
        minesLabel.setFont(new Font("Arial", Font.BOLD, 16));

        stateLabel = new JLabel("Ready", SwingConstants.CENTER);
        stateLabel.setFont(new Font("Arial", Font.BOLD, 14));

        timerLabel = new JLabel("⏱ 0", SwingConstants.CENTER);
        timerLabel.setFont(new Font("Arial", Font.BOLD, 16));

        add(minesLabel);
        add(stateLabel);
        add(timerLabel);
    }

    public void update(GameModel model) {
        updateMinesCount(model.getRemainingMines());
        updateTimer(model.getElapsedSeconds());
        updateState(model.getState());
    }

    public void updateMinesCount(int remainingMines) {
        SwingUtilities.invokeLater(() -> minesLabel.setText("💣 " + remainingMines));
    }

    public void updateTimer(int seconds) {
        SwingUtilities.invokeLater(() -> timerLabel.setText("⏱ " + seconds));
    }

    public void updateState(GameState state) {
        SwingUtilities.invokeLater(() -> {
            String text = switch (state) {
                case PRE -> "Ready";
                case ACTIVE -> "Playing";
                case WON -> "🏆 WON!";
                case LOST -> "💥 LOST!";
            };
            stateLabel.setText(text);
        });
    }
}
