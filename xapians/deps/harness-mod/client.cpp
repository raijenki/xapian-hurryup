/** $lic$
 * Copyright (C) 2016-2017 by Massachusetts Institute of Technology
 *
 * This file is part of TailBench.
 *
 * If you use this software in your research, we request that you reference the
 * TaiBench paper ("TailBench: A Benchmark Suite and Evaluation Methodology for
 * Latency-Critical Applications", Kasture and Sanchez, IISWC-2016) as the
 * source in any publications that use this software, and that you send us a
 * citation of your work.
 *
 * TailBench is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.
 */

#include "client.h"
#include "helpers.h"
#include "tbench_client.h"

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <math.h>
#include <thread>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include<fstream>
#include<time.h>
#include<stdio.h>
#include<iostream>
#include<unistd.h>
#include <queue>
#include <tuple>

bool read_file= false;
bool read_file2= false;
double current_qps = 0;
double newQPS = 0;

/*******************************************************************************
 * Dynamic QPS Lookup
 *******************************************************************************/
DQPSLookup::DQPSLookup(){
    const char* op_val= getenv("TBENCH_INPUT");
    std::string inputFile(op_val);
    std::cout << "[Nishtala-debug] input file " << inputFile << std::endl;
    started = false;
    std::ifstream infile(inputFile.c_str());
    

    uint64_t duration;
    double QPS;
    while(infile >> duration >> QPS) {
        std::cout<<"[nishtala-debug] input test :"<< duration << " " << QPS << std::endl;
        QPStiming.push(new QPScombo(duration, QPS));
    }
    if (QPStiming.empty()) {
        std::cerr <<"No input found, will use TBENCH_QPS as parameter\n";
    }
    startingNs = getCurNs();
    //printf("1 startingNs: %lu", startingNs);
}

double DQPSLookup::currentQPS() {
    if (QPStiming.empty()) {
        return -1;
    }
    uint64_t currentNs = getCurNs();
    uint64_t timing_run = QPStiming.front()->getDuration()*1000*1000*1000;
    uint64_t difference = currentNs - startingNs;
    // DEBUG: printf("dif: %lu, timing: %lu\n", difference, timing_run);

    if(difference > timing_run) {
	printf("entered");
        QPStiming.pop();
        if (QPStiming.empty()){
            return -1;
        }
        startingNs = getCurNs();
        return QPStiming.front()->getQPS();
    } else{
        return QPStiming.front()->getQPS();
    }
}


void DQPSLookup::setStartingNs(){
    if (!started) {
        started = true;
        startingNs = getCurNs();
    }
}

bool Client::getDumped(){
    return dumped;
}

bool is_empty(std::ifstream& pFile)
{
        return pFile.peek() == std::ifstream::traits_type::eof();
}

/*int Client::readfile(){
    const char* pid_file = getenv("TBENCH_CPID");
    std::string pidFile(pid_file);
    sleepUntil(100000000);
    unsigned long int pid_num=0;
    std::ifstream infile(pidFile.c_str());
    if (is_empty(infile)){
        std::cerr<<"file empty" << std::endl;
    }
    if (infile.good())
    {
        std::string sLine;
        getline(infile, sLine);
        pid_num = std::atoi(sLine.c_str());
        std::cout << "pid in int " << pid_num << " " << sLine << std::endl;
        return pid_num;
    }
    infile.close();
}*/

/*******************************************************************************
 * Client
 *******************************************************************************/

Client::Client(int _nthreads) : dqpsLookup() {
    //"input.test"){
    //getOpt("TBENCH_INPUTPATH", "input.test")){
    status = INIT;
    
    nthreads = _nthreads;
    pthread_mutex_init(&lock, nullptr);
    pthread_barrier_init(&barrier, nullptr, nthreads);
    
    minSleepNs = getOpt("TBENCH_MINSLEEPNS", 0);
    seed = getOpt("TBENCH_RANDSEED", 0);
    lambda = getOpt<double>("TBENCH_QPS", 1000.0) * 1e-9;
    
    std::string outputFile = getOpt<std::string>("TBENCH_OUTPUT","/home/cc/hurryup-dvfs/bin/rps.txt");
    std::string outputFile_keyservice = getOpt<std::string>("TBENCH_KEYSERVICE","/home/cc/hurryup-dvfs/bin/percentil.txt");
    //FILE* 
    fileOut = fopen(outputFile.c_str(), "w");
    fileOut_keyservice = fopen(outputFile_keyservice.c_str(), "w");
    fprintf(fileOut_keyservice,"%s %s %s %s\n", "keylength","QueueTimes","ServiceTimes","SojournTimes");
    fprintf(fileOut,"%s, %s, %s, %s, %s \n", "rps", "q90", "q95", "q99", "timestampNs");
    fflush(fileOut);
    fflush(fileOut_keyservice);
    
    dist = nullptr; // Will get initialized in startReq()
    startedReqs = 0;

    tBenchClientInit();
}

Request* Client::startReq() {
    if (status == INIT) {
        pthread_barrier_wait(&barrier); // Wait for all threads to start up

        pthread_mutex_lock(&lock);

        if (!dist) {
            uint64_t curNs = getCurNs();
            dist = new ExpDist(lambda, seed, curNs);

            status = WARMUP;

            pthread_barrier_destroy(&barrier);
            pthread_barrier_init(&barrier, nullptr, nthreads);
        }

        pthread_mutex_unlock(&lock);

        pthread_barrier_wait(&barrier);
    }

    pthread_mutex_lock(&lock);

    if (status == ROI) {
        double newQPS = dqpsLookup.currentQPS();
        if (current_qps != newQPS) {
            current_qps = newQPS;
            lambda = current_qps * 1e-9;
            delete dist;
            uint64_t curNs = getCurNs();
            dist = new ExpDist(lambda, seed, curNs);
        }
    }

    Request* req = new Request();
    size_t len = tBenchClientGenReq(&req->data);
    req->len = len;

    req->id = startedReqs++;
    req->genNs = dist->nextArrivalNs();
    inFlightReqs[req->id] = req;

    pthread_mutex_unlock(&lock);

    uint64_t curNs = getCurNs();
    //RN: What's the interarrival time (lambda)
    //const pthread_t this_id = pthread_self();
    //std::cout << "thread_id=" << this_id << " seed=" << seed << " lambda=" << lambda << " curNs=" << curNs << " diff=" << req->genNs - curNs << std::endl;
    if (curNs < req->genNs) {
        sleepUntil(std::max(req->genNs, curNs + minSleepNs));
    }

    return req;
}

void Client::finiReq(Response* resp) {//, FILE *fp){
    pthread_mutex_lock(&lock);
    
    auto it = inFlightReqs.find(resp->id);
    assert(it != inFlightReqs.end());
    Request* req = it->second;

    if (status == ROI) {
        counter++;
        uint64_t curNs = getCurNs();
        
        assert(curNs > req->genNs);

        uint64_t sjrn = curNs - req->genNs;
        assert(sjrn >= resp->svcNs);
        uint64_t qtime = sjrn - resp->svcNs;
        
        unsigned long KeyLength = req->len;
        
        KeyLengths.push_back(KeyLength);

        queueTimes.push_back(qtime);
        svcTimes.push_back(resp->svcNs);
        sjrnTimes.push_back(sjrn);
    
        int RPS = sjrnTimes.size();        
        //int pid = getpid();

        //if ((!read_file) && (counter == current_qps * 1.0)){
            //pid_num = readfile();
           // read_file=true;
        //} 
        
       
        if ((counter == current_qps * 1.0) && (RPS > 0)) {
            counter = 0;
            double q90, q95, q99 = 0.0;
            std::sort(sjrnTimes.begin(), sjrnTimes.end());
            int Index90 = (int)ceil(0.90 * double(RPS));
            int Index95 = (int)ceil(0.95 * double(RPS));
            int Index99 = (int)ceil(0.99 * double(RPS));
            q90 = sjrnTimes.at(Index90-1)/1e6;
            q95 = sjrnTimes.at(Index95-1)/1e6;
            q99 = sjrnTimes.at(Index99-1)/1e6;
            uint64_t curNs = getCurNs();
            fprintf(fileOut, "%d, %.3f, %.3f, %.3f, %lld\n", RPS, q90, q95, q99, (long long)curNs);
            fflush(fileOut);
            
            int reqs = sjrnTimes.size();
            //for (int r = 0; r< reqs; r++) {
               //fprintf(fileOut_keyservice, "%.3f, %.3f, %.3f, %.3f\n", KeyLengths[r], queueTimes[r], svcTimes[r], sjrnTimes[r]);
            //}
            //fflush(fileOut_keyservice);
            queueTimes.clear();
            svcTimes.clear();
            sjrnTimes.clear();
            KeyLengths.clear();
        }
    }

    delete req;
    inFlightReqs.erase(it);
    pthread_mutex_unlock(&lock);
}

void Client::_startRoi() {
    assert(status == WARMUP);
    status = ROI;
    counter = 0;
    dqpsLookup.setStartingNs();
    queueTimes.clear();
    svcTimes.clear();
    sjrnTimes.clear();
    KeyLengths.clear();
}

int Client::startRoi() {
    //pthread_mutex_lock(&lock);
    //int pid = getpid();
    _startRoi();
    //pthread_mutex_unlock(&lock);
    //return pid;
}

void Client::dumpStats() {
    pthread_mutex_lock(&lock);
    if (dumped == true) {
        pthread_mutex_unlock(&lock);
    }
    std::ofstream out("lats.bin", std::ios::out | std::ios::binary);
    int reqs = sjrnTimes.size();

    for (int r = 0; r < reqs; ++r) {
        //std::cout <<"[Nishtala-debug] queuetimes" << queueTimes[r] << std::endl;
        out.write(reinterpret_cast<const char*>(&queueTimes[r]), 
                    sizeof(queueTimes[r]));
        out.write(reinterpret_cast<const char*>(&svcTimes[r]), 
                    sizeof(svcTimes[r]));
        out.write(reinterpret_cast<const char*>(&sjrnTimes[r]), 
                    sizeof(sjrnTimes[r]));
    }
    out.close();
    dumped = true;
    pthread_mutex_unlock(&lock);
    fclose(fileOut);
}

/*******************************************************************************
 * Networked Client
 *******************************************************************************/
NetworkedClient::NetworkedClient(int nthreads, std::string serverip, 
        int serverport) : Client(nthreads)
{
    pthread_mutex_init(&sendLock, nullptr);
    pthread_mutex_init(&recvLock, nullptr);

    // Get address info
    int status;
    struct addrinfo hints;
    struct addrinfo* servInfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::stringstream portstr;
    portstr << serverport;
    
    const char* serverStr = serverip.size() ? serverip.c_str() : nullptr;

    if ((status = getaddrinfo(serverStr, portstr.str().c_str(), &hints, 
                    &servInfo)) != 0) {
        std::cerr << "getaddrinfo() failed: " << gai_strerror(status) \
            << std::endl;
        exit(-1);
    }

    serverFd = socket(servInfo->ai_family, servInfo->ai_socktype, \
            servInfo->ai_protocol);
    if (serverFd == -1) {
        std::cerr << "socket() failed: " << strerror(errno) << std::endl;
        exit(-1);
    }

    if (connect(serverFd, servInfo->ai_addr, servInfo->ai_addrlen) == -1) {
        std::cerr << "connect() failed: " << strerror(errno) << std::endl;
        exit(-1);
    }

    int nodelay = 1;
    if (setsockopt(serverFd, IPPROTO_TCP, TCP_NODELAY, 
                reinterpret_cast<char*>(&nodelay), sizeof(nodelay)) == -1) {
        std::cerr << "setsockopt(TCP_NODELAY) failed: " << strerror(errno) \
            << std::endl;
        exit(-1);
    }
}

bool NetworkedClient::send(Request* req) {
    pthread_mutex_lock(&sendLock);

    int len = sizeof(Request) - MAX_REQ_BYTES + req->len;
    int sent = sendfull(serverFd, reinterpret_cast<const char*>(req), len, 0);
    if (sent != len) {
        error = strerror(errno);
    }

    pthread_mutex_unlock(&sendLock);

    return (sent == len);
}

bool NetworkedClient::recv(Response* resp) {
    pthread_mutex_lock(&recvLock);

    int len = sizeof(Response) - MAX_RESP_BYTES; // Read request header first
    int recvd = recvfull(serverFd, reinterpret_cast<char*>(resp), len, 0);
    if (recvd != len) {
        error = strerror(errno);
        return false;
    }

    if (resp->type == RESPONSE) {
        recvd = recvfull(serverFd, reinterpret_cast<char*>(&resp->data), \
                resp->len, 0);

        if (static_cast<size_t>(recvd) != resp->len) {
            error = strerror(errno);
            return false;
        }
    }

    pthread_mutex_unlock(&recvLock);

    return true;
}

