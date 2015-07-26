#ifndef _HPP_THREADPOOL
#define _HPP_THREADPOOL

#include <algorithm>
#include <list>
#include <vector>
#include "bedrock_callbacks.h"
#include "Mutex.hpp"
#include "CondVariable.hpp"

#ifdef TEST
class ThreadPoolTest;
#endif

namespace bedrock
{
	//extern DWORD G_WORKER_KEY;

	/**
     * Provides an easy way to execute assorted functions 
     * in the context of a thread pool.
     */
    class ThreadPool
    {
    public:
        ThreadPool(unsigned int workers, bool active = true);
        ~ThreadPool();

        int  enqueue(bedrock::callback::Functor* cb, bool 
		     delete_functor = true);
        void enqueue(bedrock::callback::Functor* cb, int id, 
                     bool delete_functor = true);
		void enqueueSentinel(bedrock::callback::Functor* cb);
        int  getNextThreadID(void);
        void pause();
        void resume();

	static ThreadPool* getCurrentThreadPool();
	static int getCurrentThreadKey();
    private:
		// TESTER(::ThreadPoolTest)

        class Worker
        {
        public:
            Worker(ThreadPool* owner, int id, bool active = true);
            ~Worker();
    
            void enqueue(bedrock::callback::Functor* cb, bool delete_callback);
            void pause();
            void resume();

        protected:
            bedrock::callback::Functor* dequeue();
            static void*           run(void* arg);

        private:
            struct FunctorHolder
            {
                callback::Functor* fn;
                bool  delete_flag;
            };

            static void _releaseCallback(FunctorHolder& fh);

			//TESTER(::ThreadPoolTest)
            friend class ThreadPool;
			ThreadPool*               _owner;
			int                       _threadkey;
            Mutex                     _lock;
            std::list<FunctorHolder>  _callbacks;
            CondVariable              _callbacks_cv;
            pthread_t                 _thread;
            bool                      _active;

        static void _releaseWorker(Worker* w)
            { delete w; }
        static void _pauseWorker(Worker* w)
            { w->pause(); }
        static void _resumeWorker(Worker* w)
            { w->resume(); }
        static void _executeWorkerCallback(bedrock::callback::Functor* cb)
            { (*cb)();  delete cb; }
        };
    private:
        std::vector<Worker*> _workers;

        unsigned int    _current_worker;
        Mutex           _current_worker_lock;
    };

    inline void _findPoolAndKey(ThreadPool *& pool, int &key)
    {
        bool pool_defined = (pool != NULL);
        if (!pool_defined)
        {
            // If a NULL thread pool is provided, try and get the "current" one
            pool = ThreadPool::getCurrentThreadPool();
            assert(pool != NULL);
        }

        if (key == -1)
        {
            // if the calling function didn't pass in a threadpool, we use the current one
            if (!pool_defined)
            {
                key = ThreadPool::getCurrentThreadKey();
            }
            // otherwise, we round-robin
            else
            {
                key = pool->getNextThreadID();
            }
            assert(key != -1);
        }
    }
};
#endif //_HPP_THREADPOOL
