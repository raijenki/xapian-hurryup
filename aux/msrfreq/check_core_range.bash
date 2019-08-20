
UNCORE=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies`

for c in $UNCORE; do
    val=${c:0:2}
    printf -v result "%x" "$val"
    echo $c 0x${result}00
done
