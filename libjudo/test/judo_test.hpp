//============================================================================
// Project:       Jabber Universal Document Objects (Judo)
// Filename:      judo_test.h
// Description:   Unit Tests Header
// Created at:    Tue Jul  3 13:06:11 2001
// Modified at:   Fri Jul  6 12:30:17 2001
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
// $Id: judo_test.hpp,v 1.1 2002/04/30 11:39:43 akuma Exp $
//============================================================================

#ifndef INCL_JUDOTEST_H
#define INCL_JUDOTEST_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

namespace judo
{
    class GlobalsTest
        : public CppUnit::TestCase
    {
        CPPUNIT_TEST_SUITE(GlobalsTest);
        CPPUNIT_TEST(escape);
        CPPUNIT_TEST(unescape);
        CPPUNIT_TEST_SUITE_END();
    public:
    
	// Tests
	void escape();
	void unescape();
    };

    class CDATATest
        : public CppUnit::TestCase
    {
        CPPUNIT_TEST_SUITE(CDATATest);
        CPPUNIT_TEST(construct);
        CPPUNIT_TEST(setText);
        CPPUNIT_TEST(appendText);
        CPPUNIT_TEST(getText);
        CPPUNIT_TEST(CDATAtoString);
        CPPUNIT_TEST_SUITE_END();
    public:
        // Tests
        void construct();

        void setText();
        void appendText();
        void getText();

        void CDATAtoString();
    };

    class ElementTest
	: public CppUnit::TestCase
    {
        CPPUNIT_TEST_SUITE(ElementTest);
        CPPUNIT_TEST(construct);
        CPPUNIT_TEST(addElement);
        CPPUNIT_TEST(findElement);
        CPPUNIT_TEST(addCDATA);
        CPPUNIT_TEST(detachChild);
        CPPUNIT_TEST(getAttrib);
        CPPUNIT_TEST(putAttrib);
        CPPUNIT_TEST(delAttrib);
        CPPUNIT_TEST(cmpAttrib);
        CPPUNIT_TEST(ElementtoString);
        CPPUNIT_TEST(ElementtoStringEx);
        CPPUNIT_TEST(getCDATA);
        CPPUNIT_TEST_SUITE_END();
    public:
	// Tests
	void construct();
	void addElement();
        void findElement();
	void addCDATA();
        void detachChild();
	
	void getAttrib();
	void putAttrib();
	void delAttrib();
	void cmpAttrib();

	void ElementtoString();
	void ElementtoStringEx();

	void getCDATA();
    };

    class ElementStreamTest
	: public CppUnit::TestCase
    {
        CPPUNIT_TEST_SUITE(ElementStreamTest);
        CPPUNIT_TEST(construct);
        CPPUNIT_TEST(push);
        CPPUNIT_TEST(parseAtOnce);
        CPPUNIT_TEST_SUITE_END();
    public:

	// Tests
	void construct();
	void push();
	void parseAtOnce();
    };
};
#endif
