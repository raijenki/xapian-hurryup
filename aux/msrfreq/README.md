check_uncore_range.bash --> Determine uncore frequencies and the hex value

disable_hyperthreading.bash --> disable hyperthreading

set_userspace.bash --> set userspace governor

pseudo_urandom.bash --> generate pseudo random load. need to specify a timer

set_freq_normal.bash --> set frequency the normal way. needs frequency as argument

check_requested_and_set_freq.bash --> Gives the requested and set frequency as read from MSR

check_core_range.bash --> gives core frequency and the hex value

set_msr_frequency.bash --> sets frequency + runs check_requested_and_set_freq.bash + runs pseudo_urandom.bash +  reads the current frequency from cpufreq-info to verify it's all fine

latency_test.c --> c program. yet to be tested. wondering what's the best way.

latency_test 
