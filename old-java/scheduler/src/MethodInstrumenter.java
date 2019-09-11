import org.objectweb.asm.*;
import org.objectweb.asm.commons.LocalVariablesSorter;
import static org.objectweb.asm.Opcodes.*;

public class MethodInstrumenter extends MethodVisitor {

    private static final int BIG_AFFINITY = (1 << 0) | (1 << 1);

    private LocalVariablesSorter ls;
    private int varThreadId, varException;
    private Label tryStart, tryEnd;
    private Label catchStart, catchEnd;

    public MethodInstrumenter(MethodVisitor mv) {
        super(Opcodes.ASM5, mv);
    }

    public void setVariablesSorter(LocalVariablesSorter ls) {
        this.ls = ls;
    }

    @Override
    public void visitCode() {
        super.visitCode();

        tryStart = new Label();
        tryEnd = new Label();
        catchStart = new Label();
        catchEnd = new Label();

        visitTryCatchBlock(tryStart, tryEnd, catchStart, null);

        Type throwableType = Type.getObjectType("java/lang/Throwable");
        varThreadId = ls.newLocal(Type.INT_TYPE);
        varException = ls.newLocal(throwableType);

        visitMethodInsn(INVOKESTATIC, "net/openhft/affinity/Affinity", "getThreadId", "()I", false);
        visitVarInsn(ISTORE, varThreadId);

        visitVarInsn(ILOAD, varThreadId);
        visitInsn(I2L);
        visitMethodInsn(INVOKESTATIC, "org/TheAwesomeScheduler/HotFunctionTracker", "sendEnter", "(J)V", false);

        visitLabel(tryStart);

        generateDebugPrint("Affinity was set!");
    }

    @Override
    public void visitInsn(int opcode) {
        switch(opcode) {
            case IRETURN:
            case FRETURN:
            case ARETURN:
            case LRETURN:
            case DRETURN:
            case RETURN:
                generateAffinityRestore();
                break;
        }
        super.visitInsn(opcode);
    }

    @Override
    public void visitMaxs(int maxStack, int maxLocals) {

        // Generates garbage code for our user case, but it's the safe way to go.
        visitLabel(tryEnd);
        generateAffinityRestore();
        visitJumpInsn(GOTO, catchEnd);


        visitLabel(catchStart);
        // This frame is very complicated. Let ASM recaculate it.
        //visitFrame(Opcodes.F_FULL, N_, new Object[] {INTERNAL_NAMES_OF_ARGS..., INTERNAL_NAME_OF_AFFINITY}, 1, new Object[] {"java/lang/Throwable"});
        visitVarInsn(ASTORE, varException);
        generateAffinityRestore();
        visitVarInsn(ALOAD, varException);
        visitInsn(ATHROW);
        visitLabel(catchEnd);

        // Generates garbage code for our user case, but it's the safe way to go.
        visitFrame(Opcodes.F_SAME, 0, null, 0, null);
        visitInsn(RETURN);

        super.visitMaxs(maxStack, maxLocals);
    }

    private void generateDebugPrint(String message) {
        if(false) {
            visitFieldInsn(GETSTATIC, "java/lang/System", "err", "Ljava/io/PrintStream;");
            visitLdcInsn(message);
            visitMethodInsn(INVOKEVIRTUAL, "java/io/PrintStream", "println", "(Ljava/lang/String;)V", false);
        }
    }

    private void generateAffinityRestore() {
        visitVarInsn(ILOAD, varThreadId);
        visitInsn(I2L);
        visitMethodInsn(INVOKESTATIC, "org/TheAwesomeScheduler/HotFunctionTracker", "sendLeave", "(J)V", false);

        generateDebugPrint("Affinity was restored!");
    }
}
