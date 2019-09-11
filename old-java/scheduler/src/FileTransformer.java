import java.io.*;
import org.objectweb.asm.*;

public class FileTransformer extends BasicTransformer {

    public byte[] transform(ClassInstrumentDetail classDetail, InputStream is) throws IOException {
        ClassReader reader = new ClassReader(is);
        return super.transform(classDetail, reader);
    }

    public static void main(String[] args) throws IOException {
        try(FileInputStream is = new FileInputStream(args[0])) {
            FileTransformer ts = new FileTransformer();
            ts.addMethodToInstrument(args[2], args[3], null, null);
            ClassInstrumentDetail classDetail = ts.getClassInstrumentDetail(args[2]);
            byte[] output = ts.transform(classDetail, is);
            try(FileOutputStream fos = new FileOutputStream(args[1])) {
                fos.write(output);
            }
        }
    }
}

