package ru.nsu.wordcount;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.HashMap;

public class WordsCounter {
    private File infile;

    public WordsCounter(File infile) {
        this.infile = infile;
    }

    public WordsStats read() {
        int total = 0;
        HashMap<String, Integer> countMap = new HashMap<>();

        try (FileInputStream in = new FileInputStream(infile);
                Reader reader = new InputStreamReader(in)) {
            StringBuilder sb = new StringBuilder("");
            int code = -1;

            while ((code = reader.read()) != -1) {
                char ch = (char) code;
                if (Character.isLetterOrDigit(ch)) {
                    sb.append(ch);
                } else if (sb.length() > 0) {
                    String word = sb.toString();
                    countMap.compute(word, (__, count) -> count != null ? count + 1 : 1);
                    total++;
                    sb.setLength(0);
                }
            }
        } catch (IOException e) {
            System.out.println("[WordCounter] Error while reading " + infile + ": " + e.getLocalizedMessage());
        }

        return new WordsStats(countMap, total);
    }
}
