#ifndef V_VAbstractFunction_Interface
#define V_VAbstractFunction_Interface

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

/*****************************************
 *****  Abstract Function Templates  *****
 *****************************************/

namespace V {
    template <class R> class Vaf0 {
    public:
	virtual R operator() () const = 0;
    };

    template <class R,class P1> class Vaf1 {
    public:
	virtual R operator() (P1 iP1) const = 0;
    };

    template <class R,class P1,class P2> class Vaf2 {
    public:
	virtual R operator() (P1 iP1, P2 iP2) const = 0;
    };

    template <class R,class P1,class P2,class P3> class Vaf3 {
    public:
	virtual R operator() (P1 iP1, P2 iP2, P3 iP3) const = 0;
    };

    template <class R,class P1,class P2,class P3,class P4> class Vaf4 {
    public:
	virtual R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4) const = 0;
    };

    template <class R,class P1,class P2,class P3,class P4,class P5> class Vaf5 {
    public:
	virtual R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5) const = 0;
    };

    template <class R,class P1,class P2,class P3,class P4,class P5,class P6> class Vaf6 {
    public:
	virtual R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6) const = 0;
    };

    template <class R,class P1,class P2,class P3,class P4,class P5,class P6,class P7>
    class Vaf7 {
    public:
	virtual R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6, P7 iP7) const = 0;
    };
    

    /**********************************************************
     *****  Pointer To Function Implementation Templates  *****
     **********************************************************/

    /*---------------*
     *----  f()  ----*
     *---------------*/

    template <class R> class Vaf0f : public Vaf0<R> {
    //  Pointer To Member Type
    public:
	typedef R (*Action) ();

    //  Construction
    public:
	Vaf0f (Action pAction) : m_pAction (pAction) {
	}
	Vaf0f (Vaf0f<R> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	R operator() () const {
	    return (*m_pAction)();
	}

    //  State
    protected:
	Action m_pAction;
    };

    /*-----------------*
     *----  f(p1)  ----*
     *-----------------*/

    template <class R,class P1> class Vaf1f : public Vaf1<R,P1> {
    //  Pointer To Member Type
    public:
	typedef R (*Action) (P1);

    //  Construction
    public:
	Vaf1f (Action pAction) : m_pAction (pAction) {
	}
	Vaf1f (Vaf1f<R,P1> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	R operator() (P1 iP1) const {
	    return (*m_pAction)(iP1);
	}

    //  State
    protected:
	Action m_pAction;
    };
    

    /*--------------------*
     *----  f(p1,p2)  ----*
     *--------------------*/

    template <class R,class P1,class P2> class Vaf2f : public Vaf2<R,P1,P2> {
    //  Pointer To Member Type
    public:
	typedef R (*Action) (P1,P2);

    //  Construction
    public:
	Vaf2f (Action pAction) : m_pAction (pAction) {
	}
	Vaf2f (Vaf2f<R,P1,P2> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	R operator() (P1 iP1, P2 iP2) const {
	    return (*m_pAction)(iP1, iP2);
	}

    //  State
    protected:
	Action m_pAction;
    };

    /*-----------------------*
     *----  f(p1,p2,p3)  ----*
     *-----------------------*/

    template <class R,class P1,class P2,class P3>
    class Vaf3f : public Vaf3<R,P1,P2,P3> {
    //  Pointer To Member Type
    public:
	typedef R (*Action) (P1,P2,P3);

    //  Construction
    public:
	Vaf3f (Action pAction) : m_pAction (pAction) {
	}
	Vaf3f (Vaf3f<R,P1,P2,P3> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	R operator() (P1 iP1, P2 iP2, P3 iP3) const {
	    return (*m_pAction)(iP1, iP2, iP3);
	}

    //  State
    protected:
	Action m_pAction;
    };
    

    /*--------------------------*
     *----  f(p1,p2,p3,p4)  ----*
     *--------------------------*/

    template <class R,class P1,class P2,class P3,class P4>
    class Vaf4f : public Vaf4<R,P1,P2,P3,P4> {
    //  Pointer To Member Type
    public:
	typedef R (*Action) (P1,P2,P3,P4);

    //  Construction
    public:
	Vaf4f (Action pAction) : m_pAction (pAction) {
	}
	Vaf4f (Vaf4f<R,P1,P2,P3,P4> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4) const {
	    return (*m_pAction)(iP1, iP2, iP3, iP4);
	}

    //  State
    protected:
	Action m_pAction;
    };

    /*-----------------------------*
     *----  f(p1,p2,p3,p4,p5)  ----*
     *-----------------------------*/

    template <class R,class P1,class P2,class P3,class P4,class P5>
    class Vaf5f : public Vaf5<R,P1,P2,P3,P4,P5> {
    //  Pointer To Member Type
    public:
	typedef R (*Action) (P1,P2,P3,P4,P5);

    //  Construction
    public:
	Vaf5f (Action pAction) : m_pAction (pAction) {
	}
	Vaf5f (Vaf5f<R,P1,P2,P3,P4,P5> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5) const {
	    return (*m_pAction)(iP1, iP2, iP3, iP4, iP5);
	}

    //  State
    protected:
	Action m_pAction;
    };
    

    /*--------------------------------*
     *----  f(p1,p2,p3,p4,p5,p6)  ----*
     *--------------------------------*/

    template <class R,class P1,class P2,class P3,class P4,class P5,class P6>
    class Vaf6f : public Vaf6<R,P1,P2,P3,P4,P5,P6> {
    //  Pointer To Member Type
    public:
	typedef R (*Action) (P1,P2,P3,P4,P5,P6);

    //  Construction
    public:
	Vaf6f (Action pAction) : m_pAction (pAction) {
	}
	Vaf6f (Vaf6f<R,P1,P2,P3,P4,P5,P6> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6) const {
	    return (*m_pAction)(iP1, iP2, iP3, iP4, iP5, iP6);
	}

    //  State
    protected:
	Action m_pAction;
    };

    /*-----------------------------------*
     *----  f(p1,p2,p3,p4,p5,p6,p7)  ----*
     *-----------------------------------*/

    template <class R,class P1,class P2,class P3,class P4,class P5,class P6,class P7>
    class Vaf7f : public Vaf7<R,P1,P2,P3,P4,P5,P6,P7> {
    //  Pointer To Member Type
    public:
	typedef R (*Action) (P1,P2,P3,P4,P5,P6,P7);

    //  Construction
    public:
	Vaf7f (Action pAction) : m_pAction (pAction) {
	}
	Vaf7f (Vaf7f<R,P1,P2,P3,P4,P5,P6,P7> const& rOther) : m_pAction (rOther.m_pAction) {
	}

    //  Update
    public:
	void setActionTo (Action pAction) {
	    m_pAction = pAction;
	}

    //  Use
    public:
	R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6, P7 iP7) const {
	    return (*m_pAction)(iP1, iP2, iP3, iP4, iP5, iP6, iP7);
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

    template <class R,class Actor> class Vaf0m : public Vaf0<R> {
    //  Pointer To Member Type
    public:
	typedef R (Actor::*Action) ();

    //  Construction
    public:
	Vaf0m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vaf0m (Vaf0m<R,Actor> const& rOther)
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
	R operator() () const {
	    return (m_pActor->*m_pAction)();
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };

    /*-----------------*
     *----  f(p1)  ----*
     *-----------------*/

    template <class R,class Actor,class P1> class Vaf1m : public Vaf1<R,P1> {
    //  Pointer To Member Type
    public:
	typedef R (Actor::*Action) (P1);

    //  Construction
    public:
	Vaf1m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vaf1m (Vaf1m<R,Actor,P1> const& rOther)
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
	R operator() (P1 iP1) const {
	    return (m_pActor->*m_pAction)(iP1);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };
    

    /*--------------------*
     *----  f(p1,p2)  ----*
     *--------------------*/

    template <class R,class Actor,class P1,class P2> class Vaf2m : public Vaf2<R,P1,P2> {
    //  Pointer To Member Type
    public:
	typedef R (Actor::*Action) (P1,P2);

    //  Construction
    public:
	Vaf2m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vaf2m (Vaf2m<R,Actor,P1,P2> const& rOther)
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
	R operator() (P1 iP1, P2 iP2) const {
	    return (m_pActor->*m_pAction)(iP1, iP2);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };

    /*-----------------------*
     *----  f(p1,p2,p3)  ----*
     *-----------------------*/

    template <class R,class Actor,class P1,class P2,class P3>
    class Vaf3m : public Vaf3<R,P1,P2,P3> {
    //  Pointer To Member Type
    public:
	typedef R (Actor::*Action) (P1,P2,P3);

    //  Construction
    public:
	Vaf3m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vaf3m (Vaf3m<R,Actor,P1,P2,P3> const& rOther)
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
	R operator() (P1 iP1, P2 iP2, P3 iP3) const {
	    return (m_pActor->*m_pAction)(iP1, iP2, iP3);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };
    

    /*--------------------------*
     *----  f(p1,p2,p3,p4)  ----*
     *--------------------------*/

    template <class R,class Actor,class P1,class P2,class P3,class P4>
    class Vaf4m : public Vaf4<R,P1,P2,P3,P4> {
    //  Pointer To Member Type
    public:
	typedef R (Actor::*Action) (P1,P2,P3,P4);

    //  Construction
    public:
	Vaf4m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vaf4m (Vaf4m<R,Actor,P1,P2,P3,P4> const& rOther)
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
	R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4) const {
	    return (m_pActor->*m_pAction)(iP1, iP2, iP3, iP4);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };

    /*-----------------------------*
     *----  f(p1,p2,p3,p4,p5)  ----*
     *-----------------------------*/

    template <class R,class Actor,class P1,class P2,class P3,class P4,class P5>
    class Vaf5m : public Vaf5<R,P1,P2,P3,P4,P5> {
    //  Pointer To Member Type
    public:
	typedef R (Actor::*Action) (P1,P2,P3,P4,P5);

    //  Construction
    public:
	Vaf5m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vaf5m (Vaf5m<R,Actor,P1,P2,P3,P4,P5> const& rOther)
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
	R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5) const {
	    return (m_pActor->*m_pAction)(iP1, iP2, iP3, iP4, iP5);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };
    

    /*--------------------------------*
     *----  f(p1,p2,p3,p4,p5,p6)  ----*
     *--------------------------------*/

    template <class R,class Actor,class P1,class P2,class P3,class P4,class P5,class P6>
    class Vaf6m : public Vaf6<R,P1,P2,P3,P4,P5,P6> {
    //  Pointer To Member Type
    public:
	typedef R (Actor::*Action) (P1,P2,P3,P4,P5,P6);

    //  Construction
    public:
	Vaf6m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vaf6m (Vaf6m<R,Actor,P1,P2,P3,P4,P5,P6> const& rOther)
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
	R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6) const {
	    return (m_pActor->*m_pAction)(iP1, iP2, iP3, iP4, iP5, iP6);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };

    /*-----------------------------------*
     *----  f(p1,p2,p3,p4,p5,p6,p7)  ----*
     *-----------------------------------*/

    template <class R,class Actor,class P1,class P2,class P3,class P4,class P5,class P6,class P7>
    class Vaf7m : public Vaf7<R,P1,P2,P3,P4,P5,P6,P7> {
    //  Pointer To Member Type
    public:
	typedef R (Actor::*Action) (P1,P2,P3,P4,P5,P6,P7);

    //  Construction
    public:
	Vaf7m (Actor *pActor, Action pAction) : m_pActor (pActor), m_pAction (pAction) {
	}
	Vaf7m (Vaf7m<R,Actor,P1,P2,P3,P4,P5,P6,P7> const& rOther)
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
	R operator() (P1 iP1, P2 iP2, P3 iP3, P4 iP4, P5 iP5, P6 iP6, P7 iP7) const {
	    return (m_pActor->*m_pAction)(iP1, iP2, iP3, iP4, iP5, iP6, iP7);
	}

    //  State
    protected:
	Actor *m_pActor;
	Action m_pAction;
    };
}

#endif
