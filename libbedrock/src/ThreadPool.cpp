

#include "ThreadPool.hpp"
#include <stddef.h>
#include <iostream>
#include <stdlib.h>

using namespace bedrock;
using namespace std;

pthread_key_t  bedrock::G_WORKER_KEY ;

class RefCountedSentinel{
	public:
		RefCountedSentinel(size_t initial_count,bedrock::callback::Functor* f): _ref_count(initial_count), _callback(f){}
		void releaseRefCount(){
			_ref_lock.lock();
			_ref_count--;
			if(_ref_count==0){
				(*_callback)();
				delete _callback;
				_ref_lock.unlock();
				delete this;
				return ;
			}
			_ref_lock.unlock();
		}

	private:
		Mutex              _ref_lock;
		size_t             _ref_count;
		callback::Functor* _callback;
};


/**
  Default constructor
  @param workers Number of threads this pool should use
  @param active  Indicates whether worker threads are 
         immediately created; defaults to TRUE
**/
ThreadPool::ThreadPool(unsigned int workers,bool active)
:_current_worker(0) {
	//size up workers vector
	_workers.reserve(workers);

	//initialize workers
	for(unsigned int i=0;i<workers;i++){
		_workers.push_back(new Worker(this,i,active));
	}
}

ThreadPool::~ThreadPool(){
	for_each(_workers.begin(),_workers.end(),Worker::_releaseWorker);
}

/**
 	Enqueue a callback for processing by any of the workers.
	No guarantee is made as to which worker will process this 
	callback

	@param cb   Callback object for worker thread to process
	@param delete_functor  Should the thread pool free the callback
		                   after call it?
	@returns ID of worker thread used for processing
 **/
int ThreadPool::enqueue(bedrock::callback::Functor* cb,bool delete_functor){
	int result = getNextThreadID();
	_workers[result]->enqueue(cb,delete_functor);
	return result;
}


/**
 	Enqueue a callback for processing with a particular worker.
	The key is used to select a worker; if the same key is used
	multiple times,requests will be processed in-order.

	@param cb  Callback object for worker thread to process
	@param id  ID of worker thread to process with
	@param delete_functor Should the thread pool free the callback after call it?
 **/

void ThreadPool::enqueue(bedrock::callback::Functor*cb ,int id, bool delete_functor){
	_workers[ id % _workers.size()]->enqueue(cb,delete_functor);
}

/**
 	Enqueue a speciel sentinal on all the threads in thread pool .
	When the sentinel has been processed by all threads,the user 
	supplied functor will be executed. This provides a way to 
	know when all the threads in the pool have processed past a 
	certain point.Note that cb will be executed on the last thread
	to process sentinel.

	@param cb User-supplied functor to execute after all threads process
	the sentinel.
 **/
void ThreadPool::enqueueSentinel(bedrock::callback::Functor* cb){
	RefCountedSentinel* r = new RefCountedSentinel(_workers.size(),cb);
	for(vector<Worker*>::iterator it = _workers.begin();it!=_workers.end();it++){
		(*it)->enqueue(callback::wrap(r,&RefCountedSentinel::releaseRefCount),true);
	}
}

/**
 	Get the next worker thread id for enqueing into.
	@return ID of a worker thread
 **/

int ThreadPool::getNextThreadID(void){
	_current_worker_lock.lock();

	int result = (_current_worker++) % _workers.size();

	_current_worker_lock.unlock();
	return result;
}

/**
 	pause worker threads. to restart them, call ThreadPool::reuse
 **/
void ThreadPool::pause(){
	for_each(_workers.begin(),_workers.end(),Worker::_pauseWorker);
}

/**
 	Resume paused worker threads.
 **/
void ThreadPool::resume(){
	for_each(_workers.begin(),_workers.end(),Worker::_resumeWorker);
}

/**
 	Retrieve the ThreadPool that the calling function is executing on.
	@returns ThreadPool pointer if the calling function is executing on
	a thread which exists within a pool; otherwise, returns NULL
 **/

ThreadPool* ThreadPool::getCurrentThreadPool(){
	ThreadPool::Worker* w = (ThreadPool::Worker*)pthread_getspecific(G_WORKER_KEY) ; //使用线程局部存储
	if(w!=NULL)
		return w->_owner;
	else
		return NULL;
}

/**
	Retrieve the thread key of a worker thread that the calling
	function is executing on.
	@return -1 if the calling function isn't executing on a thread
	within a ThreadPool; otherwise returns the thread key as an int value.
 **/

int ThreadPool::getCurrentThreadKey(){
	ThreadPool::Worker* w = (ThreadPool::Worker*)pthread_getspecific(G_WORKER_KEY);
	if(w!=NULL)
		return w->_threadkey;
	else
		return -1;
}


//-----------------------
// ThreadPool::Worker
//-----------------------

ThreadPool::Worker::Worker(ThreadPool* owner,int threadkey,bool active):
	_owner(owner),_threadkey(threadkey),_callbacks_cv(_lock),_active(active){
	int err;
	err = pthread_create(&_thread,NULL,ThreadPool::Worker::run,this);
	if(err!=0){
		cerr<<"Can't create thread"<<endl;
		exit(-1);
	}
}

void ThreadPool::Worker::_releaseCallback(FunctorHolder& fh){
	delete fh.fn;
}

ThreadPool::Worker::~Worker(){
	pthread_exit((void*)0);
	_lock.lock();
	for_each(_callbacks.begin(),_callbacks.end(),
			_releaseCallback);
	_lock.unlock();
}

void ThreadPool::Worker::enqueue(bedrock::callback::Functor* cb,bool delete_functor){
	_lock.lock();

	FunctorHolder fh = {cb,delete_functor};
	_callbacks.push_back(fh);
	if(_active)
		_callbacks_cv.signal();
	_lock.unlock();
}

void ThreadPool::Worker::pause(){
	_lock.lock();

	_active = false;

	_lock.unlock();
}

void ThreadPool::Worker::resume(){
	_lock.lock();

	_active = true;
	if(!_callbacks.empty()){
		_callbacks_cv.signal();
	}
	_lock.unlock();
}

// the stl containers are not at all thread safe , so you have to lock
// where you are manipulating them even if you know that you are the only
// one puling stuff out of them, and that you are only pulling stuff out
// that is already there.
void* ThreadPool::Worker::run(void* arg){
	Worker *worker = (Worker*)arg;
	pthread_key_create(&G_WORKER_KEY,NULL);
	pthread_setspecific(G_WORKER_KEY,worker);
	worker->_lock.lock();
	while(1){
		while(!worker->_callbacks.empty() && worker->_active){
			FunctorHolder fh = worker->_callbacks.front();
			worker->_callbacks.pop_front();
			worker->_lock.unlock();
			(*(fh.fn))();
			if(fh.delete_flag){
				delete fh.fn;
			}
			worker->_lock.lock();
		}
		worker->_callbacks_cv.wait();
	}
	return 0;
}
