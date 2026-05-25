package ru.nsu.minesweeper.resources;

import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

public class Resources {
    private static Resources instance;

    public static final String CELL_CLOSED = "masked";
    public static final String CELL_OPEN = "revealed";
    public static final String NUMBER_1 = "1";
    public static final String NUMBER_2 = "2";
    public static final String NUMBER_3 = "3";
    public static final String NUMBER_4 = "4";
    public static final String NUMBER_5 = "5";
    public static final String NUMBER_6 = "6";
    public static final String NUMBER_7 = "7";
    public static final String NUMBER_8 = "8";
    public static final String MINE = "mine";
    public static final String FLAG = "flag";

    private final Map<String, BufferedImage> images = new HashMap<>();

    private Resources() {
        loadAll();
    }

    public static synchronized Resources getInstance() {
        if (instance == null) {
            instance = new Resources();
        }
        return instance;
    }

    private void loadAll() {
        loadImage(CELL_CLOSED);
        loadImage(CELL_OPEN);
        loadImage(NUMBER_1);
        loadImage(NUMBER_2);
        loadImage(NUMBER_3);
        loadImage(NUMBER_4);
        loadImage(NUMBER_5);
        loadImage(NUMBER_6);
        loadImage(NUMBER_7);
        loadImage(NUMBER_8);
        loadImage(MINE);
        loadImage(FLAG);
    }

    private void loadImage(String name) {
        String path = "/ru/nsu/minesweeper/resources/" + name + ".png";
        URL url = getClass().getResource(path);

        images.put(name, new ImageIcon(url).getImage() instanceof BufferedImage bi
                ? bi
                : toBufferedImage(new ImageIcon(url).getImage()));
    }

    private BufferedImage toBufferedImage(Image img) {
        BufferedImage bi = new BufferedImage(
                img.getWidth(null), img.getHeight(null), BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = bi.createGraphics();
        g.drawImage(img, 0, 0, null);
        g.dispose();
        return bi;
    }

    public BufferedImage getCellComposite(boolean revealed, boolean flagged,
            boolean isMine, int adjacentMines,
            int targetWidth, int targetHeight) {
        BufferedImage background;
        if (revealed) {
            background = images.get(CELL_OPEN);
        } else {
            background = images.get(CELL_CLOSED);
        }

        BufferedImage composite = new BufferedImage(
                background.getWidth(), background.getHeight(), BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = composite.createGraphics();
        g.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_SPEED);

        g.drawImage(background, 0, 0, null);

        BufferedImage overlay = null;
        if (revealed) {
            if (isMine) {
                overlay = images.get(MINE);
            } else if (adjacentMines >= 1 && adjacentMines <= 8) {
                String name = String.valueOf(adjacentMines);
                overlay = images.get(name);
            }
        } else if (flagged) {
            overlay = images.get(FLAG);
        }

        if (overlay != null) {
            int x = (background.getWidth() - overlay.getWidth()) / 2;
            int y = (background.getHeight() - overlay.getHeight()) / 2;
            g.drawImage(overlay, x, y, null);
        }

        g.dispose();

        if (targetWidth != background.getWidth() || targetHeight != background.getHeight()) {
            Image scaled = composite.getScaledInstance(targetWidth, targetHeight, Image.SCALE_SMOOTH);
            BufferedImage scaledBI = new BufferedImage(targetWidth, targetHeight, BufferedImage.TYPE_INT_ARGB);
            Graphics2D sg = scaledBI.createGraphics();
            sg.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
            sg.drawImage(scaled, 0, 0, null);
            sg.dispose();
            return scaledBI;
        }

        return composite;
    }

    public BufferedImage getImage(String name) {
        return images.get(name);
    }
}
