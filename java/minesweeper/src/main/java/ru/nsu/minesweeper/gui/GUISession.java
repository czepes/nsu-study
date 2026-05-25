package ru.nsu.minesweeper.gui;

import javax.swing.SwingUtilities;

import ru.nsu.minesweeper.controller.GameController;
import ru.nsu.minesweeper.model.GameModel;
import ru.nsu.minesweeper.session.GameSession;

public class GUISession implements GameSession {
    private GUI gui;
    private GameController controller;
    private boolean running = false;

    public GUISession(GameModel model) {
        this(model, new GameController(model));
    }

    public GUISession(GameModel model, GameController controller) {
        this.gui = new GUI(model, controller);
        this.controller = controller;
        model.addObserver(gui);
    }

    public void start() {
        if (running) {
            return;
        }
        running = true;
        SwingUtilities.invokeLater(() -> {
            gui.setVisible(true);
            controller.handleNewGame();
        });
    }
}
