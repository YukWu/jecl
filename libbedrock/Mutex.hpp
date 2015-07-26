


#ifndef _HPP_MUTEX_LINUX_
#define _HPP_MUTEX_LINUX_

#include <pthread.h>

namespace bedrock{
	/*
	 	wrapper object for pthread mutex
	 */
	class Mutex{
		public:
			Mutex(){
				pthread_mutex_init(&_lock,NULL);
			}
			~Mutex(){
				pthread_mutex_destroy(&_lock);
			}
			void lock() {  pthread_mutex_lock(&_lock);}
			void unlock(){ pthread_mutex_unlock(&_lock);}
			pthread_mutex_t getNative(){return _lock;}

		private:
			pthread_mutex_t _lock;
	};
};

#endif //_HPP_MUTEX_LINUX
