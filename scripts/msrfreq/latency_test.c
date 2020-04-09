/*
 * Change CPU frequency. 
 */

#define _GNU_SOURCE 1

#include <asm/unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>


#define IA32_PERF_STATUS 0x198      //only read
#define IA32_PERF_CTL 0x199         ///only write

/*http://www.zen.science/ 
*To determine uncore frequency range
*The register contents represent upper and lower bounds for the uncore 
*frequency: Bits 15 through 8 encode the minimum and bits
*7 to 0 the maximum frequency.*/
#define UNCORE_FREQ 0x620

/**
 * Returns time stamp counter
 */
static __inline__ uint64_t rdtsc(void)
{
    uint32_t a, d;
    asm volatile("rdtsc" : "=a" (a), "=d" (d));
    return (uint64_t) a | (uint64_t) d << 32;

}

void main()
{
    
    // tested core frequencies
    uint64_t core_settings[] = {0x1a00, 0x1900, 0x1800, 0x1700, 0x1500, 0x1400, 0x1300, 
            0x1200, 0x1100, 0x1000, 0xf00, 0xd00, 0xc00, 0xb00, 0xa00};
    // tested unCore FS frequencies 
    uint64_t uncore_settings[] = { 0xc0c, 0xd0d, 0xe0e, 0xf0f, 0x1010, 0x1111, 0x1212,
            0x1313, 0x1414, 0x1515, 0x1616, 0x1717, 0x1818 };

    uint64_t default_core_setting   = 0xa;
    uint64_t default_uncore_setting = 0xc18;

    int nr_core_settings   = 15;
    int nr_uncore_settings = 13;

    // open fd for switching msr
    int msr_fd = open("/dev/cpu/0/msr", O_RDWR);
    if (msr_fd < 0) {
        printf ("modprobe msr\n");
        printf ("Need to be root.\n");
        exit(0);
    }

    uint64_t start_time, end_time;
    int iters = 1000;
    
    struct timeval stop, start;

    // for each source target combination:
    for (uint64_t source = 0; source < nr_core_settings; source++)
        for (uint64_t target = 0; target < nr_core_settings; target++)
        {
            if (source == target)
                continue;
        
            start_time = rdtsc();
            gettimeofday(&start, NULL);

            for (int i = 0; i < iters; i++)
            {
                // set default
                pwrite(msr_fd, &core_settings[source], 8, IA32_PERF_CTL);
                pwrite(msr_fd, &core_settings[target], 8, IA32_PERF_CTL);
                //system("./check_requested_and_set_freq.bash");
            }
            gettimeofday(&stop, NULL);
           
            end_time = rdtsc();
            uint64_t delta_us = (stop.tv_usec - start.tv_usec);
            printf("change from %d00MHz to %d00MHz over %d iterations is %lu in us %lu\n",
                    0xFF & core_settings[source], 0xFF & core_settings[target], iters, (end_time - start_time), delta_us/1000);
        }
    // set default again
    pwrite(msr_fd, &default_core_setting, 8, IA32_PERF_CTL);
}
