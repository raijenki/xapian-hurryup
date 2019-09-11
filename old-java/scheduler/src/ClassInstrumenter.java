import org.objectweb.asm.*;
import org.objectweb.asm.commons.LocalVariablesSorter;
import static org.objectweb.asm.Opcodes.*;

public class ClassInstrumenter extends ClassVisitor {

    private BasicTransformer.ClassInstrumentDetail classDetail;

    public ClassInstrumenter(ClassVisitor cv, BasicTransformer.ClassInstrumentDetail classDetail) {
        super(Opcodes.ASM5, cv);
        this.classDetail = classDetail;
    }

    public MethodVisitor visitMethod(int access, String name, String desc, 
                                     String signature, String[] exceptions) {

        MethodVisitor superMv = super.visitMethod(access, name, desc, signature, exceptions);

        if(!this.classDetail.shouldInstrument(name, desc, signature)) {
            return superMv;
        }

        MethodInstrumenter instMv = new MethodInstrumenter(superMv);
        LocalVariablesSorter varsMv = new LocalVariablesSorter(access, desc, instMv);
        instMv.setVariablesSorter(varsMv);
        MethodVisitor mv = varsMv;

        return mv;
    }
}
