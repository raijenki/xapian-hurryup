
sudo modprobe msr
UNCORE=`sudo rdmsr -p 0 0x620`
LOW="${UNCORE:0:1}"
HIGH="${UNCORE:1:5}"

LOW_UNCORE=$((16#${LOW}))
HIGH_UNCORE=$((16#${HIGH}))

for (( val = $LOW_UNCORE; val <= ${HIGH_UNCORE}; val++ ))
do
    printf -v result2 "%x" "$val"
    echo $val 0x$result2
done
