
This is an Java Instrumentation Agent that adds core locking into user-defined methods.

Basically, whatever method is instrumented will be transformed from

    void methodName() {
        statements;
    }

into

    void methodName() {
        AffinityLock al = AffinityLock.acquireLock();
        try {
            statements;
        } finally {
            al.release();
        }
    }

See the [affinity library](https://github.com/OpenHFT/Java-Thread-Affinity) for further details.

This is a very basic transformation, it's just to begin with, we should probably toy with the affinity library a bit in order to find the best code to instrument into applications.

Given there is no visible user visible side-effects, maybe it's a good idea to change the `if(false)` to `if(true)` in `generateDebugPrint` at `src/MethodInstrumenter.java`. This will print some stuff in `stderr` when the `try` and `finally` block are reached.


## Building

To setup the instrumenter for building follow these instructions:

1. Download http://download.forge.ow2.org/asm/asm-6.0-bin.zip and extract it to this directory.
2. Download https://jar-download.com/explore-java-source-code.php?a=affinity&g=net.openhft&v=3.1.7&downloadable=1 and extract it to the `affinity/` directory.
3. Now you should have a `asm-6.0/` and an `affinity/` directory here.
4. To build the agent run `build-agent.sh`.

## Using

To use the instrumentation agent you first need to define which methods need to be instrumented.

Once that is done, you have to obtain the fully qualified internal name of the class (e.g. `java/lang/String`) and which method you want to instrument (e.g. `charAt`).

Then create a text file (e.g. `agent.txt`) and add the following line:
    
    java/lang/String charAt

This will instruct the agent to instrument the method `charAt` from the class `java.lang.String`.

You may insert as many methods as you want to instrument, each in a different line. For instance:

    java/lang/String charAt
    java/lang/String join
    java/lang/String trim

You may also specify the argument description of the method as a third token in order to instrument just a single overload.

    some/random/Class main ([Ljava/lang/String;)V

To run a Java application using the instrumenter use the `run-agent.sh` utility. For example:

    bash run-agent.sh ./agent.txt -jar example.jar

You may manually tweak a `java` invocation to use the agent as well. Simply add the argument `-javaagent:agent.jar=agent.txt` to the `java` command.

## Utilities

The following scripts are available for helping purposes:

### asmifier.sh \<classfile\> [args...]

This utility will output to stdout the ASM representation of a Java .class file.

Any other arguments, except the first, will be sent to the `java` invocation.

### build-agent.sh

This will build the `agent.jar` that performs instrumentation.

### run-agent.sh \<details\> [args...]

Invokes the JVM using our instrumentation agent `agent.jar` with instrumentation details taken from the **details** config file (see **Using**). Use further arguments to direct the `java` invocation into the right direction (e.g. specify main class file).

Any further arguments will be sent to the `java` invocation.

This requires `agent.jar` to be already built.

### copy-agent.sh \<src-classfile\> \<dst-classfile\> \<classname\> \<classmethod\> [args..]

This utility will transform the method named **classmethod** from the class named **classname** (use the internal class name) from the class file **src-classfile**. The transformed class file will be generated as **dst-classfile**.

Any further arguments will be sent to the `java` invocation.

This requires `agent.jar` to be already built.

## Issues?

* Generics are not taken into account. I'm not sure what happens when we try to instrument them.
* Every instrumented method will have some garbage bytecode that is never reachable. No big deal. This is a safe way to generalize avoidance for dangling instructions.
* We do not explicitly create the frame for the `finally` block used to release the lock. It's a little complicated. But ASM can recompute frames, so it's fine. This may become a problem if the `try-finally` block is not the outer most block of the method (or we create locals in its scope) because `LocalVariablesSorter` needs complete frame information to create new local variables.

