//============================================================================
// Project:       JAX -- Jabber Access eXtensions
// Filename:      Componenttest.cpp
// Description:   Tester
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
// $Id: ComponentTest.cpp,v 1.1 2002/04/30 11:39:36 akuma Exp $
//============================================================================
/*
Use the following as your jabber.xml file, then run ./ComponentTest on the
same machine.
<!--Begin jabber.xml-->
<jabber>
  <service id='my_id'>
    <my_id xmlns='jabber:config:my_id'>
      <mytag>Hello</mytag>
    </my_id>
    <accept>
      <ip>127.0.0.1</ip>
      <port>7777</port>
      <secret>my_secret</secret>
    </accept>
  </service>
</jabber>
<!--End jabber.xml-->
*/
#include "stdafx.h"
#include "jax.hpp"
using namespace jax;
using namespace std;
class MyComponentInterface: public Component
{
public:
    MyComponentInterface(){}
    void init(judo::Element *element)
    {
	if (element == 0)
	{
	    Component::exit(1, "Failed: no config found.");
	}
	judo::Element *ep = element->findElement("mytag");
	if (ep == NULL)
	{
	    Component::exit(2, "Failed: no child element.");
	}

	if (ep->getCDATA() != "Hello")
	{
	    cerr << "Failed: incorrect CDATA" << endl;
	}
	else
	{
	    cerr << "Passed" << endl;
	}
	Component::stop(0,"Normal exit.");
    }
};

int main()
{
    MyComponentInterface *mci = new MyComponentInterface();
    ComponentLoader loader(mci);
    return loader.start("localhost", 7777, true, "my_id", "my_secret", "jabber:config:my_id");
}
