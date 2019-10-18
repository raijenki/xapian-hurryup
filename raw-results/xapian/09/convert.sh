for counter in 1 2 3 10
#was having lol errors with {1..30..1}
	do
		echo "File: $counter"
		python parselats.py MSR-USERSPACE-hurryup-start@1.8ghz-10servers-$counter.bin
	done
mv *.txt txt
