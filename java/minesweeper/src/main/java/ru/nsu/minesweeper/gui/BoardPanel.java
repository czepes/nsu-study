package ru.nsu.minesweeper.gui;

import ru.nsu.minesweeper.model.*;

import javax.swing.*;
import java.awt.*;

public class BoardPanel extends JPanel {
    private final CellButton[][] buttons;
    private final int rows;
    private final int cols;

    public BoardPanel(int rows, int cols, int cellSize,
            CellClickCallback onLeftClick,
            CellClickCallback onRightClick) {
        this.rows = rows;
        this.cols = cols;
        this.buttons = new CellButton[rows][cols];

        setLayout(new GridLayout(rows, cols, 0, 0));
        setBackground(Color.DARK_GRAY);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                CellButton btn = new CellButton(i, j, cellSize);

                final int r = i, c = j;
                btn.setOnLeftClick(() -> onLeftClick.onClick(r, c));
                btn.setOnRightClick(() -> onRightClick.onClick(r, c));

                buttons[i][j] = btn;
                add(btn);
            }
        }
    }

    @FunctionalInterface
    public interface CellClickCallback {
        void onClick(int row, int col);
    }

    public int getRows() {
        return rows;
    }

    public int getCols() {
        return cols;
    }

    public void updateAll(GameModel model) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                buttons[i][j].updateFromModel(model);
            }
        }
    }

    public void cellUpdate(CellUpdate update) {
        int r = update.row();
        int c = update.col();
        if (r >= 0 && r < rows && c >= 0 && c < cols) {
            buttons[r][c].updateFromCellUpdate(update);
        }
    }
}
