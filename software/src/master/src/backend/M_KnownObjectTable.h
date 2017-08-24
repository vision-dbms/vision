#ifndef M_KnownObjectTable_Interface
#define M_KnownObjectTable_Interface

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "venvir.d"

class M_AND;
class M_ASD;
class M_CPD;

class rtPTOKEN_Handle;

class VContainerHandle;

namespace Vdd {
    class Store;
}


/*************************
 *****  Definitions  *****
 *************************/

/********************
 *----  M_KOTE  ----*
 ********************/

class M_KOT;

class M_KOTE : public VTransient {
    DECLARE_FAMILY_MEMBERS (M_KOTE, VTransient);

//  Aliases
public:
    typedef Vdd::Store Store;

//  Construction
public:
    M_KOTE ();

//  Destruction
public:
    ~M_KOTE ();

//  Query
public:
    bool IsSet () const {
	return IsntNil (m_pObjectCPD);
    }
    bool IsntSet () const {
	return IsNil (m_pObjectCPD);
    }

//  Access
public:
    operator Store* () const {
	return store ();
    }
    Store *store () const {
	return m_pStore;
    }

    operator M_CPD* () const {
	return m_pObjectCPD;
    }
    operator VContainerHandle* () const {
	return m_pObjectHandle;
    }
    M_CPD *ObjectCPD () const {
	return m_pObjectCPD;
    }
    VContainerHandle *ObjectHandle () const {
	return m_pObjectHandle;
    }

    rtPTOKEN_Handle *PTokenHandle () const {
	return m_pPTokenHandle;
    }

//  Update
public:
    void RegisterObject (M_KOT *pKOT, M_CPD *pObjectCPD);

//  State
protected:
    M_CPD*			m_pObjectCPD;
    VContainerHandle*		m_pObjectHandle;
    VReference<rtPTOKEN_Handle>	m_pPTokenHandle;
    VReference<Store>		m_pStore;
};


/*******************
 *----  M_KOT  ----*
 *******************/

class M_KOT : public VBenderenceable {
    DECLARE_CONCRETE_RTT (M_KOT, VBenderenceable);

//  Aliases
public:
    typedef Vdd::Store Store;

//  Construction
public:
    M_KOT (M_AND *pAND);	//  Creation
    M_KOT (M_CPD *pKOTC);	//  Access
    void completeInitialization ();

//  Destruction
protected:
    ~M_KOT ();

//  Access
public:
    ENVIR_BuiltInObjectPArray *ContainerArray () const;

    M_CPD *ContainerCPD () const {
	return m_pKOTC;
    }

    M_KOTE const &GetBlockEquivalentClassFromPToken (rtPTOKEN_Handle const *pHandle) const {
	return TheStringClass.PTokenHandle () == pHandle ? TheStringClass : TheSelectorClass;
    }

    inline M_ASD *Space () const;

    rtPTOKEN_Handle *TheCharacterPTokenHandle () const {
	return TheCharacterClass.PTokenHandle ();
    }
    rtPTOKEN_Handle *TheDatePTokenHandle () const {
	return TheDateClass.PTokenHandle ();
    }
    rtPTOKEN_Handle *TheDoublePTokenHandle () const {
	return TheDoubleClass.PTokenHandle ();
    }
    rtPTOKEN_Handle *TheFalsePTokenHandle () const {
	return TheFalseClass.PTokenHandle ();
    }
    rtPTOKEN_Handle *TheFloatPTokenHandle () const {
	return TheFloatClass.PTokenHandle ();
    }
    rtPTOKEN_Handle *TheIntegerPTokenHandle () const {
	return TheIntegerClass.PTokenHandle ();
    }
    rtPTOKEN_Handle *TheNAPTokenHandle () const {
	return TheNAClass.PTokenHandle ();
    }
    rtPTOKEN_Handle *ThePrimitivePTokenHandle () const {
	return ThePrimitiveClass.PTokenHandle ();
    }
    rtPTOKEN_Handle *TheStringPTokenHandle () const {
	return TheStringClass.PTokenHandle ();
    }
    rtPTOKEN_Handle *TheTruePTokenHandle () const {
	return TheTrueClass.PTokenHandle ();
    }

    rtPTOKEN_Handle *TheScalarPTokenHandle () const {
	return (rtPTOKEN_Handle*)TheScalarPToken.ObjectHandle ();
    }
    rtPTOKEN_Handle *TheSelectorPTokenHandle () const {
	return (rtPTOKEN_Handle*)TheSelectorPToken.ObjectHandle ();
    }

//  Query
public:

//  Update
public:
    void ForwardAndLock (M_CPD *pObject) const;

    void RegisterObject (
	M_CPD *pObject, M_KOTE M_KOT::*pKOTE, M_POP ENVIR_BuiltInObjectPArray::*pKOTCE
    );
    void RegisterObject (
	VContainerHandle *pObject, M_KOTE M_KOT::*pKOTE, M_POP ENVIR_BuiltInObjectPArray::*pKOTCE
    );

//  State
protected:
    M_CPD* const m_pKOTC;
public:
    M_KOTE ENVIR_KOTEs (ENVIR_KOTE_Include, ENVIR_AKOE_Exclude);

};

/************************
 *****  'typedef's  *****
 ************************/

typedef M_KOTE const M_KOT::*M_KOTM;


#endif
