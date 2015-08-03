
#include "Timer.hpp"
#include "MutexProxy.hpp"
#include <signal.h>
using namespace bedrock;
using namespace std;

class P_FindEntryEq{
	int _key;
	public:
		P_FindEntryEq(int key):_key(key){}
		bool operator()(const Timer::Entry* e) const {
			return e->key == _key;
		}
};

class P_EntryComplete{
	int _counter;
	public:
		P_EntryComplete(int counter):_counter(counter){}
		bool operator()(Timer::Entry* e){
			if(_counter % e->timelapse == 0){
				if(e->runcount>0)
					e->runcount--;
				if(e->runcount==0){
					//Enqueue and delete afterward
					e->tpool->enqueue(e->fn,e->tpoolkey);
					delete e;
					return true;
				}
				else{
					//Enqueue and save afterward
					e->tpool->enqueue(e->fn,e->tpoolkey,false);
					return false;
				}
			}
			return false;
		}
};

template<class List,class Predicate>
void apply_remove_if(List& l, Predicate p){
	typename List::iterator first = l.begin();
	typename List::iterator last  = l.end();
	while(first!=last){
		typename List::iterator next  = first;
		++next;
		if(p(*first))
			l.erase(first);
		first = next;
	}
}


inline void deleteEntry(Timer::Entry* e){
	delete e->fn;
	delete e;
}

Timer::Timer(int interval):_next_key(1),_interval(interval),_counter(1){
	int r = pthread_create(&_thread,0,_run,this);
	assert(r==0);
}


void enqueueDeleteEntry(Timer::Entry* e){
	e->tpool->enqueue(callback::wrap(deleteEntry,e),e->tpoolkey);
}

Timer::~Timer(){
	pthread_exit(&_thread);
	struct sigaction act;
	act.sa_handle = NULL;
	act.sa_flags = 0;
	sigempty(&act.sa_mask);
	sigaction(SIGUSR1,&act,NULL);
	
	//walk the list and delete all the entries
	for_each(_entrylist.begin(),_entrylist.end(),enqueueDeleteEntry);
}

/**
 Schedule an event with this timer
 @param timelapse The delay before firing this event,or between event occurences.
 @param p         The threadpool in which to run this event.
 @param threadkey The threadpool key to use for this event.
 @param fn        The event to fire.
 @param runcount  The number of times to execute this event.Supply a value of -1(default) for infinite occurances.
 @returns         A key associated with this event that  can be used to unschedule the event at a later time.
 **/

int Timer::schedule(int timelapse,ThreadPool* p,int threadkey,callback::Functor* fn,int runcount){
	Entry* entry = new Entry;
	entry->runcount = runcount;
	entry->timelapse = timelapse;
	entry->tpool    = p;
	entry->tpoolkey = threadkey;
	entry->fn  =  fn;

	MutexProxy mp(_lock);
	entry->key = ++_next_key;
	_entrylist.push_back(entry);
	return entry->key;
}

/**
	Unschedule an event that was previously scheduled with this timer.
	@param key   The key associated with the event to cancel. This is the value returned by schedule when the event was initially scheduled.
**/

void Timer::unschedule(int key){
	MutexProxy mp(_lock);
	list<Entry*>::iterator it = find_if(_entrylist.begin(),_entrylist.end(),P_FindEntryEq(key));

	Entry* e = *it;
	_entrylist.erase(it);
	enqueueDeleteEntry(e);
}


/**
	This function gets the current value of the counter.Basically, the number of _interval seconds that have gone by since the Timer was created.This is useful for stamping an object's last access time and then timing it out if that stamp is a certain age.I put this in here because locking a mutex,getting a value ,and then unlocking the mutex is something on the order of 12 times faster than calling time(),at least on Linux.
**/
unsigned int Timer::getCounter(void){
	MutexProxy mp(_lock);
	return _counter;
}

void* Timer::_run(void* arg){
	Timer* timer = (Timer*)arg;
	timer->run();
	return NULL;
}

void Timer::_timer_hit(int signo,siginfo_t* t,void* arg){
	Timer* timer = (Timer*)((*t).si_value.si_ptr);
	MutexProxy mp(timer->_lock);

	apply_remove_if(timer->_entrylist,P_EntryComplete(timer->_counter));
	++timer->_counter;
}

void Timer::run(void){
	struct sigaction act;
	union sigval tsval;
	tsval.sival_ptr = this;
	act.sa_sigaction = _timer_hit;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	sigaction(SIGUSR1,&act,NULL);

	while(true){
		sleep(_interval);

		sigqueue(getpid(),SIGUSR1,tsval);
	}
}
