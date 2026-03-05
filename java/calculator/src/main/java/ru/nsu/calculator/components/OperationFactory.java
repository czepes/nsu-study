package ru.nsu.calculator.components;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.lang.reflect.Constructor;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.stream.Collectors;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import ru.nsu.calculator.exceptions.factory.OperationCreationException;
import ru.nsu.calculator.exceptions.factory.OperationNotFoundException;
import ru.nsu.calculator.exceptions.factory.ConfigurationException;
import ru.nsu.calculator.operations.Operation;

public class OperationFactory {
    private static final Logger log = LoggerFactory.getLogger(OperationFactory.class);

    public static final String defaultConfigSrc = "/default.ops";
    public static final String defaultUsageSrc = "/default.usage";

    private final Map<String, Constructor<Operation>> constructorsMap;
    private final String usage;

    public OperationFactory() {
        this(defaultConfigSrc, defaultUsageSrc);
    }

    public OperationFactory(String configSrc, String usageSrc) throws ConfigurationException {
        log.info("Creating Command Factory");
        try (InputStream in = OperationFactory.class.getResourceAsStream(configSrc);
                InputStreamReader sr = new InputStreamReader(in);
                BufferedReader br = new BufferedReader(sr)) {
            Map<String, Constructor<Operation>> map = new HashMap<>();
            String line;

            while ((line = br.readLine()) != null) {
                String[] cmdLocation = line.split(" ");
                Constructor<Operation> constr = (Constructor<Operation>) Class
                        .forName(cmdLocation[1])
                        .getConstructor(ExecutionContext.class);
                map.put(cmdLocation[0], constr);
            }

            this.constructorsMap = Collections.unmodifiableMap(map);
        } catch (NullPointerException | IOException e) {
            log.error(e.getMessage(), e);
            throw new ConfigurationException("Failed to locate commands configuration file");
        } catch (ReflectiveOperationException e) {
            log.error(e.getMessage(), e);
            throw new ConfigurationException("File has operation with unresolved class");
        }

        try (InputStream in = OperationFactory.class.getResourceAsStream(usageSrc);
                InputStreamReader sr = new InputStreamReader(in);
                BufferedReader br = new BufferedReader(sr)) {
            this.usage = br
                    .lines()
                    .collect(Collectors
                            .joining(System.lineSeparator()));
        } catch (NullPointerException | IOException e) {
            log.error(e.getMessage(), e);
            throw new ConfigurationException("Failed to locate commands usage file");
        }
        log.info("Command Factory {} created", this);
    }

    public void printUsage(PrintStream out) {
        out.println(usage);
    }

    public Operation build(ExecutionContext context)
            throws OperationNotFoundException, OperationCreationException {
        String cmdName = context.getLastCommand().getCommandName();
        Constructor<Operation> builder = constructorsMap.get(cmdName);

        log.info("Building command '{}' of class {}", cmdName, builder);

        if (builder == null) {
            throw new OperationNotFoundException(cmdName);
        }

        ru.nsu.calculator.operations.Operation cmd = null;

        try {
            cmd = (Operation) builder.newInstance(context);
        } catch (ReflectiveOperationException e) {
            log.error(e.getMessage(), e);
            throw new OperationCreationException(cmdName, builder.toString());
        }

        return cmd;
    }
}
