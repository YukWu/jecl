//============================================================================
// Project:       Bedrock
// Filename:      bedrock_signals.hpp
// Description:   Signals implementation
// Created at:    Mon Oct 29 17:08:18 2001
// Modified at:   Tue Nov  6 16:02:47 2001
// 
// License:
// 
// The contents of this file are subject to the Jabber Contributed
// License Version 1.0 (the License). You may not copy or use this
// this file, in either source code or executable form, except in
// with the License. You may obtain a copy of the License at
// at http://www.jabber.com/jccl/.
// 
// Software distributed under the License is distributed on an AS IS
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
// See the License for the specific language governing rights and
// limitations under the License.
// 
// Copyrights:
// 
// Copyright (c) 1999-2001 Jabber, Inc.
// 
// 
// $Id: bedrock_signals.hpp,v 1.1 2002/04/30 11:39:23 akuma Exp $
//============================================================================
#ifndef _H_BEDROCK_SIGNALS
#define _H_BEDROCK_SIGNALS

namespace bedrock
{
    namespace signals
    {

	class Signal_0
	{
	public:
	    virtual ~Signal_0() {}
	    virtual void operator()(void) = 0;
	};

	template<class Param1>
	class Signal_1
	{
	public:
	    virtual ~Signal_1() {}
	    virtual void operator()(Param1 p1) = 0;
	};

	template<class Param1, class Param2>
	class Signal_2
	{
	public:
	    virtual ~Signal_2() {}
	    virtual void operator()(Param1 p1, Param2 p2) = 0;
	};

	template<class Param1, class Param2, class Param3>
	class Signal_3
	{
	public:
	    virtual ~Signal_3() {}
	    virtual void operator()(Param1 p1, Param2 p2, Param3 p3) = 0;
	};

	template<class Param1, class Param2, class Param3, class Param4>
	class Signal_4
	{
	public:
	    virtual ~Signal_4() {}
	    virtual void operator()(Param1 p1, Param2 p2, Param3 p3, Param4 p4) = 0;
	};

	template<class ObjPtr, class ObjFunPtr>
	class MemberSignal_0
	    : public Signal_0
	{
	public:
	    MemberSignal_0(const ObjPtr& obj, const ObjFunPtr& objfn)
		: _obj(obj), _objfn(objfn)
	    {}

	    void operator()(void)
	    { ((*_obj).*_objfn)(); }
	private:
	    ObjPtr _obj;
	    ObjFunPtr _objfn;
	};

	template<class ObjPtr, class ObjFunPtr, class Param1>
	class MemberSignal_1
	    : public Signal_1<Param1>
	{
	public:
	    MemberSignal_1(const ObjPtr& obj, const ObjFunPtr& objfn)
		: _obj(obj), _objfn(objfn)
	    {}
	    void operator()(Param1 p1)
	    { ((*_obj).*_objfn)(p1); }
	private:
	    ObjPtr _obj;
	    ObjFunPtr _objfn;
	};

	template<class ObjPtr, class ObjFunPtr, 
		 class Param1, class Param2>
	class MemberSignal_2
	    : public Signal_2<Param1, Param2>
	{
	public:
	    MemberSignal_2(const ObjPtr& obj, const ObjFunPtr& objfn)
		: _obj(obj), _objfn(objfn)
	    {}
	    void operator()(Param1 p1, Param2 p2)
	    { ((*_obj).*_objfn)(p1, p2); }
	private:
	    ObjPtr _obj;
	    ObjFunPtr _objfn;
	};

	template<class ObjPtr, class ObjFunPtr, 
		 class Param1, class Param2, class Param3>
	class MemberSignal_3
	    : public Signal_3<Param1, Param2, Param3>
	{
	public:
	    MemberSignal_3(const ObjPtr& obj, const ObjFunPtr& objfn)
		: _obj(obj), _objfn(objfn)
	    {}
	    void operator()(Param1 p1, Param2 p2, Param3 p3)
	    { ((*_obj).*_objfn)(p1, p2, p3); }
	private:
	    ObjPtr _obj;
	    ObjFunPtr _objfn;
	};

	template<class ObjPtr, class ObjFunPtr, 
		 class Param1, class Param2, class Param3, class Param4>
	class MemberSignal_4
	    : public Signal_4<Param1, Param2, Param3, Param4>
	{
	public:
	    MemberSignal_4(const ObjPtr& obj, const ObjFunPtr& objfn)
		: _obj(obj), _objfn(objfn)
	    {}
	    void operator()(Param1 p1, Param2 p2, Param3 p3, Param4 p4)
	    { ((*_obj).*_objfn)(p1, p2, p3,p4); }
	private:
	    ObjPtr _obj;
	    ObjFunPtr _objfn;
	};

	template <class FnPtr>
	class StaticSignal_0
	    : public Signal_0
	{
	public:
	    StaticSignal_0(const FnPtr& fn)
		: _fn(fn)
	    {}
	    void operator()()
	    { _fn(); }
	private:
	    FnPtr _fn;
	};

	template<class FnPtr, class Param1>
	class StaticSignal_1
	    : public Signal_1<Param1>
	{
	public:
	    StaticSignal_1(const FnPtr& fn)
		: _fn(fn)
	    {}
	    void operator()(Param1 p1)
	    { _fn(p1); }
	private:
	    FnPtr _fn;
	};

	template<class FnPtr, class Param1, class Param2>
	class StaticSignal_2
	    : public Signal_2<Param1, Param2>
	{
	public:
	    StaticSignal_2(const FnPtr& fn)
		: _fn(fn)
	    {}
	    void operator()(Param1 p1, Param2 p2)
	    { _fn(p1, p2); }
	private:
	    FnPtr _fn;
	};


	template<class FnPtr, 
		 class Param1, class Param2, class Param3>
	class StaticSignal_3
	    : public Signal_3<Param1, Param2, Param3>
	{
	public:
	    StaticSignal_3(const FnPtr& fn)
		: _fn(fn)
	    {}
	    void operator()(Param1 p1, Param2 p2, Param3 p3)
	    { _fn(p1, p2, p3); }
	private:
	    FnPtr _fn;
	};

	template<class FnPtr, 
		 class Param1, class Param2, class Param3, class Param4>
	class StaticSignal_4
	    : public Signal_4<Param1, Param2, Param3, Param4>
	{
	public:
	    StaticSignal_4(const FnPtr& fn)
		: _fn(fn)
	    {}
	    void operator()(Param1 p1, Param2 p2, Param3 p3, Param4 p4)
	    { _fn(p1, p2, p3, p4); }
	private:
	    FnPtr _fn;
	};

	template<class ObjPtr, class Obj, class ObjFunRet>
	inline Signal_0* wrap(const ObjPtr& obj, ObjFunRet (Obj::*objfn)())
	{
	    return new MemberSignal_0<ObjPtr, ObjFunRet(Obj::*)()>(obj, objfn);
	}

	template<class ObjPtr, class Obj, class ObjFunRet, class Type1>
	inline Signal_1<Type1>* wrap(const ObjPtr& obj, ObjFunRet (Obj::*objfn)(Type1))
	{
	    return new MemberSignal_1<ObjPtr, ObjFunRet(Obj::*)(Type1), Type1>(obj, objfn);
	}

	template<class ObjPtr, class Obj, class ObjFunRet, class Type1, class Type2>
	inline Signal_2<Type1, Type2>* wrap(const ObjPtr& obj, 
					    ObjFunRet (Obj::*objfn)(Type1, Type2))
	{
	    return new MemberSignal_2<ObjPtr, ObjFunRet(Obj::*)(Type1, Type2), 
		Type1, Type2>(obj, objfn);
	}

	template<class ObjPtr, class Obj, class ObjFunRet, class Type1, class Type2, 
		 class Type3>
	inline Signal_3<Type1, Type2, Type3>* wrap(const ObjPtr& obj, 
						   ObjFunRet (Obj::*objfn)(Type1, Type2, Type3))
	{
	    return new MemberSignal_3<ObjPtr, ObjFunRet(Obj::*)(Type1,Type2,Type3), 
		Type1, Type2, Type3>(obj, objfn);
	}

	template<class ObjPtr, class Obj, class ObjFunRet, class Type1, class Type2, 
		 class Type3, class Type4>
	inline Signal_4<Type1, Type2, Type3, Type4>* wrap(const ObjPtr& obj, 
						   ObjFunRet (Obj::*objfn)(Type1, Type2, Type3,Type4))
	{
	    return new MemberSignal_4<ObjPtr, ObjFunRet(Obj::*)(Type1,Type2,Type3,Type4), 
		Type1, Type2, Type3,Type4>(obj, objfn);
	}

	template<class FunRet>
	inline Signal_0* wrap(FunRet(*fn)())
	{
	    return new StaticSignal_0<FunRet(*)()>(fn);
	}

	template<class FunRet, class Type1>
	inline Signal_1<Type1>* wrap(FunRet(*fn)(Type1))
	{
	    return new StaticSignal_1<FunRet(*)(Type1), Type1>(fn);
	}

	template<class FunRet, class Type1, class Type2>
	inline Signal_2<Type1,Type2>* wrap(FunRet(*fn)(Type1, Type2))
	{
	    return new StaticSignal_2<FunRet(*)(Type1,Type2), Type1, Type2>(fn);
	}

	template<class FunRet, class Type1, class Type2, class Type3>
	inline Signal_3<Type1,Type2,Type3>* wrap(FunRet(*fn)(Type1,Type2,Type3))
	{
	    return new StaticSignal_3<FunRet(*)(Type1,Type2,Type3), 
		Type1,Type2,Type3>(fn);
	}

	template<class FunRet, class Type1, class Type2, class Type3, class Type4>
	inline Signal_4<Type1,Type2,Type3,Type4>* wrap(FunRet(*fn)(Type1,Type2,Type3,Type4))
	{
	    return new StaticSignal_4<FunRet(*)(Type1,Type2,Type3,Type4), 
		Type1,Type2,Type3,Type4>(fn);
	}

    }

}

#endif
