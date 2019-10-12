for counter in 2 3 4 8 10 12 14 16 18 21 24 25 26 27 29
#was having lol errors with {1..30..1}
	do
		echo "File: $counter"
		python parselats.py pure-start@1.6ghz-10servers-$counter.bin
	done
mv *.txt txt
