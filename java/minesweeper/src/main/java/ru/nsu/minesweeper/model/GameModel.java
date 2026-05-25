package ru.nsu.minesweeper.model;

import java.util.HashSet;
import java.util.Set;
import java.util.Timer;
import java.util.TimerTask;

import ru.nsu.minesweeper.error.BoardException;
import ru.nsu.minesweeper.error.CellException;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

public class GameModel {
    private Board board;
    private int elapsedSeconds;
    private GameState state;

    protected Timer timer;

    public static final String CELL_REVEAL = "cellRevealed";
    public static final String FLAG_TOGGLE = "flagToggle";
    public static final String BOARD_CHANGE = "boardChange";
    public static final String STATE_CHANGE = "stateChange";
    public static final String TIMER_UPDATE = "timerUpdate";
    public static final String MINES_UPDATE = "minesUpdate";

    private final PropertyChangeSupport pcs = new PropertyChangeSupport(this);

    public GameModel() {
        this(9, 9, 10);
    }

    public GameModel(int rows, int cols, int mines) {
        newGame(rows, cols, mines);
    }

    /**
     * Game initialization
     */

    public void endGame() {
        stopTimer();
    }

    public void newGame(int rows, int cols, int mines) throws BoardException {
        if (rows < 1 || cols < 1 || mines < 0 || mines >= rows * cols) {
            throw new BoardException("", rows, cols, mines);
        }

        this.board = new Board(rows, cols, mines);

        newGame();
    }

    public void newGame() {
        this.board = new Board(board);
        state = GameState.PRE;
        elapsedSeconds = 0;

        pcs.firePropertyChange(BOARD_CHANGE, null, null);
    }

    /**
     * Game Logic
     */

    private Set<Coords> getAdjacentCells(int row, int col) {
        Set<Coords> adjacentCells = new HashSet<>();

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) {
                    continue;
                }

                int adjRow = row + i;
                int adjCol = col + j;

                if (board.isValidCell(adjRow, adjCol)) {
                    adjacentCells.add(new Coords(adjRow, adjCol));
                }
            }
        }

        return adjacentCells;
    }

    public void revealCell(int row, int col) {
        if (!board.isValidCell(row, col) || isGameOver()) {
            return;
        }

        if (isNotStarted()) {
            board.placeMines(row, col);
            state = GameState.ACTIVE;
            startTimer();
        }

        if (board.isCellFlagged(row, col) || board.isCellRevealed(row, col)) {
            return;
        }

        if (board.isCellMine(row, col)) {
            stopTimer();
            board.revealCell(row, col);
            board.revealAllMines();
            state = GameState.LOST;
            pcs.firePropertyChange(STATE_CHANGE, null, GameState.LOST);
            return;
        }

        floodReveal(row, col);
        pcs.firePropertyChange(BOARD_CHANGE, null, null);

        if (board.checkWin()) {
            stopTimer();
            state = GameState.WON;
            pcs.firePropertyChange(STATE_CHANGE, null, GameState.WON);
        }
    }

    // Reveal all adjcent non-mine cells recursively
    private void floodReveal(int row, int col) {
        if (!board.isValidCell(row, col)) {
            return;
        }

        if (board.isCellFlagged(row, col) || board.isCellRevealed(row, col) || board.isCellMine(row, col)) {
            return;
        }

        board.revealCell(row, col);

        if (board.getCellAdjacentMines(row, col) != 0) {
            return;
        }

        for (Coords adj : getAdjacentCells(row, col)) {
            floodReveal(adj.row(), adj.col());
        }
    }

    public void toggleFlag(int row, int col) {
        if (!board.isValidCell(row, col) || isGameOver()) {
            return;
        }

        if (board.isCellRevealed(row, col)) {
            return;
        }

        if (board.getRemainingMines() == 0 && !board.isCellFlagged(row, col)) {
            return;
        }

        board.toggleCellFlag(row, col);

        pcs.firePropertyChange(FLAG_TOGGLE, null,
                new CellUpdate(row, col,
                        board.isCellRevealed(row, col), board.isCellFlagged(row, col),
                        board.isCellMine(row, col), board.getCellAdjacentMines(row, col)));
        pcs.firePropertyChange(MINES_UPDATE, null, board.getRemainingMines());
    }

    /**
     * Timer in a separate thread
     */

    private void startTimer() {
        timer = new Timer("Minesweeper-Timer", true);
        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                tick();
            }
        }, 1000, 1000);
    }

    private void stopTimer() {
        if (timer != null) {
            timer.cancel();
            timer = null;
        }
    }

    private void tick() {
        if (!isRunning()) {
            return;
        }
        elapsedSeconds++;
        pcs.firePropertyChange(TIMER_UPDATE, null, elapsedSeconds);
    }

    /*
     * Property listeners
     * Model notifies View
     */

    public void addObserver(PropertyChangeListener listener) {
        pcs.addPropertyChangeListener(listener);
    }

    public void removeObserver(PropertyChangeListener listener) {
        pcs.removePropertyChangeListener(listener);
    }

    /**
     * Getters
     */

    public int getRows() {
        return board.getRows();
    }

    public int getCols() {
        return board.getCols();
    }

    public int getTotalMines() {
        return board.getTotalMines();
    }

    public int getRemainingMines() {
        return board.getRemainingMines();
    }

    public boolean isCellRevealed(int row, int col) throws CellException {
        return board.isCellRevealed(row, col);
    }

    public boolean isCellMine(int row, int col) throws CellException {
        if (!board.isCellRevealed(row, col)) {
            return false;
        }
        return board.isCellMine(row, col);
    }

    public boolean isCellFlagged(int row, int col) throws CellException {
        return board.isCellFlagged(row, col);
    }

    public int getCellAdjacentMines(int row, int col) throws CellException {
        return board.getCellAdjacentMines(row, col);
    }

    public int getElapsedSeconds() {
        return elapsedSeconds;
    }

    public GameState getState() {
        return state;
    }

    public boolean isNotStarted() {
        return state == GameState.PRE;
    }

    public boolean isRunning() {
        return state == GameState.ACTIVE;
    }

    public boolean isGameOver() {
        return state == GameState.WON || state == GameState.LOST;
    }

    public boolean isWin() {
        return state == GameState.WON;
    }

    /**
     * Save & Load
     */

    public GameMemento createMemento() {
        return new GameMemento(board, state, elapsedSeconds);
    }

    public void restoreFromMemento(GameMemento memento) {
        stopTimer();

        this.board = memento.board;
        this.state = memento.state;
        this.elapsedSeconds = memento.elapsedSeconds;

        if (isRunning()) {
            startTimer();
        }

        pcs.firePropertyChange(BOARD_CHANGE, null, null);
    }
}
