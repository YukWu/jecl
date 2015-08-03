//============================================================================
// Project:       Bedrock - Foundational objects
// Filename:      ComponentLoader.cpp
// Description:   bedrock::Application implementation
// Created at:    Thu Jul 19 13:15:16 2001
// Modified at:   Thu Feb 21 12:07:25 2002
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
// $Id: ComponentLoader.cpp,v 1.1 2002/04/30 11:39:34 akuma Exp $
//============================================================================
#include "stdafx.h"
#include "jax.hpp"
#include <string>
using namespace jax;
using namespace std;
using namespace bedrock;

ComponentLoader::ComponentLoader(Component* ci) : 
    _ci(ci), _configured(false)
{}

ComponentLoader::~ComponentLoader() 
{}

int ComponentLoader::start(const char *hostname, unsigned int port, bool outgoing,
                           const char *component_name, const char *secret, 
                           const char* config_id)
{
    bedrock::ThreadPool tpool(1);
    bedrock::net::SocketWatcher watcher(2);
    _router = new RouterConnection<Packet, Packet>(watcher, *this, 
						   outgoing, 0);

    _name = component_name;
    _config_id = config_id;
    _secret = secret;
    _outgoing = outgoing;
    _host = hostname;
    _port = port;

    if (outgoing)
    {
	bedrock::net::Address a(hostname, "", port);
	_router->connect(component_name, secret, a, &tpool);
    }
    else
    {
	bedrock::net::Address a;
	a.addEntry(0, hostname, port);
	_router->connect(component_name, secret, a, &tpool);
    }
    return bedrock::Application::start();
}

// EventListener
void ComponentLoader::onRouterConnected()
{
    judo::Element *request = new judo::Element("xdb");
    request->putAttrib("type", "get");
    request->putAttrib("to", "config@-internal");
    request->putAttrib("from", _name);
    request->putAttrib("ns", _config_id);
    _router->deliver(new Packet(request));
}

void ComponentLoader::onRouterDisconnected()
{
    // Give the router a moment to reset its listening port
#ifdef WIN32
    Sleep(1000);
#else
    sleep(1);
#endif

    if (!_configured)
    {
	bedrock::Application::exit(1, "Disconnected from router before receiving configuration");
    }

}

void ComponentLoader::onRouterError()
{
    bedrock::Application::stop(1, "Component loader encountered router error. Exiting.");
}

void ComponentLoader::onRouterPacket(Packet* p)
{
    judo::Element *ep = p->getElement();
    if (ep->getName() == "xdb" && ep->getAttrib("type") == "result")
    {
        // Make sure we got some configuration back
        if (ep->begin() == ep->end())
        {
            bedrock::Application::exit(1, "No configuration received.  Exiting.");
        }
	judo::Element *config = dynamic_cast<judo::Element*>(*ep->begin());
	// Add connection information to the config
	config->addElement("jax:component:name", _name);
	config->addElement("jax:component:secret", _secret);
	if (_outgoing)
	{
	    config->addElement("jax:component:outgoing", "true");
	}
	config->addElement("jax:component:host", _host);
	char buf[12];
	memset(buf, '\0', sizeof(buf));
	sprintf(buf, "%d", _port);
	config->addElement("jax:component:port", buf);

        // Set configured flag
        _configured = true;

        // Disconnect router
        _router->disconnect();

        // Queue up a call to pass configuration off to component
        ThreadPool::getCurrentThreadPool()->enqueue(callback::wrap(_ci, &Component::init, config),
                                                    ThreadPool::getCurrentThreadKey());
    }
    else
    {
        _ci->bufferPacket(ep);
    }
}

