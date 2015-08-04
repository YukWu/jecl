
#ifndef _HPP_TIMER
#define _HPP_TIMER

#include "ThreadPool.hpp"
#include <algorithm>
#include <list>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <errno.h>


namespace bedrock{
	class Timer{
		public:
			Timer(int interval);
			~Timer();

			int schedule(int timelapse,ThreadPool* p,int threadkey,callback::Functor* fn,int runcount=-1);
			void unschedule(int key);
			unsigned int getCounter(void);

			struct Entry{
				int runcount;
				int timelapse;
				ThreadPool* tpool;
				int tpoolkey;
				callback::Functor* fn;
				int key;
			};
		protected:
			static void* _run(void* arg);
			//static void  _timer_hit(int,siginfo_t*,void*);
			static void  _hit(void* arg);
			void run();
			static void  _releaselock(void* arg);

		private:
			Timer(const Timer&);
			Timer& operator=(const Timer& s);

			pthread_t    _thread; //thread create to loop and send out timer events
			Mutex        _lock;
			std::list<Entry*> _entrylist;
			int          _next_key;
			unsigned int _counter;
			const int    _interval;
			bool         _runstate;
	};
}

#endif //_HPP_TIMER
