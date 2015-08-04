

#ifndef _HPP_APPLICATION
#define _HPP_APPLICATION

#include <string>
#include "Mutex.hpp"
#include "CondVariable.hpp"
#include <stdlib.h>


namespace bedrock{
	/**
	 application loop singleton. This object provides proper signal
	 handling for threaded programs and a way for controller
	 ojbects to shutdown the application in a graceful way.
	 **/
	 class Application{
	 	public:
			/**
			 Put the current thread to sleep until it is time to exit
			 the current application
			 @see Application::stop
			 **/
			static int start();

			/**
			 Kick the main thread and tell it to wakeup and complete any
			 processing necessary for shutting down. You must call Application::start before calling this method. If you do not, this function will ASSERT.
			 @param rc Code to return from Application::start@
			 @param message Optional message describing why this shutdown is occuring
			 **/
			static void stop(int rc,const std::string& message = "");

			/**
			 	Exit immediately
				@param ec error code
				@param message Textual message description why this shutdown
				is occouring.
			 **/
			static void exit(int ec,const std::string& message="");

			/**
			 Register a OS signal which should fire an Application::stop
			 event. Do not call this function after calling Application::start
			 **/
			static void registerShutdownSignal(int signal);
		private:
			//Hide all constructors so no one will try and instantiate
			//an Application object
			Application():_shutdown_signal(_shutdown_lock),_shutdown_code(0),_started(false),_shutting_down(false){}
			~Application();
			Application(const Application& a);
			Application& operator=(const Application& a);

			//static singleton instance
			static Application* _app_instance;

			//private members
			Mutex              _shutdown_lock;
			CondVariable       _shutdown_signal;
			int                _shutdown_code;
			std::string        _shutdown_msg;
			bool               _started;
			bool               _shutting_down;
	};


};
#endif //_HPP_APPLICATION
