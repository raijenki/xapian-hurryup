#include <atomic>
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
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include "server.h"
#include "tbench_server.h"

// FOR HURRY-UP PURPOSES: BEGGINING
#include <sched.h>
#include <unordered_map>
#define _GNU_SOURCE 1
#define IA32_PERF_STATUS 0x198     //only read
#define IA32_PERF_CTL 0x199         ///only write
#define UNCORE_FREQ 0x620


//std::atomic<int> coreId;
pthread_t hurryup;
int running = 1;
int fd[24];
std::unordered_map <pthread_t, int> schedMap;
std::unordered_map <pthread_t, int> core_mapping;
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
    string concat_dir1, concat_dir2, concat_dir3;
    string steps[13] = { "1000000", "1100000", "1200000", "1500000", "1600000", 
	    "1700000", "1800000", "1900000", "2000000", "2100000", "2300000", 
	    "2400000", "2500000" };
    int coreFreqs[27];
    int step;

    for(int i = 0; i <= 26; i++) {
	    coreFreqs[i] = 3;
    }

	while(running) {
		for (auto x : core_mapping) {
			if(schedMap[x.first] == x.second) {
				coreFreqs[x.second] += 4; 
				if(coreFreqs[x.second] > 12) coreFreqs[x.second] = 12;
				step = floor(coreFreqs[x.second]);
				write(fd[x.second], steps[step].c_str(), strlen(steps[step].c_str()));
	
				}
			else {
				coreFreqs[x.second] -= 1;
				if(coreFreqs[x.second] < 0) coreFreqs[x.second] = 0;
				step = ceil(coreFreqs[x.second]);
				write(fd[x.second], steps[step].c_str(), strlen(steps[step].c_str()));
				//fputs("1800000", pFile);
				//printf("freq`s down");
			//	fwrite(min, 1, sizeof(min), pFile);
				}
			lseek(fd[x.second], 0, SEEK_SET);
			//fclose(pFile);
   		}
		usleep(3000);
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
    pthread_t autoThread = pthread_self();
    schedMap[autoThread] = core_mapping[autoThread];
  
    Xapian::Query query = parser.parse_query(term, flags);
    enquire.set_query(query);
    
    // FOR HP PURPOSES: BEGGINING
   
    mset = enquire.get_mset(0, MSET_SIZE);
    schedMap[autoThread] = -1;

   
       // FOR HP PURPOSES: ENDING

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
    running = 0;
    //printf("Waiting ");
    pthread_join(hurryup, NULL);
    //printf("Ended");
    pthread_barrier_destroy(&barrier);
    }
