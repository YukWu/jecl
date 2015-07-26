
#ifndef _HPP_MUTEXPROXY
#define _HPP_MUTEXPROXY

#include "Mutex.hpp"

namespace bedrock{
	
	/*
	 Proxy object for ensuring that a Mutex gets unlocked 
	 when the stack unwinds.
	 */
	class MutexProxy{
		public :
			MutexProxy(Mutex& m):_m(m){ _m.lock();}
			~MutexProxy(){ _m.unlock(); }
		private:
			Mutex &_m;
	};
};

#endif //_HPP_MUTEXPROXY
