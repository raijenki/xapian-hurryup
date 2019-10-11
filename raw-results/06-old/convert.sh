for counter in {1..30..1}
	do
		python parselats.py pure-start@2.6ghz-10servers-$counter.bin
	done
mv *.txt txt
