//============================================================================
// Project:       JAX -- Jabber Access eXtensions
// Filename:      func_routerconn.cpp
// Description:   test
// Created at:    Thu Jul 19 10:00:55 2001
// Modified at:   Thu Jul 19 16:51:09 2001
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
// Copyright (c) 2001 Jabber.com, Inc.  All Rights Reserved.  
// 
// $Id: func_routerconn.cpp,v 1.1 2002/04/30 11:39:36 akuma Exp $
//============================================================================
#include "jax.h"
using namespace jax;
using namespace std;

int                   G_finished = 0;
bedrock::Mutex        G_finished_lock;
bedrock::CondVariable G_finished_cv(G_finished_lock);

class Acceptor
    : public jax::RouterConnection<Packet, Packet>::EventListener
{
private:
    bedrock::ThreadPool         _tpool;
    bedrock::net::SocketWatcher _watcher;
    bedrock::net::Address       _address;
    RouterConnection<Packet, Packet>  _router;
    string                      _name;
    string                      _secret;
public:
    Acceptor(unsigned int port)
        : _tpool(3), _watcher(_tpool, 3), 
          _router(_watcher, *this, false, _tpool.getNextThreadID()), _name("component1"), _secret("thepassword")
        {
            _address.addEntry(0, "127.0.0.1", port);
        }
    void onRouterConnected()
        {
            bedrock::MutexProxy mp(G_finished_lock);
            G_finished++;
            G_finished_cv.signal();
        }
    void onRouterDisconnected()
        {
            bedrock::MutexProxy mp(G_finished_lock);
            G_finished++;
            G_finished_cv.signal();
        }
    void onRouterError()
        {
            assert(0);
        }
    void onRouterPacket(Packet* p)
        {
            bedrock::MutexProxy mp(G_finished_lock);
            G_finished++;
            G_finished_cv.signal();
            delete p;
        }
    void send()
        {
            judo::Element* elem = new judo::Element("message");
            elem->addElement("body", "Hello my friends & the world.");
            elem->putAttrib("to", "All cool connectors");
            elem->putAttrib("from", "All cool acceptors");
            _router.deliver(new Packet(elem));
        }
    void close()
        {
            _router.disconnect();
        }
    void connect()
        {
            _router.connect(_name, _secret, _address);
        }
};

class Connector
    : public RouterConnection<Packet, Packet>::EventListener
{
private:
    bedrock::ThreadPool         _tpool;
    bedrock::net::SocketWatcher _watcher;
    bedrock::net::Address       _address;
    RouterConnection<Packet, Packet> _router;
    string                      _name;
    string                      _secret;
public:
    Connector(const char* hostname, unsigned int port)
        : _tpool(3), _watcher(_tpool, 3), _address(hostname, "", port),
          _router(_watcher, *this, true, _tpool.getNextThreadID()), _name("component1"), _secret("thepassword")
        {}
    void onRouterConnected()
        {
            bedrock::MutexProxy mp(G_finished_lock);
            G_finished++;
            G_finished_cv.signal();
        }
    void onRouterDisconnected()
        {
            bedrock::MutexProxy mp(G_finished_lock);
            G_finished++;
            G_finished_cv.signal();
        }
    void onRouterError()
        {
            assert(0);
        }
    void onRouterPacket(Packet* p)
        {
            bedrock::MutexProxy mp(G_finished_lock);
            G_finished++;
            G_finished_cv.signal();
            delete p;
        }
    void send()
        {
            judo::Element* elem = new judo::Element("message");
            elem->addElement("body", "Hello my friends & the world.");
            elem->putAttrib("to", "All cool acceptors");
            elem->putAttrib("from", "All cool connectors");
            _router.deliver(new Packet(elem));
        }
    void close()
        {
            _router.disconnect();
        }
    void connect()
        {
            _router.connect(_name, _secret, _address);
        }
};

int main(int argc, char** argv)
{
    Acceptor a(3000);
    Connector c("127.0.0.1", 3000);

    // Wait for two signals to signify that both
    // acceptor and connector are up
    try
    {
        a.connect();
        c.connect();
        {
            bedrock::MutexProxy mp(G_finished_lock);
            while (G_finished != 2)
            {
                G_finished_cv.wait();
            }
            G_finished = 0;
        }
    }
    catch (bedrock::net::Socket::exception::BindError& e)
    {
        cerr << "Unable to bind (" << e.en << "): " << e.es << endl;
        assert(0);
    }

    cerr << "Connected" << endl;

    // Transmit packets then wait for two wakeups
    a.send();
    c.send();
    {
        bedrock::MutexProxy mp(G_finished_lock);
        while (G_finished != 2)
        {
            G_finished_cv.wait();
        }
        G_finished = 0;
    }

    cerr << "Sent" << endl;

    // Close the connection and wait for two wakeups
    a.close();
    c.close();
    {
        bedrock::MutexProxy mp(G_finished_lock);
        while (G_finished != 2)
        {
            G_finished_cv.wait();
        }
        G_finished = 0;
    }

    cerr << "Closed" << endl;
}
