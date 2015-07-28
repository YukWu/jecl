#ifndef _H_BEDROCK_CALLBACKS
#define _H_BEDROCK_CALLBACKS

namespace bedrock
{
    namespace callback
    {

    class Functor
    {
    public:
        virtual void operator()() = 0;
        virtual ~Functor() {}
    };

    // -------------------------------
    //
    // MemberFunctor[0-4]
    //
    // -------------------------------
    template<class ObjPtr, class ObjFunPtr>
    class MemberFunctor0 : public Functor
    {
    public:
        MemberFunctor0(const ObjPtr& obj, const ObjFunPtr& objfn)
        : _obj(obj), _objfn(objfn)
        {}

        virtual void operator()() 
        { ((*_obj).*_objfn)(); }
    private:
        ObjPtr    _obj;
        ObjFunPtr _objfn;
    };

    template<class ObjPtr, class ObjFunPtr, 
         class Param1>
    class MemberFunctor1 : public Functor
    {
    public:
        MemberFunctor1(const ObjPtr& obj, const ObjFunPtr& objfn,
               Param1 p1)
        : _obj(obj), _objfn(objfn), _p1(p1)
        {}

        virtual void operator()() 
        { ((*_obj).*_objfn)(_p1); }
    private:
        ObjPtr    _obj;
        ObjFunPtr _objfn;
        Param1    _p1;
    };

    template<class ObjPtr, class ObjFunPtr, 
         class Param1, class Param2>
    class MemberFunctor2 : public Functor
    {
    public:
        MemberFunctor2(const ObjPtr& obj, const ObjFunPtr& objfn,
               Param1 p1, Param2 p2)
        : _obj(obj), _objfn(objfn), _p1(p1), _p2(p2)
        {}

        virtual void operator()() 
        { ((*_obj).*_objfn)(_p1, _p2); }
    private:
        ObjPtr _obj;
        ObjFunPtr _objfn;
        Param1    _p1;
        Param2    _p2;
    };

    template<class ObjPtr, class ObjFunPtr,
         class Param1, class Param2, class Param3>
    class MemberFunctor3 : public Functor
    {
    public:
        MemberFunctor3(const ObjPtr& obj, const ObjFunPtr& objfn,
               Param1 p1, Param2 p2, Param3 p3)
        : _obj(obj), _objfn(objfn), _p1(p1), _p2(p2), _p3(p3)
        {}

        virtual void operator()() 
        { ((*_obj).*_objfn)(_p1, _p2, _p3); }
    private:
        ObjPtr _obj;
        ObjFunPtr _objfn;
        Param1    _p1;
        Param2    _p2;
        Param3    _p3;
    };

    template<class ObjPtr, class ObjFunPtr,
         class Param1, class Param2, class Param3, class Param4>
    class MemberFunctor4 : public Functor
    {
    public:
        MemberFunctor4(const ObjPtr& obj, const ObjFunPtr& objfn,
               Param1 p1, Param2 p2, Param3 p3, Param4 p4)
        : _obj(obj), _objfn(objfn), _p1(p1), _p2(p2), _p3(p3), _p4(p4)
        {}

        virtual void operator()() 
        { ((*_obj).*_objfn)(_p1, _p2, _p3, _p4); }
    private:
        ObjPtr _obj;
        ObjFunPtr _objfn;
        Param1    _p1;
        Param2    _p2;
        Param3    _p3;
        Param4    _p4;
    };

    // -------------------------------
    //
    // StaticFunctor[0-4]
    //
    // -------------------------------
    template<class FnPtr>
    class StaticFunctor0 : public Functor
    {
    public:
        StaticFunctor0(const FnPtr& fn)
        : _fn(fn)
        {}

        virtual void operator()() 
        { _fn(); }
    private:
        FnPtr _fn;
    };

    template<class FnPtr, class Param1>
    class StaticFunctor1 : public Functor
    {
    public:
        StaticFunctor1(const FnPtr& fn, Param1 p1)
        : _fn(fn), _p1(p1)
        {}

        virtual void operator()() 
        { _fn(_p1); }
    private:
        FnPtr  _fn;
        Param1 _p1;
    };


    template<class FnPtr, class Param1, class Param2>
    class StaticFunctor2 : public Functor
    {
    public:
        StaticFunctor2(const FnPtr& fn, Param1 p1, Param2 p2)
        : _fn(fn), _p1(p1), _p2(p2)
        {}

        virtual void operator()() 
        { _fn(_p1, _p2); }
    private:
        FnPtr  _fn;
        Param1 _p1;
        Param2 _p2;
    };

    template<class FnPtr, class Param1, class Param2, class Param3>
    class StaticFunctor3 : public Functor
    {
    public:
        StaticFunctor3(const FnPtr& fn, Param1 p1, Param2 p2, Param3 p3)
        : _fn(fn), _p1(p1), _p2(p2), _p3(p3)
        {}

        virtual void operator()() 
        { _fn(_p1, _p2, _p3); }
    private:
        FnPtr  _fn;
        Param1 _p1;
        Param2 _p2;
        Param3 _p3;
    };

    template<class FnPtr, class Param1, class Param2, class Param3, 
         class Param4>
    class StaticFunctor4 : public Functor
    {
    public:
        StaticFunctor4(const FnPtr& fn, Param1 p1, Param2 p2, Param3 p3,
               Param4 p4)
        : _fn(fn), _p1(p1), _p2(p2), _p3(p3), _p4(p4)
        {}

        virtual void operator()() 
        { _fn(_p1, _p2, _p3, _p4); }
    private:
        FnPtr  _fn;
        Param1 _p1;
        Param2 _p2;
        Param3 _p3;
        Param4 _p4;
    };

    // -------------------------------
    //
    // Wrap -- static functions/methods
    //
    // -------------------------------
    template<class FunRet>
    inline Functor* wrap( FunRet(*fn)())
    {
        return new StaticFunctor0
        <FunRet(*)()>
        (fn);
    }

    template<class FunRet, 
         class Type1, class Param1>
    inline Functor* wrap( FunRet (*fn)(Type1),
                   const Param1& p1)
    {
        return new StaticFunctor1
        <FunRet(*)(Type1), Param1>
        (fn, p1);
    }

    template<class FunRet, 
         class Type1, class Param1,
         class Type2, class Param2>
    inline Functor* wrap( FunRet (*fn)(Type1, Type2),
                   const Param1& p1, const Param2& p2)
    {
        return new StaticFunctor2
        <FunRet(*)(Type1, Type2), Param1, Param2>
        (fn, p1, p2);
    }
 
    template<class FunRet, 
         class Type1, class Param1,
         class Type2, class Param2,
         class Type3, class Param3>
    inline Functor* wrap( FunRet (*fn)(Type1, Type2, Type3),
                   const Param1& p1, const Param2& p2,
                   const Param3& p3)
    {
        return new StaticFunctor3
        <FunRet(*)(Type1, Type2, Type3), Param1, Param2, Param3>
        (fn, p1, p2, p3);
    }

    template<class FunRet, 
         class Type1, class Param1,
         class Type2, class Param2,
         class Type3, class Param3,
         class Type4, class Param4>
    inline Functor* wrap( FunRet (*fn)(Type1, Type2, Type3, Type4),
                   const Param1& p1, const Param2& p2,
                   const Param3& p3, const Param4& p4)
    {
        return new StaticFunctor4
        <FunRet(*)(Type1, Type2, Type3, Type4), 
        Param1, Param2, Param3, Param4>
        (fn, p1, p2, p3, p4);
    }

    // -------------------------------
    //
    // Wrap -- non-const, methods
    //
    // -------------------------------
    template<class ObjPtr, class Obj, class ObjFunRet>
    inline Functor* wrap( const ObjPtr& objptr, ObjFunRet (Obj::*objfn)())
    {
        return new MemberFunctor0<ObjPtr, ObjFunRet(Obj::*)()>(objptr, objfn);
    }

    template<class ObjPtr, class Obj, class ObjFunRet, 
         class Type1, class Param1>
    inline Functor* wrap( const ObjPtr& objptr, ObjFunRet (Obj::*objfn)(Type1),
                   const Param1& p1)
    {
        return new MemberFunctor1
        <ObjPtr, ObjFunRet(Obj::*)(Type1), 
        Param1>
        (objptr, objfn, p1);
    }

    template<class ObjPtr, class Obj, class ObjFunRet, 
         class Type1, class Param1,
         class Type2, class Param2>
    inline Functor* wrap( const ObjPtr& objptr, 
                   ObjFunRet (Obj::*objfn)(Type1, Type2),
                   const Param1& p1, const Param2& p2)
    {
        return new MemberFunctor2
        <ObjPtr, ObjFunRet(Obj::*)(Type1, Type2), 
        Param1, Param2>
        (objptr, objfn, p1, p2);
    }

    template<class ObjPtr, class Obj, class ObjFunRet, 
         class Type1, class Param1,
         class Type2, class Param2,
         class Type3, class Param3>
    inline Functor* wrap( const ObjPtr& objptr, 
                   ObjFunRet (Obj::*objfn)(Type1, Type2, Type3),
                   const Param1& p1, 
                   const Param2& p2,
                   const Param3& p3)
    {
        return new MemberFunctor3
        <ObjPtr, ObjFunRet(Obj::*)(Type1, Type2, Type3), 
        Param1, Param2, Param3>
        (objptr, objfn, p1, p2, p3);
    }

    template<class ObjPtr, class Obj, class ObjFunRet, 
         class Type1, class Param1,
         class Type2, class Param2,
         class Type3, class Param3,
         class Type4, class Param4>
    inline Functor* wrap( const ObjPtr& objptr, 
                   ObjFunRet (Obj::*objfn)(Type1, Type2, Type3, Type4),
                   const Param1& p1, 
                   const Param2& p2,
                   const Param3& p3,
                   const Param4& p4)
    {
        return new MemberFunctor4
        <ObjPtr, ObjFunRet(Obj::*)(Type1, Type2, Type3, Type4), 
        Param1, Param2, Param3, Param4>
        (objptr, objfn, p1, p2, p3, p4);
    }

    // -------------------------------
    //
    // Wrap -- const, methods
    //
    // -------------------------------
    template<class ObjPtr, class Obj, class ObjFunRet>
    inline Functor* wrap( const ObjPtr& objptr, ObjFunRet (Obj::*objfn)() const)
    {
        return new MemberFunctor0<ObjPtr, ObjFunRet(Obj::*)() const >(objptr, objfn);
    }

    template<class ObjPtr, class Obj, class ObjFunRet, 
         class Type1, class Param1>
    inline Functor* wrap( const ObjPtr& objptr, ObjFunRet (Obj::*objfn)(Type1) const,
                   const Param1& p1)
    {
        return new MemberFunctor1
        <ObjPtr, ObjFunRet(Obj::*)(Type1) const , Param1>
        (objptr, objfn, p1);
    }

    template<class ObjPtr, class Obj, class ObjFunRet, 
         class Type1, class Param1,
         class Type2, class Param2>
    inline Functor* wrap( const ObjPtr& objptr, 
                   ObjFunRet (Obj::*objfn)(Type1, Type2) const,
                   const Param1& p1, const Param2& p2)
    {
        return new MemberFunctor2
        <ObjPtr, ObjFunRet(Obj::*)(Type1, Type2) const, Param1, Param2>
        (objptr, objfn, p1, p2);
    }

    template<class ObjPtr, class Obj, class ObjFunRet, 
         class Type1, class Param1,
         class Type2, class Param2,
         class Type3, class Param3>
    inline Functor* wrap( const ObjPtr& objptr, 
                   ObjFunRet (Obj::*objfn)(Type1, Type2, Type3) const,
                   const Param1& p1, 
                   const Param2& p2,
                   const Param3& p3)
    {
        return new MemberFunctor3
        <ObjPtr, ObjFunRet(Obj::*)(Type1, Type2, Type3) const, 
        Param1, Param2, Param3>
        (objptr, objfn, p1, p2, p3);
    }

    template<class ObjPtr, class Obj, class ObjFunRet, 
         class Type1, class Param1,
         class Type2, class Param2,
         class Type3, class Param3,
         class Type4, class Param4>
    inline Functor* wrap( const ObjPtr& objptr, 
                   ObjFunRet (Obj::*objfn)(Type1, Type2, Type3, Type4) const,
                   const Param1& p1, 
                   const Param2& p2,
                   const Param3& p3,
                   const Param4& p4)
    {
        return new MemberFunctor4
        <ObjPtr, ObjFunRet(Obj::*)(Type1, Type2, Type3, Type4) const, 
        Param1, Param2, Param3, Param4>
        (objptr, objfn, p1, p2, p3, p4);
    }


    }
}

#endif
