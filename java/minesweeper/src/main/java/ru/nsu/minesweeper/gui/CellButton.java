package ru.nsu.minesweeper.gui;

import ru.nsu.minesweeper.error.CellException;
import ru.nsu.minesweeper.model.*;
import ru.nsu.minesweeper.resources.Resources;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;

public class CellButton extends JButton {
    private final int row;
    private final int col;
    private Runnable onLeftClick;
    private Runnable onRightClick;

    private boolean isRevealedState = false;
    private boolean isFlaggedState = false;
    private boolean isMineState = false;
    private int adjacentMinesState = 0;

    private final int displaySize;

    public CellButton(int row, int col, int displaySize) {
        this.row = row;
        this.col = col;
        this.displaySize = displaySize;

        setPreferredSize(new Dimension(displaySize, displaySize));
        setMinimumSize(new Dimension(displaySize, displaySize));
        setMaximumSize(new Dimension(displaySize, displaySize));

        setMargin(new Insets(0, 0, 0, 0));
        setFocusPainted(false);
        setContentAreaFilled(false);
        setBorder(BorderFactory.createEmptyBorder());

        setIcon(createIcon(false, false, false, 0));

        addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) {
                if (SwingUtilities.isRightMouseButton(e)) {
                    if (onRightClick != null) {
                        onRightClick.run();
                    }
                }
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                if (SwingUtilities.isRightMouseButton(e)) {
                    e.consume();
                }
            }
        });
    }

    public void setOnLeftClick(Runnable callback) {
        this.onLeftClick = callback;
        addActionListener(e -> {
            if (onLeftClick != null) {
                onLeftClick.run();
            }
        });
    }

    public void setOnRightClick(Runnable callback) {
        this.onRightClick = callback;
    }

    public int getRow() {
        return row;
    }

    public int getCol() {
        return col;
    }

    public void updateFromModel(GameModel model) {
        try {
            setAppearance(model.isCellRevealed(row, col),
                    model.isCellFlagged(row, col),
                    model.isCellMine(row, col),
                    model.getCellAdjacentMines(row, col));
        } catch (CellException e) {
            setText("?");
        }
    }

    public void updateFromCellUpdate(CellUpdate update) {
        setAppearance(update.isRevealed(), update.isFlagged(),
                update.isMine(), update.adjacentMines());
    }

    private void setAppearance(boolean revealed, boolean flagged,
            boolean isMine, int adjacent) {
        this.isRevealedState = revealed;
        this.isFlaggedState = flagged;
        this.isMineState = isMine;
        this.adjacentMinesState = adjacent;

        applyIcon();
    }

    private void applyIcon() {
        ImageIcon icon = createIcon(isRevealedState, isFlaggedState,
                isMineState, adjacentMinesState);
        setIcon(icon);
        setDisabledIcon(icon);
        setEnabled(!isRevealedState);
    }

    private ImageIcon createIcon(boolean revealed, boolean flagged,
            boolean isMine, int adjacent) {
        BufferedImage composite = Resources.getInstance()
                .getCellComposite(revealed, flagged, isMine, adjacent,
                        displaySize, displaySize);
        return new ImageIcon(composite);
    }
}
