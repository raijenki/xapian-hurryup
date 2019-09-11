import java.lang.instrument.*;
import java.security.ProtectionDomain;

public class AgentTransformer extends BasicTransformer 
                              implements ClassFileTransformer {
    public byte[] transform(ClassLoader loader, String className,
                            Class<?> classBeingRedefined, ProtectionDomain protectionDomain,
                            byte[] classfileBuffer) {
        return super.transform(className, classfileBuffer);
    }
}

