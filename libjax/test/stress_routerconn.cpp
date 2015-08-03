//============================================================================
// Project:       JAX -- Jabber Access eXtensions
// Filename:      stress_routerconn.cpp
// Description:   tester
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
// $Id: stress_routerconn.cpp,v 1.1 2002/04/30 11:39:36 akuma Exp $
//============================================================================

#include "jax.h"
using namespace jax;

using namespace bedrock;
using namespace bedrock::net;
using namespace std;

int                   G_count = 0;
bedrock::Mutex        G_count_lock;

class Connector
    : public RouterConnection<Packet, Packet>::EventListener
{
private:
    bedrock::ThreadPool         _tpool;
    bedrock::net::SocketWatcher _watcher;
    bedrock::net::Address       _address;
    jax::RouterConnection<Packet, Packet> _router;
    string                      _name;
    string                      _secret;
    bool                        _outgoing;
public:
    Connector(bedrock::net::Address& a, bool outgoing)
        : _tpool(3), _watcher(_tpool, 3), _address(a),
          _router(_watcher, *this, outgoing, _tpool.getNextThreadID()), 
	  _name("component1"), _secret("thepassword"), _outgoing(outgoing)
        {
            _router.connect(_name, _secret, _address);
        }
    void onRouterConnected()
        {
            bedrock::MutexProxy mp(G_count_lock);
            G_count++;
            if (_outgoing)
                _router.disconnect();
        }
    void onRouterDisconnected()
        {
            bool ok = false;

            if (_outgoing)
                usleep(50);
	    
            while (!ok)
            {
                try 
                {
		    _router.connect(_name, _secret, _address);
		    ok = true;
                } 
                catch (...) 
                {
                    cerr << "caught" << endl;
                    _router.disconnect();
                    sleep(2);
                }
            }
        }
    void onRouterError()
        {
            cerr << "Router error." << endl;
            _router.disconnect();
        }
    void onRouterPacket(Packet* p)
        {
            delete p;
        }
};

int main(int argc, char** argv)
{
    Connector* c;

    if (argc != 4)
    {
        cerr << "3 arguments required: <outgoing> (true or false), <ip/hostname>, <port>" << endl;
        exit(-1);
    }

    // Wait for two signals to signify that both
    // acceptor and connector are up
    try
    {
        // Connecting
        if (strcmp(argv[1], "true") == 0)
        {
	    Address a(argv[2], "", atoi(argv[3]));;
            c = new Connector(a, true);
        }
        // Accepting
        else
        {
	    Address a;
	    a.addEntry(0, argv[2], atoi(argv[3]));
            c = new Connector(a, false);
        }

        while (1)
        {
            sleep(2);
            {
                MutexProxy mp(G_count_lock);
                cerr << G_count << endl;
                if (G_count == 1000) 
                    break;
            }
        }
    }
    catch (bedrock::net::Socket::exception::BindError& e)
    {
        cerr << "Unable to bind (" << e.en << "): " << e.es << endl;
        assert(0);
    }

}
