//============================================================================
// Project:       Bedrock -- Foundational Objects
// Filename:      bedrock_debug.h
// Description:   bedrock::debug namespace
// Created at:    Thu Jul 19 15:07:42 2001
// Modified at:   Thu Jul 19 15:43:35 2001
// 
// License:
// 
// The contents of this file are subject to the Jabber Contributed
// Code License, version 1.0 (the License). You may not copy or use
// this file, in either source code or executable form, except in
// compliance with the License. You may obtain a copy of the License
// at http://www.jabber.com/jccl/.
// 
// Software distributed under the License is distributed on an AS IS
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.
// See the License for the specific language governing rights and
// limitations under the License.
// 
// Copyrights:
// 
// Copyright (c) 1999-2001 Jabber, Inc.
// 
// 
// $Id: bedrock_debug.h,v 1.1 2002/04/30 11:39:23 akuma Exp $
//============================================================================
#ifndef INCL_BEDROCK_DEBUG_H
#define INCL_BEDROCK_DEBUG_H

#include <iostream>
#include <string>

#ifdef TRACER
namespace bedrock
{
    namespace debug
    {
        class Tracer
        {
        public:
            Tracer(const char* name, std::ostream& os = std::cerr):
              _name(name), _os(os)
              { _os << "(" << pthread_self() << ") --> " << _name << std::endl; }
              ~Tracer()
              { _os << "(" << pthread_self() << ") <-- " << _name << std::endl; }

              std::ostream& operator<<(int value)
              { 
                  _os << "(" << pthread_self() << ") -- " << _name << ": "<< value; 
                  return _os; 
              }
              std::ostream& operator<<(const std::string& value)
              { 
                  _os << "(" << pthread_self() << ") -- " << _name << ": "<< value; 
                  return _os; 
              }

              std::ostream& operator<<(std::ostream& f(std::ostream&))
              {
                  return f(_os);
              }
        private:
            std::string         _name;
            std::ostream&       _os;
        };
    }

    /* 
       Function to simplify error checking.  The argument passed into the
       function should be return value of a function which returns 0 for
       success and errno for failure.  In particular, pthread_* calls
       behave this way. Errors for these functions is considered fatal, so
       the validating function will abort if the result code is non-zero.
    */
    inline void abortingCall(int funcresult)
	{
	    if (funcresult != 0)
	    {
		std::cerr << "Function call failed with errno = " << funcresult 
			  << "(" << strerror(funcresult) << "). Aborting." << std::endl;
		abort();
	    }
	}

}
#else
namespace bedrock
{
    namespace debug
    {
        class Tracer
        {
        public:
            Tracer(const char* name, std::ostream& os = std::cerr) {}

            Tracer& operator<<(int value)
            { return *this; }

            Tracer& operator<<(const std::string& value)
            { return *this; }

            Tracer& operator << (std::ostream& f(std::ostream&))
            { return *this; }
        };
    }

    inline void abortingCall(int funcresult)
	{}

}

#endif // TRACER

#endif // INCL_BEDROCK_DEBUG_H



