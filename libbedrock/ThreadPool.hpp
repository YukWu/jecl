#ifndef _HPP_THREADPOOL
#define _HPP_THREADPOOL

#include <algorithm>
#include <list>
#include <vector>
#include "bedrock_callbacks.h"
#include "Mutex.hpp"
#include "CondVariable.hpp"


namespace bedrock
{
	extern pthread_key_t  G_WORKER_KEY;

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

            friend class ThreadPool;
            ThreadPool*               _owner;
            int                       _threadkey; //线程key
            Mutex                     _lock;
            std::list<FunctorHolder>  _callbacks; //任务队列
            CondVariable              _callbacks_cv; //条件变量
            pthread_t                 _thread;
            bool                      _active; //线程是否运行中，由condvariable实现

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
