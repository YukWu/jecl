#include "ThreadPool.hpp"
#include "MutexProxy.hpp"
#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

using namespace bedrock;

unsigned long  G_counter = 0;
Mutex G_counter_lock;

void inc_counter(){
	MutexProxy mp(G_counter_lock);
	G_counter++;
}

// stress test

void* _stressTestEnqueueThread(void* arg){
	ThreadPool* p = (ThreadPool*)arg;

	std::cerr<<pthread_self()<<"  enqueue thread started"<<std::endl;
	for(int i=0;i<10000000;i++){
		p->enqueue(bedrock::callback::wrap(inc_counter));
	}
	std::cerr<<"\t"<<pthread_self()<<"  enqueue thread is end"<<std::endl;
	return NULL;
}

void stressTest(){
	G_counter=0;
	ThreadPool p(3);
	pthread_t id;
	for(int i=0;i<3;i++){
		pthread_create(&id,NULL,_stressTestEnqueueThread,&p);
	}
	G_counter_lock.lock();
	while(G_counter!=30000000){
		G_counter_lock.unlock();
		sleep(1);
		G_counter_lock.lock();
		std::cerr<<"G_counter is now : "<<G_counter<<std::endl;
	}
	G_counter_lock.unlock();
}

Mutex mut;
void* secondThreadFn(void* arg){
	CondVariable *cptr = (CondVariable*)arg;
	sleep(5);
	mut.lock();
	cptr->signal();
	mut.unlock();
	return NULL;
}

void wait(){
	CondVariable cond(mut);

	pthread_t id;
	pthread_create(&id,NULL,secondThreadFn,&cond);
	mut.lock();
	cond.wait();
	mut.unlock();
}


int main(void){
	stressTest();
	//wait();
	return 0;
}
