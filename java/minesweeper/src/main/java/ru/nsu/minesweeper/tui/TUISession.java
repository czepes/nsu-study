package ru.nsu.minesweeper.tui;

import ru.nsu.minesweeper.controller.GameController;
import ru.nsu.minesweeper.error.BoardException;
import ru.nsu.minesweeper.error.LoadException;
import ru.nsu.minesweeper.error.SaveException;
import ru.nsu.minesweeper.model.GameModel;
import ru.nsu.minesweeper.session.GameSession;

public class TUISession implements GameSession {
    private TUI tui;
    private GameModel model;
    private GameController controller;
    private boolean running = false;

    public TUISession(GameModel model) {
        this(model, new GameController(model));
    }

    public TUISession(GameModel model, GameController controller) {
        this.tui = new TUI(model, controller);
        this.model = model;
        this.controller = controller;
        model.addObserver(tui);
    }

    public void start() {
        if (running) {
            return;
        }
        running = true;
        Thread thread = new Thread(() -> {
            controller.handleNewGame();
            gameLoop();
        }, "TUI-Thread");
        thread.setDaemon(false);
        thread.start();
    }

    private void gameLoop() {
        while (running) {
            System.out.print("> ");
            String input = tui.next().toLowerCase();
            parse(input);
        }
    }

    private void parse(String input) {
        if (input.isEmpty()) {
            tui.printGameField();
            return;
        }

        String[] args = input.split("\\s+");
        String command = args[0];

        switch (command) {
            case "o":
            case "open":
                parseTouch(args, true);
                return;
            case "f":
            case "flag":
                parseTouch(args, false);
                return;
            case "n":
            case "new":
                parseNewGame(args);
                return;
            case "h":
            case "highscores":
                parseHighscores(args);
                return;
            case "a":
            case "about":
                parseAbout(args);
                return;
            case "s":
            case "save":
                parseSave(args);
                return;
            case "l":
            case "load":
                parseLoad(args);
                return;
            case "e":
            case "exit":
                handleExit();
                return;
            case "help":
                parseHelp();
                return;
            default:
                parseUnknown();
                return;
        }
    }

    private void handleExit() {
        running = false;
        tui.showMessage("THANK YOU FOR PLAYING!");
        model.removeObserver(tui);
        controller.handleExit();
    }

    private void parseTouch(String[] args, boolean open) {
        if (args.length < 3) {
            tui.showMessage("Usage: " + args[0] + " <row> <col>\n");
            return;
        }

        int rows = model.getRows();
        int cols = model.getCols();

        try {
            int row = Integer.parseInt(args[1]);
            int col = Integer.parseInt(args[2]);

            if (row < 0 || row >= rows || col < 0 || col >= cols) {
                tui.showError("Wrong coordinates. "
                        + "Allowed region: rows in [0, " + (rows - 1) + "],"
                        + " cols in [0, " + (cols - 1) + "]\n");
                return;
            }

            if (open) {
                controller.handleCellTouch(row, col);
            } else {
                controller.handleFlagToggle(row, col);
            }
        } catch (NumberFormatException e) {
            tui.showError("Wrong coordinates format. Use integer numbers.\n");
        }
    }

    private void parseNewGame(String[] args) {
        if (args.length == 1) {
            controller.handleNewGame();
        } else if (args.length >= 4) {
            try {
                int newRows = Integer.parseInt(args[1]);
                int newCols = Integer.parseInt(args[2]);
                int newMines = Integer.parseInt(args[3]);
                controller.handleNewGame(newRows, newCols, newMines);
            } catch (NumberFormatException e) {
                tui.showError("Wrong parameters format. Usage: new <rows> <cols> <mines>\n");
            } catch (BoardException e) {
                tui.showError(e.getMessage() + "\n");
            }
        } else {
            tui.showMessage("Usage:\n"
                    + "  new             - new game with current parameters\n"
                    + "  new <R> <C> <M> - new game with RxC field and M mines\n");
        }
    }

    private void parseHighscores(String[] __) {
        try {
            tui.showHighscores(controller.getHighscores());
        } catch (LoadException e) {
            tui.showError(e.getMessage() + "\n");
        }

    }

    private void parseAbout(String[] __) {
        tui.showAbout();
    }

    private void parseSave(String[] __) {
        try {
            controller.handleSave();
        } catch (SaveException e) {
            tui.showError(e.getMessage() + "\n");
        }
    }

    private void parseLoad(String[] __) {
        try {
            controller.handleLoad();
        } catch (LoadException e) {
            tui.showError(e.getMessage() + "\n");
        }
    }

    private void parseHelp() {
        tui.showMessage("Commands: open | flag | new | highscores | about | save | load | exit | help\n");
    }

    private void parseUnknown() {
        tui.showMessage("Unknown command. Enter 'help' for commands list.\n");
    }
}
