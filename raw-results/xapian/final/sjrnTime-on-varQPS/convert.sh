for process in 120 240 360 480 600 720 840 960 1080 1200 1320
do
	for counter in 1 2 3 4 5
	do
		echo "File: ${counter}"
		python parselats.py pure-governor@userspace-12servers-${counter}-${process}.bin
	done
done
mv *.txt pure-cpufreq@1.8/txt/
mv *.bin pure-cpufreq@1.8/bin/
