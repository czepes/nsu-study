package ru.nsu.wordcount;

import java.io.File;

public class App {
    private static String defaultOutputFilename = "wordcount.csv";

    public static void help() {
        System.out.println("./wordcount [in.txt] [out.csv = " + defaultOutputFilename + "] [-a] [-h|--help]\n"
                + "[in.txt] - path to input file, must end with .txt\n"
                + "[out.csv] - path to output file, must end with .csv, defaults to " + defaultOutputFilename + "\n"
                + "[-a] - sorted in ascending order, default order is descending\n"
                + "[-h | --help] - print this message\n");
    }

    public static void main(String[] args) {
        File infile = null;
        File outfile = null;
        boolean descend = true;

        if (args.length < 1) {
            System.err.println("Missing input file");
            return;
        }

        for (String arg : args) {
            if (arg.equals("-h") || arg.equals("--help")) {
                help();
            } else if (arg.equals("-a")) {
                descend = false;
            } else if (arg.endsWith(".txt")) {
                infile = new File(arg);
            } else if (arg.endsWith(".csv")) {
                outfile = new File(arg);
            } else {
                System.err.println("Unknown argument: " + arg);
            }
        }

        if (infile == null) {
            System.err.println("Missing input file");
            return;
        }
        if (!infile.exists()) {
            System.err.println("Input file " + infile + " does not exists");
            return;
        }
        if (outfile == null) {
            outfile = new File(defaultOutputFilename);
        }

        WordsCounter reader = new WordsCounter(infile);
        WordsWriter writer = new WordsWriter(outfile);
        writer.write(reader.read().getSortedWords(descend));
    }
}
