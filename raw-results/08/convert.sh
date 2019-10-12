for counter in 1 2 10 15 17 20 28 31 32 33 39
#was having lol errors with {1..30..1}
	do
		echo "File: $counter"
		python parselats.py hurryup-start@1.4ghz-10servers-$counter.bin
	done
mv *.txt txt
