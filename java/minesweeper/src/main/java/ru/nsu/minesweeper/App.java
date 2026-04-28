package ru.nsu.minesweeper;

import ru.nsu.minesweeper.controller.GameController;
import ru.nsu.minesweeper.error.LoadException;
import ru.nsu.minesweeper.gui.GUISession;
import ru.nsu.minesweeper.model.GameModel;
import ru.nsu.minesweeper.session.GameSession;
import ru.nsu.minesweeper.tui.TUISession;

public class App {
    public static void main(String[] args) {
        if (args.length == 0) {
            System.out.println("MINESWEEPER LAUNCHER");
            System.out.println("Choose interface:");
            System.out.println("1. Text (console) UI");
            System.out.println("2. Graphic (Swing) UI");
            return;
        }

        GameModel model = new GameModel();
        GameController controller = null;
        GameSession session = null;

        try {
            controller = new GameController(model);
        } catch (LoadException e) {
            System.err.println(e.getMessage());
            controller = new GameController(model);
        }

        switch (args[0]) {
            default:
            case "1":
                session = new TUISession(model, controller);
                break;
            case "2":
                session = new GUISession(model, controller);
                break;
            case "3":
                TUISession st = new TUISession(model, controller);
                GUISession sg = new GUISession(model, controller);
                st.start();
                sg.start();
                return;
        }

        session.start();
    }
}
