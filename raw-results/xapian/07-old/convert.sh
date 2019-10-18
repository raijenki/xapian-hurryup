for counter in 1 
#was having lol errors with {1..30..1}
	do
		echo "File: $counter"
		python parselats.py pure-start@1.6ghz-10servers-$counter.bin
	done
mv *.txt txt
