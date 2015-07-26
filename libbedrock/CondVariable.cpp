

#include "CondVariable.hpp"

using namespace bedrock;

ConVariable::ConVariable(Mutex& m):_mutex(m){
	_cond = PTHREAD_COND_INITIALIER;

}

ConVariable::~ConVariable(){
	
}

void ConVariable::signal(){

}

void ConVariable::signalAll(){

}

void ConVariable::wait(){
	pthread_cond_wait(&_cond,&mutex);
}


