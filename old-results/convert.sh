for qps in 1200 2400 3600 4800 6000 7200
do
	for counter in 1
	do
		python parselats.py hup-userspace@min2.1ghz-max2.5ghz-12servers-$counter-$qps-sleep4000.bin
	done
done
mv *.txt txtfiles/
