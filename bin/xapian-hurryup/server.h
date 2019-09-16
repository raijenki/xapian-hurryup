#ifndef __SERVER_H
#define __SERVER_H

#include <stdatomic.h>
#include <pthread.h>
#include <xapian.h>
#include <vector>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>
class Server {
    private:
        static unsigned long numReqsToProcess;
        static volatile std::atomic_ulong numReqsProcessed;
        static const unsigned int MSET_SIZE = 20480;
        static pthread_barrier_t barrier;
	pid_t x;
        Xapian::Database db;
        Xapian::Enquire enquire;
        Xapian::Stem stemmer;
        Xapian::SimpleStopper stopper;
        Xapian::QueryParser parser;
        pthread_mutex_t lock;
        Xapian::MSet mset;
	//bool sched;
        int id;
        void _run();
        void processRequest();
	public:
        Server(int id, std::string dbPath);
        ~Server();
	
	static void* run(void* v);
        static void init(unsigned long _numReqsToProcess, unsigned numServers);
        static void fini();
	};
void* hurryScheduler(void* v);	
extern bool sched[2];
extern atomic_int coreId;
extern bool running;
extern pthread_t hurryup;
	//bool sched;

#endif
