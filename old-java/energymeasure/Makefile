JAVA_HOME?=/usr/lib/jvm/java-8-oracle

LDFLAGS=-fPIC
CFLAGS=-std=c99

all: EnergyReading.class libenergymeasure.so

EnergyReading.class: EnergyReading.java
	javac EnergyReading.java

energymeasure_EnergyReading.h: EnergyReading.java
	javac -h . EnergyReading.java -d .

readenergy.o: EnergyReading.class readenergy.c readenergy.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c readenergy.c -o $@

libenergymeasure.so: readenergy.o energymeasure_EnergyReading.c energymeasure_EnergyReading.h
	$(CC) $(CFLAGS) $(LDFLAGS) \
		-I"$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/linux" \
		-shared -o $@ energymeasure_EnergyReading.c readenergy.o

clean:
	$(RM) EnergyReading.class
	$(RM) readenergy.o
	$(RM) libenergymeasure.so

