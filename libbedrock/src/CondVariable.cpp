

#include "CondVariable.hpp"

using namespace bedrock;

CondVariable::CondVariable(Mutex& m):_mutex(m){
	pthread_cond_init(&_cond,NULL);
}

CondVariable::~CondVariable(){
	pthread_cond_destroy(&_cond);
}

void CondVariable::signal(){
	pthread_cond_signal(&_cond);
}

void CondVariable::signalAll(){
	pthread_cond_broadcast(&_cond);
}

void CondVariable::wait(){
	pthread_cond_wait(&_cond,_mutex.getNativeAddress());
}


