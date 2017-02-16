#ifndef V_VAbstractProcedure_Interface
#define V_VAbstractProcedure_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

/******************************************
 *****  Abstract Procedure Templates  *****
 ******************************************/

namespace V {
    class Vap0 {
    public:
	virtual void operator() () const = 0;
    };

    template <class P1> class Vap1 {
    public:
	virtual void operator() (P1 iP1) const = 0;
    };

    template <class P1,class P2> class Vap2 {
    public:
	virtual void operator() (P1 iP1, P2 iP2) const = 0;
    };

    template <class P1,class P2,class P3> class Vap3 {
    public:
	virtual void operator() (P1 iP1, P2 iP2, P3 iP3) const = 0;
    };

    template <class P1,class P2,class P3,class P4> class Vap4 {
    public:
	virtual void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4) const = 0;
    };

    template <class P1,class P2,class P3,class P4,class P5> class Vap5 {
    public:
	virtual void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5) const = 0;
    };

    template <class P1,class P2,class P3,class P4,class P5,class P6> class Vap6 {
    public:
	virtual void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6) const = 0;
    };

    template <class P1,class P2,class P3,class P4,class P5,class P6,class P7> class Vap7 {
    public:
	virtual void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6, P7 iP7) const = 0;
    };
    

    /**********************************************************
     *****  Pointer To Function Implementation Templates  *****
     **********************************************************/

    /*---------------*
     *----  f()  ----*
     *---------------*/

    class Vap0f : public Vap0 {
    //  Pointer To Member Type
    public:
	typedef void (*Action) ();

    //  Construction
    public:
	Vap0f (Action pAction) : m_pAction (pAction) {
	}
	Vap0f (Vap0f const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() () const {
	    (*m_pAction)();
	}

    //  State
    protected:
	Action m_pAction;
    };

    /*-----------------*
     *----  f(p1)  ----*
     *-----------------*/

    template <class P1> class Vap1f : public Vap1<P1> {
    //  Pointer To Member Type
    public:
	typedef void (*Action) (P1);

    //  Construction
    public:
	Vap1f (Action pAction) : m_pAction (pAction) {
	}
	Vap1f (Vap1f<P1> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1) const {
	    (*m_pAction)(iP1);
	}

    //  State
    protected:
	Action m_pAction;
    };
    

    /*--------------------*
     *----  f(p1,p2)  ----*
     *--------------------*/

    template <class P1,class P2> class Vap2f : public Vap2<P1,P2> {
    //  Pointer To Member Type
    public:
	typedef void (*Action) (P1,P2);

    //  Construction
    public:
	Vap2f (Action pAction) : m_pAction (pAction) {
	}
	Vap2f (Vap2f<P1,P2> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2) const {
	    (*m_pAction)(iP1, iP2);
	}

    //  State
    protected:
	Action m_pAction;
    };

    /*-----------------------*
     *----  f(p1,p2,p3)  ----*
     *-----------------------*/

    template <class P1,class P2,class P3> class Vap3f : public Vap3<P1,P2,P3> {
    //  Pointer To Member Type
    public:
	typedef void (*Action) (P1,P2,P3);

    //  Construction
    public:
	Vap3f (Action pAction) : m_pAction (pAction) {
	}
	Vap3f (Vap3f<P1,P2,P3> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2, P3 iP3) const {
	    (*m_pAction)(iP1, iP2, iP3);
	}

    //  State
    protected:
	Action m_pAction;
    };
    

    /*--------------------------*
     *----  f(p1,p2,p3,p4)  ----*
     *--------------------------*/

    template <class P1,class P2,class P3,class P4> class Vap4f : public Vap4<P1,P2,P3,P4> {
    //  Pointer To Member Type
    public:
	typedef void (*Action) (P1,P2,P3,P4);

    //  Construction
    public:
	Vap4f (Action pAction) : m_pAction (pAction) {
	}
	Vap4f (Vap4f<P1,P2,P3,P4> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4) const {
	    (*m_pAction)(iP1, iP2, iP3, iP4);
	}

    //  State
    protected:
	Action m_pAction;
    };

    /*-----------------------------*
     *----  f(p1,p2,p3,p4,p5)  ----*
     *-----------------------------*/

    template <class P1,class P2,class P3,class P4,class P5>
    class Vap5f : public Vap5<P1,P2,P3,P4,P5> {
    //  Pointer To Member Type
    public:
	typedef void (*Action) (P1,P2,P3,P4,P5);

    //  Construction
    public:
	Vap5f (Action pAction) : m_pAction (pAction) {
	}
	Vap5f (Vap5f<P1,P2,P3,P4,P5> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5) const {
	    (*m_pAction)(iP1, iP2, iP3, iP4, iP5);
	}

    //  State
    protected:
	Action m_pAction;
    };
    

    /*--------------------------------*
     *----  f(p1,p2,p3,p4,p5,p6)  ----*
     *--------------------------------*/

    template <class P1,class P2,class P3,class P4,class P5,class P6>
    class Vap6f : public Vap6<P1,P2,P3,P4,P5,P6> {
    //  Pointer To Member Type
    public:
	typedef void (*Action) (P1,P2,P3,P4,P5,P6);

    //  Construction
    public:
	Vap6f (Action pAction) : m_pAction (pAction) {
	}
	Vap6f (Vap6f<P1,P2,P3,P4,P5,P6> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6) const {
	    (*m_pAction)(iP1, iP2, iP3, iP4, iP5, iP6);
	}

    //  State
    protected:
	Action m_pAction;
    };

    /*-----------------------------------*
     *----  f(p1,p2,p3,p4,p5,p6,p7)  ----*
     *-----------------------------------*/

    template <class P1,class P2,class P3,class P4,class P5,class P6,class P7>
    class Vap7f : public Vap7<P1,P2,P3,P4,P5,P6,P7> {
    //  Pointer To Member Type
    public:
	typedef void (*Action) (P1,P2,P3,P4,P5,P6,P7);

    //  Construction
    public:
	Vap7f (Action pAction) : m_pAction (pAction) {
	}
	Vap7f (Vap7f<P1,P2,P3,P4,P5,P6,P7> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6, P7 iP7) const {
	    (*m_pAction)(iP1, iP2, iP3, iP4, iP5, iP6, iP7);
	}

    //  State
    protected:
	Action m_pAction;
    };
    

    /********************************************************
     *****  Pointer To Member Implementation Templates  *****
     ********************************************************/

    /*---------------*
     *----  f()  ----*
     *---------------*/

    template <class Actor> class Vap0m : public Vap0 {
    //  Pointer To Member Type
    public:
	typedef void (Actor::*Action) ();

    //  Construction
    public:
	Vap0m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vap0m (Vap0m<Actor> const& rOther)
	    : m_pActor (rOther.m_pActor), m_pAction (rOther.m_pAction)
	{
	}

    //  Update
    public:
	void setActorTo (Actor *pActor) {
	    m_pActor = pActor;
	}
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() () const {
	    (m_pActor->*m_pAction)();
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };

    /*-----------------*
     *----  f(p1)  ----*
     *-----------------*/

    template <class Actor,class P1> class Vap1m : public Vap1<P1> {
    //  Pointer To Member Type
    public:
	typedef void (Actor::*Action) (P1);

    //  Construction
    public:
	Vap1m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vap1m (Vap1m<Actor,P1> const& rOther)
	    : m_pActor (rOther.m_pActor), m_pAction (rOther.m_pAction)
	{
	}

    //  Update
    public:
	void setActorTo (Actor *pActor) {
	    m_pActor = pActor;
	}
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1) const {
	    (m_pActor->*m_pAction)(iP1);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };
    

    /*--------------------*
     *----  f(p1,p2)  ----*
     *--------------------*/

    template <class Actor,class P1,class P2> class Vap2m : public Vap2<P1,P2> {
    //  Pointer To Member Type
    public:
	typedef void (Actor::*Action) (P1,P2);

    //  Construction
    public:
	Vap2m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vap2m (Vap2m<Actor,P1,P2> const& rOther)
	    : m_pActor (rOther.m_pActor), m_pAction (rOther.m_pAction)
	{
	}

    //  Update
    public:
	void setActorTo (Actor *pActor) {
	    m_pActor = pActor;
	}
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2) const {
	    (m_pActor->*m_pAction)(iP1, iP2);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };

    /*-----------------------*
     *----  f(p1,p2,p3)  ----*
     *-----------------------*/

    template <class Actor,class P1,class P2,class P3> class Vap3m : public Vap3<P1,P2,P3> {
    //  Pointer To Member Type
    public:
	typedef void (Actor::*Action) (P1,P2,P3);

    //  Construction
    public:
	Vap3m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vap3m (Vap3m<Actor,P1,P2,P3> const& rOther)
	    : m_pActor (rOther.m_pActor), m_pAction (rOther.m_pAction)
	{
	}

    //  Update
    public:
	void setActorTo (Actor *pActor) {
	    m_pActor = pActor;
	}
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2, P3 iP3) const {
	    (m_pActor->*m_pAction)(iP1, iP2, iP3);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };
    

    /*--------------------------*
     *----  f(p1,p2,p3,p4)  ----*
     *--------------------------*/

    template <class Actor,class P1,class P2,class P3,class P4>
    class Vap4m : public Vap4<P1,P2,P3,P4> {
    //  Pointer To Member Type
    public:
	typedef void (Actor::*Action) (P1,P2,P3,P4);

    //  Construction
    public:
	Vap4m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vap4m (Vap4m<Actor,P1,P2,P3,P4> const& rOther)
	    : m_pActor (rOther.m_pActor), m_pAction (rOther.m_pAction)
	{
	}

    //  Update
    public:
	void setActorTo (Actor *pActor) {
	    m_pActor = pActor;
	}
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4) const {
	    (m_pActor->*m_pAction)(iP1, iP2, iP3, iP4);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };

    /*-----------------------------*
     *----  f(p1,p2,p3,p4,p5)  ----*
     *-----------------------------*/

    template <class Actor,class P1,class P2,class P3,class P4,class P5>
    class Vap5m : public Vap5<P1,P2,P3,P4,P5> {
    //  Pointer To Member Type
    public:
	typedef void (Actor::*Action) (P1,P2,P3,P4,P5);

    //  Construction
    public:
	Vap5m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vap5m (Vap5m<Actor,P1,P2,P3,P4,P5> const& rOther)
	    : m_pActor (rOther.m_pActor), m_pAction (rOther.m_pAction)
	{
	}

    //  Update
    public:
	void setActorTo (Actor *pActor) {
	    m_pActor = pActor;
	}
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5) const {
	    (m_pActor->*m_pAction)(iP1, iP2, iP3, iP4, iP5);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };
    

    /*--------------------------------*
     *----  f(p1,p2,p3,p4,p5,p6)  ----*
     *--------------------------------*/

    template <class Actor,class P1,class P2,class P3,class P4,class P5,class P6>
    class Vap6m : public Vap6<P1,P2,P3,P4,P5,P6> {
    //  Pointer To Member Type
    public:
	typedef void (Actor::*Action) (P1,P2,P3,P4,P5,P6);

    //  Construction
    public:
	Vap6m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vap6m (Vap6m<Actor,P1,P2,P3,P4,P5,P6> const& rOther)
	    : m_pActor (rOther.m_pActor), m_pAction (rOther.m_pAction)
	{
	}

    //  Update
    public:
	void setActorTo (Actor *pActor) {
	    m_pActor = pActor;
	}
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6) const {
	    (m_pActor->*m_pAction)(iP1, iP2, iP3, iP4, iP5, iP6);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };

    /*-----------------------------------*
     *----  f(p1,p2,p3,p4,p5,p6,p7)  ----*
     *-----------------------------------*/

    template <class Actor,class P1,class P2,class P3,class P4,class P5,class P6,class P7>
    class Vap7m : public Vap7<P1,P2,P3,P4,P5,P6,P7> {
    //  Pointer To Member Type
    public:
	typedef void (Actor::*Action) (P1,P2,P3,P4,P5,P6,P7);

    //  Construction
    public:
	Vap7m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vap7m (Vap7m<Actor,P1,P2,P3,P4,P5,P6,P7> const& rOther)
	    : m_pActor (rOther.m_pActor), m_pAction (rOther.m_pAction)
	{
	}

    //  Update
    public:
	void setActorTo (Actor *pActor) {
	    m_pActor = pActor;
	}
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	void operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6, P7 iP7) const {
	    (m_pActor->*m_pAction)(iP1, iP2, iP3, iP4, iP5, iP6, iP7);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };
}


#endif
