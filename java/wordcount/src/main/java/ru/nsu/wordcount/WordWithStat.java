package ru.nsu.wordcount;

public class WordWithStat extends WordStat {
    protected String word;

    public WordWithStat(String word, WordStat stat) {
        super(stat.getCount(), stat.getFrequency());
        this.word = word;
    }

    public WordWithStat(String word, int count, double frequency) {
        super(count, frequency);
        this.word = word;
    }

    public String getWord() {
        return word;
    }
}
