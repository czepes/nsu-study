package ru.nsu.minesweeper.view;

import java.beans.PropertyChangeListener;
import java.util.List;

import ru.nsu.minesweeper.model.CellUpdate;
import ru.nsu.minesweeper.model.GameModel;
import ru.nsu.minesweeper.controller.Highscore;

public interface GameView extends PropertyChangeListener {

    void fullUpdate(GameModel model);

    void cellUpdate(CellUpdate update);

    void updateMinesCount(int remainingMines);

    void updateTimer(int seconds);

    void showGameOver(boolean won, int seconds);

    void showHighscorePrompt();

    void showHighscores(List<Highscore> highscores);

    void showAbout();

    void showMessage(String msg);

    void showError(String msg);

    String requestPlayerName();
}
