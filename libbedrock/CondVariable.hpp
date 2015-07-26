

#ifndef _HPP_CONDVARIABLE
#define _HPP_CONDVARIABLE

#include "Mutex.hpp"

namespace bedrock{
	
	class CondVariable{
		public:
			CondVariable(Mutex& m);
			~CondVariable();

			void signal();
			void signalAll();
			void wait();
		protected:
			static void cancellation_cleanup(void* arg);
		private:
			/*用于同步条件变量的锁对象*/
			Mutex _mutex;
			/* posix 条件变量 */
			pthread_cond_t _cond;
	};
};


#endif //_HPP_CONDVARIABLE
