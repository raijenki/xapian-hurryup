for counter in 2 6 30 32 33 37
#was having lol errors with {1..30..1}
	do
		echo "File: $counter"
		python parselats.py pure-start@1.4ghz-10servers-$counter.bin
	done
mv *.txt txt
