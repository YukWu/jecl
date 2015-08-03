//============================================================================
// Project:       JAX -- Jabber Access eXtensions
// Filename:      jax.hpp
// Description:   Primary header
// Created at:    Thu Jul 19 10:00:55 2001
// Modified at:   Thu Feb 21 14:36:30 2002
//
//   License:
// 
// The contents of this file are subject to the Jabber Open Source License
// Version 1.0 (the "License").  You may not copy or use this file, in either
// source code or executable form, except in compliance with the License.  You
// may obtain a copy of the License at http://www.jabber.com/license/ or at
// http://www.opensource.org/.  
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the License
// for the specific language governing rights and limitations under the
// License.
//
//   Copyrights
//
// Portions created by or assigned to Jabber.com, Inc. are 
// Copyright (c) 2001-2002 Jabber.com, Inc.  All Rights Reserved.  
// 
// $Id: jax.hpp,v 1.1 2002/04/30 11:39:34 akuma Exp $
//============================================================================
#ifndef _H_JAX
#define _H_JAX

#include <string>

#include "bedrock.h"
#include "judo.hpp"


/**
 * Core namespace for Jabber Access eXtensions
 */
namespace jax
{

    /**
     * Wrapper class for Jabber packets
     */
    class Packet
    {
    public:
	Packet(judo::Element* elem)
	    : _element(elem)
	    {}

	virtual ~Packet() 
	    { delete _element; }

	judo::Element* getElement(void)
	    { return _element; }

	std::string toString(void)
	    { return _element->toString(); }

	static Packet* create(judo::Element* elem)
	    { return new Packet(elem); }

	static std::string extractHostname(const std::string& jid)
	    {
		std::string::size_type d1 = jid.find("@"); // Search for @ divider
		std::string::size_type d2 = jid.find("/"); // Search for / divider
		if ((d1 != std::string::npos) && (d2 != std::string::npos))
		    return jid.substr(d1 + 1, d2 - d1 - 1);
		else if ((d1 != std::string::npos) && (d2 == std::string::npos))
		    return jid.substr(d1 + 1, std::string::npos);
		else if ((d1 == std::string::npos) && (d2 != std::string::npos))
		    return jid.substr(0, d2);
		else
		    return jid;
	    }
	
	virtual void swapAddresses()
	    {
		std::string tmp = _element->getAttrib("from");
		_element->putAttrib("from", _element->getAttrib("to"));
		_element->putAttrib("to", tmp);
	    }

	void addErrorInfo(unsigned int code,
			  const std::string& message)
	    {
		_element->putAttrib("type", "error");
		judo::Element* error = _element->addElement("error", message);
		char buf[15];
#ifdef WIN32
        _snprintf(buf, 15, "%d", code);
#else
		snprintf(buf, 15, "%d", code);
#endif
		error->putAttrib("code", std::string(buf));
	    }
    private:
	judo::Element* _element;
    };

    /**
     * Represents a connection to a Jabber XML router. 
     */
    template<typename P, typename PFactory>
    class RouterConnection 
        : public judo::ElementStreamEventListener, 
	  public bedrock::net::SocketEventListener
    {
    public:    
	class EventListener;

       /**
	* Default constructor. 
	* @param watcher The socket watcher managing our internal connection
	* @param listener Object which will be receiving events related to the 
	* connection
	* @param outgoing Indicates whether the connection will
	* @param threadkey The thread identifier this router connection will 
	* use.
	* connect to the router by connecting <b>to</b> the router
	* (outgoing == true) or by accepting a connection <b>from</b>
	* the router (outgoing == false).
	*/
        RouterConnection(bedrock::net::SocketWatcher &watcher, 
			 EventListener& listener, 
			 bool outgoing, unsigned int threadkey)
	    : _watcher(watcher), _outgoing(outgoing), 
	      _listener(listener), _socket(NULL), _state(Disconnected), 
	      _state_cv(_state_lock), _threadkey(threadkey)
	    {
            _stream = new judo::ElementStream(this);
        }
        virtual ~RouterConnection();

        void connect(const std::string &name, const std::string &secret, 
		     bedrock::net::Address& a, bedrock::ThreadPool* tpool = 0,
                     int threadkey = -1);    
        void disconnect();
        void deliver(P* p);
	
        struct exception 
        {
            class SocketError 
            {
            public:
                SocketError(int err) : _err(err) {}
                int _err;
            };
        };

	/**
	 * Interface which allows a RouterConnection to access require
	 * information from a controlling object.
	 */
	class EventListener
	{
	public:    
	    virtual void onRouterConnected() = 0;
	    virtual void onRouterDisconnected() = 0;
	    virtual void onRouterError() = 0;
	    virtual void onRouterPacket(P* p) = 0;
	    virtual ~EventListener() {}
	};


    protected:
        // judo::ElementStream impl
        void onDocumentStart(judo::Element* elem);
        void onElement(judo::Element* elem);
        void onDocumentEnd();

        // bedrock::net::SocketEventListener impl
        bool onAccept(bedrock::net::Socket* newsocket);
        void onConnected(bedrock::net::Socket *sock);
        void onClose(bedrock::net::Socket *sock);
        void onInitialize(bedrock::net::Socket *sock);
        void onError(bedrock::net::Socket *sock, 
		     bedrock::net::Socket::exception::ErrorCode ec, int err);
        bool onRead(bedrock::net::Socket *sock, const char* buf, int buf_sz);
    private:
        TESTER(RouterConnectionTest)

        enum State { 
            Disconnected, 
            Connecting, 
            Connected,
            Error
        };

	judo::ElementStream   *_stream;
	bedrock::net::SocketWatcher& _watcher;
        bool                  _outgoing;
        EventListener&        _listener;
	std::string                _name;
	std::string                _secret;
        bedrock::net::Socket* _socket;
        State                 _state;
        bedrock::Mutex        _state_lock;
        bedrock::CondVariable _state_cv;
	unsigned int          _threadkey;
	std::string                _handshake_id;
    };

    class Component
    {
    public:
	/**
	 * Callback point after configuration information has been loaded
	 * from router.
	 * @param config the configuration as retrieved from the router plus
	 *               a few extra children elements.  These children are:
	 *               "jax::Component::name" -- service id
	 *               "jax::Component::secret" -- secret
	 *               "jax::Component::outgoing" -- present only if this
	 *                                             is an outgoing 
	 *                                             connection
	 *               "jax::Component::host" -- router host to connect to
	 *                                         ("" if accepting)
	 *               "jax::Component::port" -- port to connect to (accept
	 *                                         from)
	 */
	virtual void init(judo::Element *config) = 0;

        /**
         * Callback point called when a packet arrives from the router
         * before configuration information arrives. WARNING: This
         * method will drop the packet by default, override it if you
         * want other behaviour.
         * @param packet Raw XML packet received from router
         */
        virtual void bufferPacket(judo::Element* packet) 
            { delete packet; }

	virtual ~Component() {}

	static void stop(int ec, const std::string &message)
	    {bedrock::Application::stop(ec, message);}
	static void exit(int ec, const std::string &message)
	    {bedrock::Application::exit(ec, message);}

    };

    class ComponentLoader :
	public RouterConnection<Packet, Packet>::EventListener
    {
    public:
	ComponentLoader(Component* ci);
	virtual ~ComponentLoader();
	int start(const char *hostname, unsigned int port, bool outgoing, 
		  const char *component_name, const char *secret, 
                  const char* config_id);

	// Application stuff
	void registerShutdownSignal(int signal)
	    {bedrock::Application::registerShutdownSignal(signal);}
	
	// EventListener
	void onRouterConnected();
	void onRouterDisconnected();
	void onRouterError();
	void onRouterPacket(Packet* p);
    private:
	Component*                        _ci;
	std::string                       _name;
        std::string                       _config_id;
	std::string                       _secret;
	bool                              _outgoing;
	std::string                       _host;
	unsigned int                      _port;
	RouterConnection<Packet, Packet>* _router;
        bool                              _configured;
    };
    
};


/**
 * Destructor.
 * Just calls disconnect.
*/
template<typename P, typename PFactory> 
jax::RouterConnection<P, PFactory>::~RouterConnection()
{
    bedrock::MutexProxy mp(_state_lock);
    assert(_state != Disconnected || _socket == NULL);

    if (_socket != NULL)
    {
        _socket->close();
        _state_cv.wait();
    }
    delete _stream;
}

/**
 * Establish a connection to the router. The type of
 * connection which is established is determined within by the
 * second constructor parameter.
 * @param a Address of the router; requires IP field to be
 * filled in if the second parameter of
 * RouterConnection::RouterConnection was false.
 * @throws exception::SocketError
 * @throws bedrock::net::SocketWatcher::exception::WatcherFull
*/
template<typename P, typename PFactory> 
void jax::RouterConnection<P, PFactory>::connect(const std::string &name,
						 const std::string &secret,
						 bedrock::net::Address& a,
                                                 bedrock::ThreadPool* tpool,
                                                 int threadkey)
{
    bedrock::MutexProxy mp(_state_lock);
    assert(_state == Disconnected);

    _socket = NULL;
    _state  = Error;
    _name = name;
    _secret = secret;

    try {
        if (_outgoing)
        {
            _socket = _watcher.createConnectSocket(this, 
						   a.getHostname().c_str(),
						   a.getService().c_str(), 
						   a.getPort(),
                                                   tpool,
						   _threadkey);
        }
        else
        {
            _socket = _watcher.createListenSocket(this,
						  a.getIP().c_str(),
						  a.getPort(),
						  true,
                                                  tpool,
						  _threadkey);
        }
    } catch (bedrock::net::Socket::exception::BindError& e) {
        throw exception::SocketError(e.en);
    } catch (bedrock::net::Socket::exception::ListenError& e) {
        throw exception::SocketError(e.en);
    } catch (bedrock::net::Socket::exception::ErrorCreatingSocket& e) {
        throw exception::SocketError(e.en);
    }

    _state = Connecting;
}

/**
 * Close the connection to the router. This may only be called
 * after the controller has been notified that the connection
 * is shutdown (EventListener::onDisconnected).
 */
template<typename P, typename PFactory> 
void jax::RouterConnection<P, PFactory>::disconnect()
{
    bedrock::MutexProxy mp(_state_lock);
    assert(_state != Disconnected);

    if (_socket != NULL)
        _socket->close();
    else
        _state = Disconnected;
}

/**
 * Send a packet to the router. 
 */
template<typename P, typename PFactory> 
void jax::RouterConnection<P, PFactory>::deliver(P* p)
{
    bedrock::MutexProxy mp(_state_lock);

    if (_state == Disconnected || _state == Error)
    {
        std::cerr << "Couldn't deliver packet: " << p->toString() << std::endl;
    }
    else
    {
        _socket->write(p->toString());
    }

    delete p;
}

// judo::ElementStream impl
template<typename P, typename PFactory> 
void jax::RouterConnection<P, PFactory>::onDocumentStart(judo::Element* elem)
{
    if (_outgoing)
    {
        bedrock::SHAHasher hasher;
        judo::Element handshake("handshake");

        hasher.hash(elem->getAttrib("id"));
        hasher.hash(_secret);

        const std::string& val = hasher.toString();
        handshake.addCDATA(val.c_str(), val.size());

        _socket->write(handshake.toString());
    }
    else
    {
        judo::Element root("stream:stream");
        root.putAttrib("xmlns:stream", "http://etherx.jabber.org/streams");
        root.putAttrib("xmlns", "jabber:component:accept");
        root.putAttrib("from", _name);
	
	char id[9];
	sprintf(id, "%x", rand());
	_handshake_id = id;
        root.putAttrib("id", _handshake_id);

        _socket->write(root.toStringEx().c_str());
    }

    delete elem;
}

template<typename P, typename PFactory> 
void jax::RouterConnection<P, PFactory>::onElement(judo::Element* elem)
{
    // If we are not yet connected, we should receive a handshake packet.
    // If we are the accepting socket, we need to validate the packet,
    // and if we are the connecting socket we receive an empty handshake
    // if the other end considers us valid.
    
    _state_lock.lock();

    if (_state == Connected)
    {
        _state_lock.unlock();
        _listener.onRouterPacket(PFactory::create(elem));
    }
    else 
    {
        assert(_state == Connecting);

        if (elem->getName() == "handshake")
        {
            if (_outgoing)
            {
                _state = Connected;
                _state_lock.unlock();
                _listener.onRouterConnected();
            }
            else
            {
                // Check this handshake:
                bedrock::SHAHasher hasher;
		
                hasher.hash(_handshake_id);
                hasher.hash(_secret);

                const std::string& val = hasher.toString();

                // If the hashed value matches the data in the
                // arriving packet, we send a handshake confirmation
                // back to the sender
                if (val == elem->getCDATA())
                {
                    judo::Element handshake("handshake");
                    _socket->write(handshake.toString());
                    _state = Connected;
                    _state_lock.unlock();
                    _listener.onRouterConnected();
                }
                else
                {
                    judo::Element error("stream:error");
                    error.addCDATA("Invalid handshake", strlen("Invalid handshake"));
                    _socket->write(error.toString());
                    _socket->close();
                    _state_lock.unlock();
                    _listener.onRouterError();
                }
                
            }
        }
        else
        {
            judo::Element error("stream:error");
            error.addCDATA("Must send handshake first.", 26);
            _socket->write(error.toString());
            _socket->close();
            _state_lock.unlock();
            _listener.onRouterError();
        }

        delete elem;
    }
}

template<typename P, typename PFactory> 
void jax::RouterConnection<P, PFactory>::onDocumentEnd()
{
    // Ok, force the socket to close
    bedrock::MutexProxy mp(_state_lock);
    _socket->close();
}

// bedrock::net::SocketEventListener impl
template<typename P, typename PFactory> 
bool jax::RouterConnection<P, PFactory>::onAccept(bedrock::net::Socket* newsocket)
{
    bedrock::MutexProxy mp(_state_lock);
    // Currently, _socket is the listening socket; once 
    // an incoming connection has been made, that will 
    // become our _socket to the router

    // Make sure the listener socket will not inform us of
    // any more events; then close the listening socket.
    _socket->setListener(NULL);
    _socket->close();

    // Now save the connection to the router and make sure
    // it notifies us of events; also be sure to register
    // for reads on this new socket as the incoming connection
    // is responsible for initiating the XML stream
    _socket = newsocket;
    _socket->setListener(this);
    _socket->resumeRead();

    return false;
}

template<typename P, typename PFactory> 
void jax::RouterConnection<P, PFactory>::onConnected(bedrock::net::Socket *sock)
{
    bedrock::MutexProxy mp(_state_lock);
    _socket->resumeRead();

    if (_outgoing)
    {
        // We want to send the stream header now. 
	judo::Element elem("stream:stream");
	elem.putAttrib("xmlns:stream", "http://etherx.jabber.org/streams");
	elem.putAttrib("xmlns", "jabber:component:accept");
	elem.putAttrib("to", _name);
	
	_socket->write(elem.toStringEx().c_str());
    }
}

template<typename P, typename PFactory> 
void jax::RouterConnection<P, PFactory>::onClose(bedrock::net::Socket *sock)
{
    _state_lock.lock();
    _socket = NULL;
    _state = Disconnected;
    // Reset the parser
    _stream->reset();
    _state_cv.signal();
    _state_lock.unlock();
    // Notify the listener
    _listener.onRouterDisconnected();
}

template<typename P, typename PFactory> 
void jax::RouterConnection<P, PFactory>::onInitialize(bedrock::net::Socket *sock)
{}

template<typename P, typename PFactory> 
void jax::RouterConnection<P, PFactory>::onError(bedrock::net::Socket *sock, 
						 bedrock::net::Socket::exception::ErrorCode ec, 
						 int err)
{
    //TODO: Finish error handling
    {
        bedrock::debug::Tracer t("RouterConnection::onError");
        t << "Socket error(" << (int)ec << "): " << strerror(err); //<< std::endl;
    }

    _state_lock.lock();
    _state = Error;
    _state_lock.unlock();

    _listener.onRouterError();
}

template<typename P, typename PFactory> 
bool jax::RouterConnection<P, PFactory>::onRead(bedrock::net::Socket *sock, const char* buf, int buf_sz)
{
    try 
    {
	_stream->push(buf, buf_sz);
    }
    catch (judo::ElementStream::exception::ParserError &e)
    {
        UNUSED(e);
	_state_lock.lock();
	_state = Error;
	_state_lock.unlock();
	_listener.onRouterError();
    }

    return true;
}

#endif // _H_JAX
