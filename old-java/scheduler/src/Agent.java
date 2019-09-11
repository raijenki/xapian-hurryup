import java.io.IOException;
import java.lang.instrument.Instrumentation;

public class Agent {
    public static void premain(String args, Instrumentation inst) throws IOException {
        AgentTransformer transformer = new AgentTransformer();
        if(args != null) {
            transformer.loadMethodsToInstrument(args);
        }
        inst.addTransformer(transformer);

        (new AgentSchedulingThread()).start();
    }
}

