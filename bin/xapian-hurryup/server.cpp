#include <asm/unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/syscall.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include "server.h"
#include "tbench_server.h"

// FOR HURRY-UP PURPOSES: BEGGINING
#include <sched.h>
#include <stdatomic.h>
#include <unordered_map>
#define _GNU_SOURCE 1
#define IA32_PERF_STATUS 0x198      //only read
#define IA32_PERF_CTL 0x199         ///only write
#define UNCORE_FREQ 0x620


std::atomic_int coreId;
//std::atomic<int> coreId;
pthread_t hurryup;
bool running = true;
std::unordered_map <int, int> schedMap;

// FOR HURRY-UP PURPOSES: ENDING



using namespace std;

unsigned long Server::numReqsToProcess = 0;
volatile atomic_ulong Server::numReqsProcessed(0);
pthread_barrier_t Server::barrier;


Server::Server(int id, string dbPath) 
    : db(dbPath)
    , enquire(db)
    , stemmer("english")
    , id(id)
{
    const char* stopWords[] = { "a", "about", "an", "and", "are", "as", "at", "be",
        "by", "en", "for", "from", "how", "i", "in", "is", "it", "of", "on",
        "or", "that", "the", "this", "to", "was", "what", "when", "where",
        "which", "who", "why", "will", "with" };

    stopper = Xapian::SimpleStopper(stopWords, \
            stopWords + sizeof(stopWords) / sizeof(stopWords[0]));

    parser.set_database(db);
    parser.set_default_op(Xapian::Query::OP_OR);
    parser.set_stemmer(stemmer);
    parser.set_stemming_strategy(Xapian::QueryParser::STEM_SOME);
    parser.set_stopper(&stopper);
}

Server::~Server() {
}



void Server::_run() {
    pthread_barrier_wait(&barrier);

    tBenchServerThreadStart();

    while (numReqsProcessed < numReqsToProcess) {
       processRequest();
       ++numReqsProcessed;
    }
}

// FOR HURRY-UP PURPOSES: BEGGINING
void* hurryScheduler(void* v) {
    uint64_t maxFreq = 0x1a00;
    uint64_t defaultFreq = 0x1200;
    //cpu_set_t set;
    //CPU_SET(15, &set);
    //int msr_fd = open("/dev/cpu/12/msr", O_RDWR);
    //sched_setaffinity(0, sizeof(set), &set); 

    //int schedservers = 2;
	while(running) {    
       // for(int i = 0; i < schedservers; i++) {
            //if(sched[i] == true) {
   //             printf("Changing freq\N");
                //pwrite(msr_fd, &maxFreq, 8, IA32_PERF_CTL);
           // }
            //if(sched[i] == false) {
                //printf("Diminishing freq");
                //pwrite(msr_fd, &defaultFreq, 8, IA32_PERF_CTL);
         //   }
       // }
   }
}

// FOR HURRY-UP PURPOSES: ENDING

void Server::processRequest() {
    const unsigned MAX_TERM_LEN = 256;
    char term[MAX_TERM_LEN];
    void* termPtr;
   
    size_t len = tBenchRecvReq(&termPtr);
    memcpy(reinterpret_cast<void*>(term), termPtr, len);
    term[len] = '\0';

    unsigned int flags = Xapian::QueryParser::FLAG_DEFAULT;
    Xapian::Query query = parser.parse_query(term, flags);
    enquire.set_query(query);
    
    /*pthread_t autoThread; 
    int p;
    autoThread = pthread_self();
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    /CPU_ISSET(p, &cpuset);

    printf("%d ", p)a;*/
    mset = enquire.get_mset(0, MSET_SIZE);
    
    const unsigned MAX_RES_LEN = 1 << 20;
    char res[MAX_RES_LEN];

    unsigned resLen = 0;
    unsigned doccount = 0;
    const unsigned MAX_DOC_COUNT = 25; // up to 25 results per page
    for (auto it = mset.begin(); it != mset.end(); ++it) {
        std::string desc = it.get_document().get_description();
        resLen += desc.size();
        assert(resLen <= MAX_RES_LEN);
        memcpy(reinterpret_cast<void*>(&res[resLen]), desc.c_str(), desc.size());

        if (++doccount == MAX_DOC_COUNT) break;
    }
    //sched[0] = false;
    tBenchSendResp(reinterpret_cast<void*>(res), resLen);

}

void* Server::run(void* v) {
    
	
    int coreNumber = coreId;
    coreId = coreId + 2;
    printf("%d, coreNumber");
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(coreNumber, &cpuset);
    pthread_t selfThread = pthread_self();
    pthread_setaffinity_np(selfThread, sizeof(cpu_set_t), &cpuset); 


    Server* server = static_cast<Server*> (v);
    //pthread_create(&hurryup, NULL, hurryScheduler, NULL);
    server->_run();
    return NULL;
}

void Server::init(unsigned long _numReqsToProcess, unsigned numServers) {
    numReqsToProcess = _numReqsToProcess;
    pthread_barrier_init(&barrier, NULL, numServers);
}

void Server::fini() {
//    pthread_cancel(hurryup);
    running = false;
    printf("Waiting ");
    pthread_join(hurryup, NULL);
    printf("Ended");
    pthread_barrier_destroy(&barrier);
    }
