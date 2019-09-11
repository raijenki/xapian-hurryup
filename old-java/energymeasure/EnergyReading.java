package energymeasure;

public class EnergyReading
{
    static {
        System.loadLibrary("energymeasure");
    }

    public double energySys;
    public double energyA57;
    public double energyA53;

    public native void measureEnergy();
    public static native void setAffinity(int pid, int affinity);
    public static native int getAffinity(int pid);

    public static void main(String[] args) {
        long start, end, time1, time2;

        start = System.nanoTime();
        EnergyReading reading = new EnergyReading();
        reading.measureEnergy();
        end = System.nanoTime();
        time1 = (end - start);
        System.out.println("Energy: " + (reading.energySys) + "\n");
        System.out.println("Time taken: " + (end - start) + " ns");

        System.out.printf("Current affinity: %d\n", getAffinity(0));
        setAffinity(0, (1 << 0));
        System.out.printf("Current affinity (expected 1): %d\n", getAffinity(0));
        setAffinity(0, (1 << 3));
        System.out.printf("Current affinity (expected 8): %d\n", getAffinity(0));
        setAffinity(0, (1 << 3) | (1 << 1));
        System.out.printf("Current affinity (expected 10): %d\n", getAffinity(0));
    }
};

