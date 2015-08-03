
#include "Application.hpp"
#include "MutexProxy.hpp"
#include <iostream>
#include <assert.h>


using namespace bedrock;
using namespace std;

Application* Application::_app_instance = NULL;

void Application::exit(int ec,const string& message){
	cerr<<"Application exit requested("<< ec <<")"<<endl;
	if(!message.empty()){
		cout<<" Exit message: " <<message << endl;
	}
	::exit(ec);
}

int Application::start(){
	if(_app_instance == NULL){
		_app_instance = new Application();
	}
	assert(_app_instance->_started==false);

	//Lock the mutex and wait to get signaled
	MutexProxy mp(_app_instance->_shutdown_lock);
	_app_instance->_started = true;
	while(!_app_instance->_shutting_down){
		_app_instance->_shutdown_signal.wait();
	}

	_app_instance->_started = false;
	cout<< " Application stop requested ("<<_app_instance->_shutdown_code<<")"<<endl;
	if(!_app_instance->_shutdown_msg.empty())
		cout<<"Shutdown message: " << _app_instance->_shutdown_msg<<endl;
	return _app_instance->_shutdown_code;
}


void Application::stop(int rc,const string& message){
	assert(_app_instance!=NULL);
	assert(_app_instance->_started == true);

	MutexProxy mp(_app_instance->_shutdown_lock);
	_app_instance->_shutting_down = true;
	_app_instance->_shutdown_code = rc;
	_app_instance->_shutdown_msg  = message;
	_app_instance->_shutdown_signal.signal();
}

void Application::registerShutdownSignal(int signal_id){
	if(_app_instance==NULL){
		_app_instance = new Application();
	}
	assert(_app_instance->_started ==false);
}
