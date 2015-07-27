

#include "CondVariable.hpp"

using namespace bedrock;

ConVariable::ConVariable(Mutex& m):_mutex(m){
	_cond = PTHREAD_COND_INITIALIER;
}

ConVariable::~ConVariable(){
	pthread_cond_destroy(&_cond);
}

void ConVariable::signal(){
	pthread_cond_signal(&_cond);
}

void ConVariable::signalAll(){
	pthread_cond_broadcast(&_cond);
}

void ConVariable::wait(){
	pthread_cond_wait(&_cond,&mutex);
}


