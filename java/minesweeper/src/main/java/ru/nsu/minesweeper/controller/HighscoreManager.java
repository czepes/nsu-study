package ru.nsu.minesweeper.controller;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import ru.nsu.minesweeper.error.LoadException;
import ru.nsu.minesweeper.error.SaveException;

public class HighscoreManager {
    private static final String SCORES_FILE = "data/minesweeper-score.dat";
    private static final int MAX_SCORES = 10;

    private List<Highscore> highscores;

    public HighscoreManager() {
        highscores = new ArrayList<>();
        loadScores();
    }

    public boolean isHighscore(int time) {
        if (highscores.size() < MAX_SCORES) {
            return true;
        }
        return time < highscores.get(highscores.size() - 1).getTimeInSeconds();
    }

    public void addScore(String playerName, int timeInSeconds, int rows, int cols, int mines) {
        Highscore score = new Highscore(playerName, timeInSeconds, rows, cols, mines);
        highscores.add(score);
        Collections.sort(highscores, Comparator.comparingInt(Highscore::getTimeInSeconds));

        if (highscores.size() > MAX_SCORES) {
            highscores = highscores.subList(0, MAX_SCORES);
        }

        saveScores();
    }

    public List<Highscore> getHighscores() {
        return Collections.unmodifiableList(highscores);
    }

    /**
     * Scores file IO
     */
    @SuppressWarnings("unchecked")
    private void loadScores() throws LoadException {
        File file = new File(SCORES_FILE);
        if (!file.exists()) {
            return;
        }

        try (ObjectInputStream ois = new ObjectInputStream(new FileInputStream(file))) {
            Object obj = ois.readObject();
            if (obj instanceof List<?> list) {
                boolean validCast = list.stream().allMatch(item -> item instanceof Highscore);
                if (validCast) {
                    highscores = (List<Highscore>) obj;
                }
            } else {
                file.delete();
                throw new LoadException("highscore table", "Invalid data format");
            }
        } catch (IOException | ClassNotFoundException e) {
            highscores = new ArrayList<>();
            file.delete();
            throw new LoadException("highscore table", e.getMessage());
        }
    }

    private void saveScores() throws SaveException {
        try (ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(SCORES_FILE))) {
            oos.writeObject(highscores);
        } catch (IOException e) {
            throw new SaveException("highscore table", e.getMessage());
        }
    }
}
