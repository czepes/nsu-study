package ru.nsu.wordcount;

public class WordStat {
    protected int count;
    protected double frequency;

    public WordStat(int count, double frequency) {
        this.count = count;
        this.frequency = frequency;
    }

    public int getCount() {
        return count;
    }

    public double getFrequency() {
        return frequency;
    }

    public void update(int count, double frequency) {
        this.count = count;
        this.frequency = frequency;
    }
}
