for counter in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
#was having lol errors with {1..30..1}
	do
		echo "File: $counter"
		python parselats.py pure-start@2.6ghz-10servers-$counter.bin
	done
mv *.txt txt
