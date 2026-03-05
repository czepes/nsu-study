package ru.nsu.calculator;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

import java.io.File;
import java.util.HashMap;
import java.util.Map;

import ru.nsu.calculator.components.*;
import ru.nsu.calculator.operations.*;
import ru.nsu.calculator.exceptions.io.*;
import ru.nsu.calculator.exceptions.command.*;
import ru.nsu.calculator.exceptions.factory.*;

public class CalculatorTest {
    @Test
    public void testCommandReader() {
        assertThrows(CalculatorFileNotFoundException.class, () -> {
            CommandReader.from(new File("./data/no-file.calc"));
        });

        assertThrows(CalculatorNoNextCommandException.class, () -> {
            CommandReader cr = CommandReader.from(new File("./data/almost-empty.calc"));
            cr.next();
            cr.next();
        });

        CommandReader cr = CommandReader.from(new File("./data/sum.calc"));
        Command[] cmd = new Command[5];

        for (int i = 0; i < cmd.length; i++) {
            cmd[i] = cr.next();
        }

        assertEquals(cmd[0].getType(), CommandType.SKIP);
        assertEquals(cmd[0].getCommandName(), "#");
        assertEquals(cmd[1].getType(), CommandType.EXEC);
        assertEquals(cmd[1].getCommandName(), "push");
        assertEquals(cmd[2].getType(), CommandType.EXEC);
        assertEquals(cmd[2].getCommandName(), "push");
        assertEquals(cmd[3].getType(), CommandType.EXEC);
        assertEquals(cmd[3].getCommandName(), "+");
        assertEquals(cmd[4].getType(), CommandType.EXEC);
        assertEquals(cmd[4].getCommandName(), "print");
    }

    @Test
    public void testCommandFactory() {
        assertThrows(ConfigurationException.class, () -> {
            new OperationFactory("/no-file.ops", OperationFactory.defaultUsageSrc);
        });

        assertThrows(ConfigurationException.class, () -> {
            new OperationFactory(OperationFactory.defaultConfigSrc, "/no-file.usage");
        });

        assertThrows(ConfigurationException.class, () -> {
            new OperationFactory("./unresolved.ops", OperationFactory.defaultUsageSrc);
        });

        assertThrows(OperationNotFoundException.class, () -> {
            ExecutionContext ctx = new ExecutionContext();
            OperationFactory cf = new OperationFactory();
            ctx.update(Command.parse("unknown-command"));
            cf.build(ctx);
        });

        {
            ExecutionContext ctx = new ExecutionContext();
            OperationFactory cf = new OperationFactory();

            Map<String, Class<?>> classes = new HashMap<>();
            classes.put("push", PushOperation.class);
            classes.put("pop", PopOperation.class);
            classes.put("define", DefineOperation.class);
            classes.put("print", PrintOperation.class);
            classes.put("+", SumOperation.class);
            classes.put("-", DifOperation.class);
            classes.put("*", MulOperation.class);
            classes.put("/", DivOperation.class);
            classes.put("sqrt", SqrtOperation.class);

            classes.forEach((String cmd, Class<?> cls) -> {
                ctx.update(Command.parse(cmd));
                assertInstanceOf(cls, cf.build(ctx));
            });
        }
    }

    @Test
    public void testCommandExecution() {
        ExecutionContext ctx = new ExecutionContext();
        OperationFactory cf = new OperationFactory();
        Calculator c = new Calculator(ctx, cf);

        // PUSH ? -> !exception!
        assertThrows(InvalidArgumentException.class, () -> c.execute("push"));

        // POP ? -> !exception!
        assertThrows(StackUnderflowException.class, () -> c.execute("pop"));

        // DEFINE ? ? -> !exception!
        assertThrows(InvalidArgumentException.class, () -> c.execute("define"));
        assertThrows(InvalidArgumentException.class, () -> c.execute("define a"));
        assertThrows(InvalidArgumentException.class, () -> c.execute("define 0"));
        assertThrows(InvalidArgumentException.class, () -> c.execute("define ʕ·͡ᴥ·ʔ 0"));
        assertThrows(InvalidArgumentException.class, () -> c.execute("define 0 0"));

        // PRINT ? -> !exception!
        assertThrows(StackUnderflowException.class, () -> c.execute("print"));

        // op(?) -> !exception!
        assertThrows(StackUnderflowException.class, () -> c.execute("sqrt"));

        // ? op ? -> !exception!
        assertThrows(StackUnderflowException.class, () -> c.execute("+"));
        assertThrows(StackUnderflowException.class, () -> c.execute("-"));
        assertThrows(StackUnderflowException.class, () -> c.execute("/"));
        assertThrows(StackUnderflowException.class, () -> c.execute("*"));

        // PUSH 1
        c.execute("push 1");
        assertEquals(ctx.getLastResult(), 1);

        // POP 1
        c.execute("pop");
        assertEquals(ctx.getLastResult(), 1);

        // 1 + 1 = 2
        c.execute("push 1");
        c.execute("push 1");
        c.execute("+");
        assertEquals(ctx.getLastResult(), 2);

        // 2 - 1 = 1
        c.execute("push 1");
        c.execute("-");
        assertEquals(ctx.getLastResult(), 1);

        // 1 * 16 = 16
        c.execute("push 16");
        c.execute("*");
        assertEquals(ctx.getLastResult(), 16);

        // 16 / 0 -> !exception!
        c.execute("push 0");
        assertThrows(DivisionByZeroException.class, () -> c.execute("/"));
        c.execute("pop");

        // 16 / 4 = 4
        c.execute("push 4");
        c.execute("/");
        assertEquals(ctx.getLastResult(), 4);

        // sqrt(4) = 2
        c.execute("sqrt");
        assertEquals(ctx.getLastResult(), 2);

        // sqrt(-2) -> !exception!
        c.execute("push -1");
        c.execute("*");
        assertThrows(SqrtNegativeException.class, () -> c.execute("sqrt"));

        // DEFINE two 2; -2 + two = 0
        c.execute("define two 2");
        c.execute("push two");
        c.execute("+");
        assertEquals(ctx.getLastResult(), 0);
    }
}
