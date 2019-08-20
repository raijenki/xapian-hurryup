
taskset -c 0 timeout "${1}" cat /dev/urandom > /dev/null
