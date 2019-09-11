/*
thread-aware fine-grained het mapper
vinicius petrucci, ufba, 2016
*/
//g++ -std=c++11 -o sched scheduler.cpp


#include <sys/time.h>
#include <time.h>
#include <assert.h> 
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <deque>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <memory>
#include <algorithm> 
#include <functional>
#include <cctype>
#include <locale>
#include <iterator>
#include <tuple>
#include <map>
#include <cstring>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>


#define NUM_CORES             8
#define BIG_CORE_TYPE         0
#define SMALL_CORE_TYPE       1

#define LATENCY_TARGET        200  // ms
#define SAMPLING_THRESHOLD    0.2
#define TOOK_THRESHOLD        0.7

#define ENABLE_SCHED 		  0

using namespace std;

typedef tuple<int,int,int,int> tuple_t; 

unordered_map<int, int> thread_mapping;        

string exec_output(const char* cmd) {
	shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
	if (!pipe) return "ERROR";
	char buffer[2048];
	string result = "";
	while (!feof(pipe.get())) {
		if (fgets(buffer, 2048, pipe.get()) != NULL)
			result += buffer;
	}
	return result;
}

int which_core(int pid)
{
	stringstream out;
	out  << pid;
	string spid;
	spid  = out.str();
	ostringstream oss;
	oss << "/proc/" << spid << "/stat";
	string soss = oss.str();
	ifstream file(soss.c_str());  
	if (!file) {
		cerr << "cannot open /proc/" << spid << "/stat" << endl;
		return -1;
	}
	string tokens;
	getline(file, tokens);
	file.close();
	istringstream ss(tokens);
	string core_num;
	for (int i = 0; i <= 38; ++i)
		ss >> core_num;
	return atoi(core_num.c_str());
}

// ARM big.LITTLE SAMSUNG EXYNOS
// 0-3 big cores
// 4-7 small cores
void thread_to_core_type(int pid, int core_type)
{
	if (core_type == BIG_CORE_TYPE) {
		cpu_set_t  mask;
		CPU_ZERO(&mask);
		CPU_SET(0, &mask);
		CPU_SET(1, &mask);     
		CPU_SET(2, &mask);     
		CPU_SET(3, &mask);     
		if (sched_setaffinity(pid, sizeof(mask), &mask) < 0)
			perror("sched_setaffinity");
	} else if (core_type == SMALL_CORE_TYPE) {
		cpu_set_t  mask;
		CPU_ZERO(&mask);
		CPU_SET(4, &mask);
		CPU_SET(5, &mask);
		CPU_SET(6, &mask);
		CPU_SET(7, &mask);
		if (sched_setaffinity(pid, sizeof(mask), &mask) < 0)
			perror("sched_setaffinity");
	}
}

string which_core_str(int core)
{
	if (core >=0 and core <= 3) return "big";
	if (core >=4 and core <= 7) return "LITTLE";
}

void thread_to_core(int pid, int core)
{
	cpu_set_t  mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);
	if (sched_setaffinity(pid, sizeof(mask), &mask) < 0)
		perror("sched_setaffinity");

	thread_mapping[pid] = core;
}

void init_thread_map(unordered_map<string, unsigned int> thread_pids)
{
	int core = 0;
	for(auto const &ent : thread_pids) {
		thread_to_core(ent.second, core);
		cout << "mapping thread " << ent.first << "(" << ent.second << ") to core " << core << endl;
		core++;
	}
}

long int get_time_ms()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int time_ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	return time_ms;
}

void sleep_ms(long int ms)
{
	nanosleep((const struct timespec[]){{0, ms*000000L}}, NULL);
}

void insert_deq(string s, deque<string> &deq, int max_size)
{
	if (deq.size() > max_size) {
		deq.pop_front();
		assert(deq.size() == max_size);
	}  
	deq.push_back(s);
}


size_t getFilesize(const char* filename) {
	struct stat st;
	stat(filename, &st);
	return st.st_size;
}


int main(int argc, char const *argv[]) {
	string line_in;
	long int t1_ms = 0;
	long int t2_ms = 0;  
	long int diff_time_ms;  
	deque<string> str_deq;
	int max_deq_size = 5000;
	typedef vector< tuple_t > tuple_list; 
	unordered_map<string, unsigned int> thread_pids; 
	unordered_map<string, tuple_t> pending_reqs;    
	int es_pid = -1; 


	//map<int, pair<int, int>, greater<int>> big_threads_node0, big_threads_node1;
	int big_threads[2][2];
	map<int, pair<int, int>> small_threads_node0, small_threads_node1;


	string cmd_out = exec_output("jps | grep Elasticsearch | cut -f1 -d' '"); 
	cmd_out.erase(remove(cmd_out.begin(), cmd_out.end(), '\n'), cmd_out.end());
	
	if (cmd_out != "")
		es_pid = atoi(cmd_out.c_str());

	if (es_pid == -1) {
		cout << "elasticsearch process not found!" << endl;
		exit(2);
	}

	cout << "elasticsearch pid: " << es_pid << endl;      

	string cmd = "jstack " + cmd_out + " |  egrep '[[]search[]]' | sed -n 's/.*#\\([0-9]*\\).*nid=\\([xa-f0-9]*\\).*/\\1,\\2/p'";

	cmd_out = exec_output(cmd.c_str()); 
	
	if (cmd_out == "") {
		cout << "elasticsearch has no search threads" << endl;
		exit(3);
	}

	vector<string> lines;    
	stringstream ss(cmd_out);
	while (ss.good()) {
		string substr;
		getline(ss, substr, '\n');
		lines.push_back(substr);
	}      

	for (string l : lines) {
		if (l.length() <= 1) continue;
		stringstream ss(l);
		string substr1, substr2;    
		getline(ss, substr1, ',');
		getline(ss, substr2, ',');
		string thread_id = substr1;
		string thread_nid = substr2;
		unsigned int thread_nid_int = stoul(thread_nid, nullptr, 16);    
		thread_pids[thread_id] = thread_nid_int;
	}

	init_thread_map(thread_pids);

//	t1_ms = get_time_ms();

	while (1) {

//		t2_ms = get_time_ms();

//		diff_time_ms = t2_ms - t1_ms;

		sleep_ms(SAMPLING_THRESHOLD * LATENCY_TARGET);
		//if (diff_time_ms >= SAMPLING_THRESHOLD * LATENCY_TARGET) {
		if (1) {
			
			/* This code reads all the data from ES that was saved on a mmap,
			and separate it into big and small */
			size_t filesize = getFilesize(argv[1]);
			int fd = open(argv[1], O_RDONLY, 0);
			assert(fd != -1);
			char* mmappedData = static_cast<char*>(mmap(NULL, filesize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0));
			assert(mmappedData != MAP_FAILED);

			//It's necessary to transfer the mmapped data to a new char* because strtok doesn't work with it-
			char *strmmap = strdup(mmappedData);
			char *strData = strtok(strmmap, "#");
			strData = strtok(strData, ",");
			unsigned long int timestamp;
			string tid;


			/* 
				Separete the threads by node 
				Node 0 				Node 1
				Small - 0 			Small - 1
				Bigs - 2, 4, 6 		Bigs - 3, 5, 6
			*/
			do {
				timestamp = strtoul(strData, NULL, 0);
				strData = strtok(NULL, ",");
				tid = strData;

				int threadid = thread_pids[tid];
				int coreid = thread_mapping[threadid];

				if (coreid < 2) {
					big_threads[coreid][0] = timestamp;
					big_threads[coreid][1] = threadid;
				}
				else {
					if ((coreid % 2) == 0)
						small_threads_node0[timestamp] = make_pair(threadid, coreid);
					else
						small_threads_node1[timestamp] = make_pair(threadid, coreid);
				}

			} while((strData = strtok(NULL, ",")) != NULL);

			int rc = munmap(mmappedData, filesize);
			assert(rc == 0);
			close(fd);

			if (ENABLE_SCHED) {
				
				/* NODE 0 SWAP */
				int ts_small = small_threads_node0.begin()->first;
				int tid_small = small_threads_node0.begin()->second.first;
				int coreid_small = small_threads_node0.begin()->second.second;

				long int cur_ms = get_time_ms();

				if ( (cur_ms - ts_small > TOOK_THRESHOLD * LATENCY_TARGET)  && (ts_small < big_threads[0][0]) ) {
					thread_to_core(tid_small, 0);
					thread_to_core(big_threads[0][1], coreid_small);
					cout << "swapping: threadid small to big: " << tid_small << ", threadid big to small: " << big_threads[0][1] << "\n";
				}



				/* NODE 1 SWAP */
				ts_small = small_threads_node1.begin()->first;
				tid_small = small_threads_node1.begin()->second.first;
				coreid_small = small_threads_node1.begin()->second.second;

				//if (ts_small < big_threads[1][0]) {
				if ( (cur_ms - ts_small > TOOK_THRESHOLD * LATENCY_TARGET)  && (ts_small < big_threads[1][0]) ) {
					thread_to_core(tid_small, 0);
					thread_to_core(big_threads[1][1], coreid_small);
					cout << "swapping: threadid small to big: " << tid_small << ", threadid big to small: " << big_threads[1][1] << "\n";
				}
			}
		}
	}	
}
