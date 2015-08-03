//============================================================================
// Project:       Jabber Universal Document Objects (Judo)
// Filename:      ElementStreamTest.cpp
// Description:   judo::ElementStream unit tests
// Created at:    Tue Jul  3 15:59:14 2001
// Modified at:   Mon Jul 30 18:07:08 2001
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
// Copyright (c) 1999-2001 Jabber.com, Inc.  All Rights Reserved.  
// 
// $Id: ElementStreamTest.cpp,v 1.1 2002/04/30 11:39:43 akuma Exp $
//============================================================================

#include "stdafx.h"
#include "judo.hpp"
#include "judo_test.hpp"
using namespace judo;

#include <iostream>
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(ElementStreamTest);

list<Element*> G_results;

class ElementStreamTestImpl
    : public ElementStreamEventListener
{
public:
    ElementStreamTestImpl()
	{
        _stream = new ElementStream(this);
    }

    ~ElementStreamTestImpl()
    {
        delete _stream;
    }
    void onDocumentStart(Element* t)
	{
	    G_results.push_back(t);
	}
    void onElement(Element* t)
	{
	    G_results.push_back(t);
	}
    void onDocumentEnd()
	{
	    for_each(G_results.begin(), G_results.end(), _releaseElement);
	    G_results.clear();
	}
    ElementStream *_stream;
private:
    static void _releaseElement(Element* t)
	{
	    delete t;
	}

};

void ElementStreamTest::construct()
{
    ElementStreamTestImpl es;

    CPPUNIT_ASSERT(es._stream->_document_started == false);
    CPPUNIT_ASSERT(es._stream->_document_ended == false);
}

void ElementStreamTest::push()
{
    CPPUNIT_ASSERT(G_results.empty());

    ElementStreamTestImpl es;

    try 
    {    
	// Push the root element in
	es._stream->push("<root>", 6);
	CPPUNIT_ASSERT(es._stream->_document_started == true);
	CPPUNIT_ASSERT(G_results.size() == 1);
	CPPUNIT_ASSERT(G_results.back()->toString() == "<root/>");

	// Do a partial push of a top-level packet
	es._stream->push("<message to='dizzy@j.org'><body>Hello", 37);
	CPPUNIT_ASSERT(es._stream->_element_stack.size() == 2);
	CPPUNIT_ASSERT(G_results.size() == 1);

	// Complete the push of the top-level packet
	es._stream->push("</body></message>", 17);
	CPPUNIT_ASSERT(es._stream->_element_stack.size() == 0);
	CPPUNIT_ASSERT(G_results.size() == 2);
	CPPUNIT_ASSERT(G_results.back()->toString() == "<message to='dizzy@j.org'><body>Hello</body></message>");

	// Push another packet on
	es._stream->push("<presence/>", 11);
	CPPUNIT_ASSERT(es._stream->_element_stack.size() == 0);
	CPPUNIT_ASSERT(G_results.size() == 3);
	CPPUNIT_ASSERT(G_results.back()->toString() == "<presence/>");

	// Close the document
	es._stream->push("</root>", 7);
	CPPUNIT_ASSERT(es._stream->_document_ended == true);
	CPPUNIT_ASSERT(G_results.empty());
    } catch (ElementStream::exception::ParserError& e)
    {
	cerr << "Parser error: " << e.getMessage() << endl;
	throw;
    }
}

void ElementStreamTest::parseAtOnce()
{
    // Standard test
    const char* data1 = "<root>foobar<tag1/>somedata</root>";
    Element* e = ElementStream::parseAtOnce(data1);
    CPPUNIT_ASSERT(e->toString() == "<root>foobar<tag1/>somedata</root>");
    delete e;

    // Incomplete test
    bool successful2 = false;
    const char* data2 = "<root><tag1>foadsf</tag1><tag2>";
    try
    {
	e = ElementStream::parseAtOnce(data2);
    } catch (ElementStream::exception::IncompleteParse&) {
	successful2 = true;
    }
    CPPUNIT_ASSERT(successful2 == true);

    // Invalid XML test
    bool successful3 = false;
    const char* data3 = "<root></tag1></root>";
    try
    {
	e = ElementStream::parseAtOnce(data3);
    } catch (ElementStream::exception::ParserError&) {
	successful3 = true;
    }
    CPPUNIT_ASSERT(successful3 == true);
    
}
