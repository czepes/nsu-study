package ru.nsu.wordcount;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

public class WordsStats {
    private Map<String, WordStat> frequencyMap;
    private int total;

    public WordsStats(Map<String, Integer> countMap, int total) {
        frequencyMap = new HashMap<>();
        this.total = total;

        countMap.keySet().forEach((word) -> {
            int count = countMap.get(word);
            frequencyMap.put(word, new WordStat(count, count / (double) total));
        });
    }

    public int getTotal() {
        return total;
    }

    public Map<String, WordStat> getWords() {
        return frequencyMap;
    }

    public Collection<WordWithStat> getSortedWords(boolean descend) {
        ArrayList<WordWithStat> sorted = new ArrayList<>();

        frequencyMap.keySet().forEach((word) -> {
            WordStat stat = frequencyMap.get(word);
            sorted.add(new WordWithStat(word, stat));
        });

        sorted.sort((ws1, ws2) -> (ws1.getCount() - ws2.getCount()) * (descend ? -1 : 1));

        return sorted;
    }

    public Collection<WordWithStat> getSortedWords() {
        return getSortedWords(true);
    }
}
