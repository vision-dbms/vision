#ifndef VSymbolImplementation_Interface
#define VSymbolImplementation_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VSNFTask.h"

/*************************
 *****  Definitions  *****
 *************************/

/*------------------------------------*
 *----  Base Implementation Type  ----*
 *------------------------------------*/

class VSymbolImplementation : public VTransient {
//  Task Implementation
public:
    virtual void implement (VSNFTask* pTask) const = 0;
};


/*-----------------------------------------*
 *----  Constant Implementation Types  ----*
 *-----------------------------------------*/

class VsiNA : public VSymbolImplementation {
public:
    VsiNA () {
    }

protected:
    void implement (VSNFTask* pTask) const;
};

class VsiTrue : public VSymbolImplementation {
public:
    VsiTrue () {
    }

protected:
    void implement (VSNFTask* pTask) const;
};

class VsiFalse : public VSymbolImplementation {
public:
    VsiFalse () {
    }

protected:
    void implement (VSNFTask* pTask) const;
};


/*-------------------------*
 *----  Sink Template  ----*
 *-------------------------*/

template <class R> class VSymbolImplementationSink : public VSymbolImplementation {
//  Sink Type
public:
    typedef void (VSNFTask::*Sink) (R) const;

//  Construction
protected:
    VSymbolImplementationSink (Sink pSink) : m_pSink (pSink) {
    }

//  State
protected:
    Sink const m_pSink;
};


/*------------------------------------*
 *----  Implementation Templates  ----*
 *------------------------------------*/

/**********************************************
 *****  Conditional Instance Path Access  *****
 **********************************************/

template <class T, class R> class Vsi_cipath_c : public VSymbolImplementation {
//  Member Type
public:
    typedef bool (T::*Source) (
	M_CPD*& rpReordering, rtLINK_CType*& rpRestriction, R& rDatum
    ) const;

//  Construction
public:
    Vsi_cipath_c (Source pSource) : m_pSource (pSource) {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};


/******************************************
 *****  Conditional Reference Access  *****
 ******************************************/

template <class T, class R> class Vsi_ciref : public VSymbolImplementation {
//  Member Type
public:
    typedef bool (T::*Source) (R& rDatum);

//  Construction
public:
    Vsi_ciref (Source pSource) : m_pSource (pSource) {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};

template <class T, class R> class Vsi_ciref_c : public VSymbolImplementation {
//  Member Type
public:
    typedef bool (T::*Source) (R& rDatum) const;

//  Construction
public:
    Vsi_ciref_c (Source pSource) : m_pSource (pSource) {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};


/********************************************
 *****  Unconditional Reference Access  *****
 ********************************************/

template <class T, class R> class Vsi_uiref : public VSymbolImplementation {
//  Member Type
public:
    typedef R (T::*Source) ();

//  Construction
public:
    Vsi_uiref (Source pSource) : m_pSource (pSource) {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};

template <class T, class R> class Vsi_uiref_c : public VSymbolImplementation {
//  Member Type
public:
    typedef R (T::*Source) () const;

//  Construction
public:
    Vsi_uiref_c (Source pSource) : m_pSource (pSource) {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};


/**************************
 *****  Duc Feathers  *****
 **************************/

template <class T> class Vsi_setDucFeathers : public VSymbolImplementation {
//  Construction
public:
    Vsi_setDucFeathers () {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;
};


/***********************
 *****  Functions  *****
 ***********************/

template <class T, class R> class Vsi_f0 : public VSymbolImplementationSink<R> {
//  Member Type
public:
    typedef R (T::*Source) ();

//  Construction
public:
    Vsi_f0 (
	Source pSource,
	typename VSymbolImplementationSink<R>::Sink pSink = &VSNFTask::loadDucWith
    ) : VSymbolImplementationSink<R> (pSink), m_pSource (pSource)
    {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};

template <class T, class R> class Vsi_f0_c : public VSymbolImplementationSink<R> {
//  Member Type
public:
    typedef R (T::*Source) () const;

//  Construction
public:
    Vsi_f0_c (
	Source pSource,
	typename VSymbolImplementationSink<R>::Sink pSink = &VSNFTask::loadDucWith
    ) : VSymbolImplementationSink<R> (pSink), m_pSource (pSource)
    {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};


template <class T, class A1, class R> class Vsi_f1 : public VSymbolImplementationSink<R> {
//  Member Type
public:
    typedef R (T::*Source) (A1* pA1);

//  Construction
public:
    Vsi_f1 (
	Source pSource, char const* pConverse,
	typename VSymbolImplementationSink<R>::Sink pSink = &VSNFTask::loadDucWith
    ) : VSymbolImplementationSink<R> (pSink), m_pSource (pSource), m_pConverse (pConverse)
    {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    char const* const	m_pConverse;
    Source const	m_pSource;
};


/*********************************
 *****  Functions (Boolean)  *****
 *********************************/

template <class T> class Vsi_b0 : public VSymbolImplementationSink<bool> {
//  Member Type
public:
    typedef bool (T::*Source) ();

//  Construction
public:
    Vsi_b0 (Source pSource)
	: VSymbolImplementationSink<bool> (&VSNFTask::loadDucWithBoolean)
	, m_pSource (pSource)
    {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};

template <class T> class Vsi_b0_c : public VSymbolImplementationSink<bool> {
//  Member Type
public:
    typedef bool (T::*Source) () const;

//  Construction
public:
    Vsi_b0_c (Source pSource)
	: VSymbolImplementationSink<bool> (&VSNFTask::loadDucWithBoolean)
	, m_pSource (pSource)
    {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};


template <class T, class A1> class Vsi_b1 : public VSymbolImplementationSink<bool> {
//  Member Type
public:
    typedef bool (T::*Source) (A1* pA1);

//  Construction
public:
    Vsi_b1 (Source pSource, char const* pConverse)
	: VSymbolImplementationSink<bool> (&VSNFTask::loadDucWithBoolean)
	, m_pSource (pSource), m_pConverse (pConverse)
    {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    char const* const	m_pConverse;
    Source const	m_pSource;
};


/****************************************
 *****  Functions (Representative)  *****
 ****************************************/

template <class T, class RSource, class RSink> class Vsi_r0 : public VSymbolImplementationSink<RSink> {
//  Member Type
public:
    typedef RSource (T::*Source) ();

//  Construction
public:
    Vsi_r0 (Source pSource)
	: VSymbolImplementationSink<RSink> (&VSNFTask::loadDucWithRepresentative)
	, m_pSource (pSource)
    {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};

template <class T, class RSource, class RSink> class Vsi_r0_c : public VSymbolImplementationSink<RSink> {
//  Member Type
public:
    typedef RSource (T::*Source) () const;

//  Construction
public:
    Vsi_r0_c (Source pSource)
	: VSymbolImplementationSink<RSink> (&VSNFTask::loadDucWithRepresentative)
	, m_pSource (pSource)
    {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};


/************************
 *****  Procedures  *****
 ************************/

template <class T> class Vsi_p0 : public VSymbolImplementation {
//  Member Type
public:
    typedef void (T::*ProcedureType) ();

//  Construction
public:
    Vsi_p0 (ProcedureType pProcedure) : m_pProcedure (pProcedure) {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    ProcedureType const m_pProcedure;
};

template <class T> class Vsi_p0_c : public VSymbolImplementation {
//  Member Type
public:
    typedef void (T::*ProcedureType) () const;

//  Construction
public:
    Vsi_p0_c (ProcedureType pProcedure) : m_pProcedure (pProcedure) {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    ProcedureType const m_pProcedure;
};


/************************************
 *****  Direct Property Access  *****
 ************************************/

template <class T, class R> class Vsi_property : public VSymbolImplementationSink<R> {
//  Member Type
public:
    typedef R T::*Source;

//  Construction
public:
    Vsi_property (
	Source pSource,
	typename VSymbolImplementationSink<R>::Sink pSink = &VSNFTask::loadDucWith
    ) : VSymbolImplementationSink<R> (pSink), m_pSource (pSource)
    {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};


/***********************************
 *****  Property 'get' Access  *****
 ***********************************/

template <class T, class R> class Vsi_getr_c : public VSymbolImplementationSink<R const&> {
//  Member Type
public:
    typedef void (T::*Source) (R& rR) const;

//  Construction
public:
    Vsi_getr_c (
	Source pSource,
	typename VSymbolImplementationSink<R const&>::Sink pSink = &VSNFTask::loadDucWith
    ) : VSymbolImplementationSink<R const&> (pSink), m_pSource (pSource)
    {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Source const m_pSource;
};


/**************************
 *****  Task Boolean  *****
 **************************/

template <class T> class Vsi_tb : public VSymbolImplementation {
//  Member Type
public:
    typedef bool (T::*Processor) (VSNFTask*);

//  Construction
public:
    Vsi_tb (Processor pProcessor) : m_pProcessor (pProcessor) {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Processor const m_pProcessor;
};

/***************************
 *****  Task Function  *****
 ***************************/

template <class T, class R> class Vsi_tf : public VSymbolImplementationSink<R> {
//  Member Type
public:
    typedef R (T::*Processor) (VSNFTask* pTask);

//  Construction
public:
    Vsi_tf (
	Processor pProcessor,
	typename VSymbolImplementationSink<R>::Sink pSink = &VSNFTask::loadDucWith
    ) : VSymbolImplementationSink<R> (pSink), m_pProcessor (pProcessor)
    {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Processor const m_pProcessor;
};

/***************************************
 *****  Task Procedure (full DUC)  *****
 ***************************************
 *
 *  Full DUC task procedures assume full responsibility for implementing the task,
 *  including setting the DUC.
 *
 *****/

template <class T> class Vsi_tpFullDuc : public VSymbolImplementation {
//  Member Type
public:
    typedef void (T::*Processor) (VSNFTask*);

//  Construction
public:
    Vsi_tpFullDuc (Processor pProcessor) : m_pProcessor (pProcessor) {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Processor const m_pProcessor;
};

/***************************************
 *****  Task Procedure (void DUC)  *****
 ***************************************
 *
 *  Void Duc task procedures assume full responsibility for implementing the task
 *  except for setting the DUC.
 *
 *****/

template <class T> class Vsi_tpVoidDuc : public VSymbolImplementation {
//  Member Type
public:
    typedef void (T::*Processor) (VSNFTask*);

//  Construction
public:
    Vsi_tpVoidDuc (Processor pProcessor) : m_pProcessor (pProcessor) {
    }

//  Task Implementation
protected:
    void implement (VSNFTask* pTask) const;

//  State
protected:
    Processor const m_pProcessor;
};


/*********************
 *****  Members  *****
 *********************/

#if defined(_AIX) && defined(__TEMPINC__)
#pragma implementation("VSymbolImplementation.i")
#else
#include "VSymbolImplementation.i"
#endif


/**************************************
 *****  Constant Implementations  *****
 **************************************/

extern VsiTrue	g_siTrue;
extern VsiFalse	g_siFalse;
extern VsiNA	g_siNA;


#endif
