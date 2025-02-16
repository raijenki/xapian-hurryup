#ifndef __SERVER_H
#define __SERVER_H

#include <unordered_map>
#include <pthread.h>
#include <xapian.h>
#include <vector>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
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
extern void* hurryScheduler(void* v);	
extern std::unordered_map<pthread_t, int> core_mapping;
extern int running;
extern int fd[24];
extern pthread_t hurryup;
	//bool sched;

#endif
