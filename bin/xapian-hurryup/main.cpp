#include <iostream>
#include <atomic>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <unordered_map>
#include <string.h>
#include "server.h"
#include "tbench_server.h"

using namespace std;

const unsigned long numRampUpReqs = 1000L;
const unsigned long numRampDownReqs = 1000L;
unsigned long numReqsToProcess = 1000L;
atomic_ulong numReqsProcessed(0);
pthread_barrier_t barrier;

//int coreNumber;

inline void usage() {
    cerr << "xapian_search [-n <numServers>]\
        [-d <dbPath>] [-r <numRequests]" << endl;
}

inline void sanityCheckArg(string msg) {
    if (strcmp(optarg, "?") == 0) {
        cerr << msg << endl;
        usage();
        exit(-1);
    }
}

int main(int argc, char* argv[]) {
    unsigned numServers = 4;
    string dbPath = "db";

    int c;
    string optString = "n:d:r:";
    while ((c = getopt(argc, argv, optString.c_str())) != -1) {
        switch (c) {
            case 'n':
                sanityCheckArg("Missing #servers");
                numServers = atoi(optarg);
                break;

            case 'd':
                sanityCheckArg("Missing dbPath");
                dbPath = optarg;
                break;

            case 'r':
                sanityCheckArg("Missing #reqs");
                numReqsToProcess = atol(optarg);
                break;
            default:
                cerr << "Unknown option " << c << endl;
                usage();
                exit(-1);
                break;
        }
    }
    //printf("oi");
    tBenchServerInit(numServers);
    //printf("oi");
    Server::init(numReqsToProcess, numServers);
    Server** servers = new Server* [numServers];
    for (unsigned i = 0; i < numServers; i++)
        servers[i] = new Server(i, dbPath);
   
    pthread_t* threads = NULL;
    string concat_dir1 = "/sys/devices/system/cpu/cpu";
    string concat_dir2;
    string concat_dir3;
    cpu_set_t cpuset;
    //FILE *cores = fopen("cores.txt", "a+"); 
    int coreNumber = 0;
        if (numServers > 1) {
        threads = new pthread_t [numServers - 1];
        for (unsigned i = 0; i < numServers - 1; i++) {
            pthread_create(&threads[i], NULL, Server::run, servers[i]);
	    //printf("Thread on core %d", coreNumber);
	    // For hurry-up purposes: BEGGININGa
	    //fprintf(cores, "core %d created\n", coreNumber);
	   
	    // For hurry-up purposes: BEGGINING
	    concat_dir2 = to_string(coreNumber);
	    concat_dir3 = concat_dir1 + concat_dir2 + "/cpufreq/scaling_setspeed";
	    fd[coreNumber] = open(concat_dir3.c_str(), O_RDWR);
	    CPU_ZERO(&cpuset);
	    CPU_SET(coreNumber, &cpuset);
	    pthread_setaffinity_np(threads[i], sizeof(cpu_set_t), &cpuset);
	    core_mapping[threads[i]] = coreNumber;
	    //printf("map %d ", coreNumber);
	    coreNumber = coreNumber + 2;

	    // For hurry-up purposes: ENDING
        }
	//fclose(cores);

    }
    pthread_create(&hurryup, NULL, hurryScheduler, NULL);
    CPU_ZERO(&cpuset);
    CPU_SET(15, &cpuset);
    pthread_setaffinity_np(hurryup, sizeof(cpu_set_t), &cpuset);
    Server::run(servers[numServers - 1]);

    if (numServers > 1) {
        for (unsigned i = 0; i < numServers - 1; i++)
            pthread_join(threads[i], NULL);
    }

    tBenchServerFinish();

    for (unsigned i = 0; i < numServers; i++)
        delete servers[i];

    Server::fini();

    return 0;
}
