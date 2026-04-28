package ru.nsu.minesweeper.controller;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

import ru.nsu.minesweeper.error.LoadException;
import ru.nsu.minesweeper.error.SaveException;
import ru.nsu.minesweeper.model.GameMemento;
import ru.nsu.minesweeper.model.GameModel;

public class SaveManager {
    private static final String SAVE_FILE = "data/minesweeper-save.dat";

    public void save(GameModel model) throws SaveException {
        try (ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(SAVE_FILE))) {
            oos.writeObject(model.createMemento());
        } catch (IOException e) {
            throw new SaveException("game", e.getMessage());
        }
    }

    public void load(GameModel model) throws LoadException {
        File file = new File(SAVE_FILE);
        if (!file.exists()) {
            throw new LoadException("game", "No saved game found");
        }

        try (ObjectInputStream ois = new ObjectInputStream(new FileInputStream(file))) {
            Object obj = ois.readObject();
            if (obj instanceof GameMemento) {
                GameMemento memento = (GameMemento) obj;
                model.restoreFromMemento(memento);
            } else {
                file.delete();
                throw new LoadException("game", "Invalid saved game format");
            }
        } catch (IOException | ClassNotFoundException e) {
            file.delete();
            throw new LoadException("game", e.getMessage());
        }

    }
}
