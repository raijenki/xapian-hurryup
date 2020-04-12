
HYPERTHREADED=`grep -H . /sys/devices/system/cpu/cpu*/topology/thread_siblings_list | sort -n -t ',' -k 2 -u | cut -d \: -f 2 | cut -d \, -f 2`
for x in $HYPERTHREADED; do
    echo 0 > /sys/devices/system/cpu/cpu${x}/online;
done
