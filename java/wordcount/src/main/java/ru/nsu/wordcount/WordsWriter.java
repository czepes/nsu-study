package ru.nsu.wordcount;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Map;

public class WordsWriter {
    File outfile;

    public WordsWriter(File outfile) {
        this.outfile = outfile;
    }

    public void write(Collection<WordWithStat> wordsStat) {
        try (FileOutputStream out = new FileOutputStream(outfile);
                Writer writer = new OutputStreamWriter(out)) {
            writer.write("word;count;frequency\n");
            for (WordWithStat wordStat : wordsStat) {
                writer.write(wordStat.getWord() + ";"
                        + wordStat.getCount() + ";"
                        + (new DecimalFormat("#.##%")).format(wordStat.getFrequency())
                        + "\n");
            }
        } catch (IOException e) {
            System.err.println("[WordWriter] Failed to write to " + outfile + ": " + e.getLocalizedMessage());
        }
    }

    public void write(Map<String, WordStat> frequencyMap) {
        ArrayList<WordWithStat> wordsStat = new ArrayList<>();
        frequencyMap.keySet().forEach((word) -> wordsStat.add(new WordWithStat(word, frequencyMap.get(word))));
        write(wordsStat);
    }
}
