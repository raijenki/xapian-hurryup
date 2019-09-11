import java.io.*;
import java.lang.RuntimeException;
import java.util.HashMap;
import java.util.HashSet;
import org.objectweb.asm.*;

public class BasicTransformer {

    private HashMap<String, ClassInstrumentDetail> classList;

    public BasicTransformer() {
        this.classList = new HashMap<String, ClassInstrumentDetail>();
    }

    public boolean shouldTransformClass(String className) {
        return classList.containsKey(className);
    }

    public ClassInstrumentDetail getClassInstrumentDetail(String className) {
        return classList.get(className);
    }

    public void addMethodToInstrument(String className, String methodName,
                                      String methodDesc, String methodSignature) {
        ClassInstrumentDetail classDetail = this.getClassInstrumentDetail(className);
        if(classDetail == null) {
            classDetail = new ClassInstrumentDetail();
            this.classList.put(className, classDetail);
        }
        classDetail.addMethodToInstrument(methodName, methodDesc, methodSignature);
    }

    public void loadMethodsToInstrument(String configPath) throws IOException, RuntimeException {
        try(BufferedReader br = new BufferedReader(new FileReader(configPath))) {
            String line;
            while((line = br.readLine()) != null) {
                line = line.trim();
                if(line.length() != 0) {
                    String[] tokens = line.split("\\s+");
                    if(tokens.length < 2 || tokens.length > 3)
                        throw new RuntimeException("Wrong number of tokens in line: " + line);


                    String className = tokens[0];
                    String methodName = tokens[1];
                    String methodDesc = tokens.length > 2? tokens[2] : null;
                    this.addMethodToInstrument(className, methodName, methodDesc, null);
                }
            }
        }
    }

    public byte[] transform(String className, byte[] classfileBuffer) {
        ClassInstrumentDetail classDetail = this.getClassInstrumentDetail(className);
        if(classDetail != null) {
            ClassReader reader = new ClassReader(classfileBuffer);
            return transform(classDetail, reader);
        }
        return classfileBuffer;
    }

    public byte[] transform(ClassInstrumentDetail classDetail, ClassReader reader) {
        ClassWriter writer = new ClassWriter(ClassWriter.COMPUTE_MAXS | ClassWriter.COMPUTE_FRAMES);
        ClassInstrumenter visitor = new ClassInstrumenter(writer, classDetail);
        reader.accept(visitor, ClassReader.EXPAND_FRAMES);
        return writer.toByteArray();
    }

    public class ClassInstrumentDetail {
        private HashMap<String, MethodInstrumentDetail> methodList;

        public ClassInstrumentDetail() {
            this.methodList = new HashMap<String, MethodInstrumentDetail>();
        }

        public void addMethodToInstrument(String name, String desc, String signature) {
            MethodInstrumentDetail methodDetail = methodList.get(name);
            if(methodDetail == null) {
                methodDetail = new MethodInstrumentDetail();
                methodList.put(name, methodDetail);
            }
            methodDetail.addDetail(desc, signature);
        }

        public boolean shouldInstrument(String name, String desc, String signature) {
            MethodInstrumentDetail methodDetail = methodList.get(name);
            if(methodDetail != null) {
                return methodDetail.shouldInstrument(desc, signature);
            }
            return false;
        }
    }

    private class MethodInstrumentDetail {
        private HashSet<String> descList = new HashSet<String>();

        public void addDetail(String desc, String signature) {
            if(desc != null) {
                descList.add(desc);
            }
        }

        public boolean shouldInstrument(String desc, String signature) {
            if(descList.size() == 0)
                return true;
            return descList.contains(desc);
        }
    }
}

