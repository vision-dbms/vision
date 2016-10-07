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

class VContainerHandle;


/*************************
 *****  Definitions  *****
 *************************/

class M_KnownObjectTable : public VBenderenceable {
    DECLARE_CONCRETE_RTT (M_KnownObjectTable, VBenderenceable);

//  Known Object Table Entry
public:
    class Entry : public VTransient {
    //  Construction
    public:
	Entry () : m_pObjectCPD (0), m_pObjectHandle (0), m_pPTokenCPD (0) {
	}

    //  Destruction
    public:
	~Entry ();

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
	M_CPD *ObjectCPD () const {
	    return m_pObjectCPD;
	}
	M_CPD *PTokenCPD () const {
	    return m_pPTokenCPD;
	}
	operator M_CPD* () const {
	    return m_pObjectCPD;
	}

	VContainerHandle *ObjectHandle () const {
	    return m_pObjectHandle;
	}
	VContainerHandle *PTokenHandle () const {
	    return m_pPTokenHandle;
	}
	operator VContainerHandle* () const {
	    return m_pObjectHandle;
	}

	inline M_CPD *RetainedObjectCPD () const;
	inline M_CPD *RetainedPTokenCPD () const;

    //  Update
    public:
	void RegisterObject (M_KnownObjectTable *pKOT, M_CPD *pObjectCPD);

    //  State
    protected:
	M_CPD			*m_pObjectCPD;
	M_CPD			*m_pPTokenCPD;
	VContainerHandle	*m_pObjectHandle;
	VContainerHandle	*m_pPTokenHandle;
    };

//  Construction
public:
    M_KnownObjectTable (M_AND *pAND);	//  Creation
    M_KnownObjectTable (M_CPD *pKOTC);	//  Access
    void completeInitialization ();

//  Destruction
protected:
    ~M_KnownObjectTable ();

//  Access
public:
    ENVIR_BuiltInObjectPArray *ContainerArray () const;

    M_CPD *ContainerCPD () const {
	return m_pKOTC;
    }

    Entry const &GetBlockEquivalentClassFromPToken (VContainerHandle const *pHandle) const {
	return TheStringClass.PTokenHandle () == pHandle
	    ? TheStringClass : TheSelectorClass;
    }

    inline M_ASD *Space () const;

    M_CPD *TheCharacterPTokenCPD () const {
	return TheCharacterClass.PTokenCPD ();
    }
    M_CPD *TheDatePTokenCPD () const {
	return TheDateClass.PTokenCPD ();
    }
    M_CPD *TheDoublePTokenCPD () const {
	return TheDoubleClass.PTokenCPD ();
    }
    M_CPD *TheFalsePTokenCPD () const {
	return TheFalseClass.PTokenCPD ();
    }
    M_CPD *TheFloatPTokenCPD () const {
	return TheFloatClass.PTokenCPD ();
    }
    M_CPD *TheIntegerPTokenCPD () const {
	return TheIntegerClass.PTokenCPD ();
    }
    M_CPD *TheNAPTokenCPD () const {
	return TheNAClass.PTokenCPD ();
    }
    M_CPD *ThePrimitivePTokenCPD () const {
	return ThePrimitiveClass.PTokenCPD ();
    }
    M_CPD *TheStringPTokenCPD () const {
	return TheStringClass.PTokenCPD ();
    }
    M_CPD *TheTruePTokenCPD () const {
	return TheTrueClass.PTokenCPD ();
    }

    VContainerHandle *TheCharacterPTokenHandle () const {
	return TheCharacterClass.PTokenHandle ();
    }
    VContainerHandle *TheDatePTokenHandle () const {
	return TheDateClass.PTokenHandle ();
    }
    VContainerHandle *TheDoublePTokenHandle () const {
	return TheDoubleClass.PTokenHandle ();
    }
    VContainerHandle *TheFalsePTokenHandle () const {
	return TheFalseClass.PTokenHandle ();
    }
    VContainerHandle *TheFloatPTokenHandle () const {
	return TheFloatClass.PTokenHandle ();
    }
    VContainerHandle *TheIntegerPTokenHandle () const {
	return TheIntegerClass.PTokenHandle ();
    }
    VContainerHandle *TheNAPTokenHandle () const {
	return TheNAClass.PTokenHandle ();
    }
    VContainerHandle *ThePrimitivePTokenHandle () const {
	return ThePrimitiveClass.PTokenHandle ();
    }
    VContainerHandle *TheStringPTokenHandle () const {
	return TheStringClass.PTokenHandle ();
    }
    VContainerHandle *TheTruePTokenHandle () const {
	return TheTrueClass.PTokenHandle ();
    }

//  Query
public:

//  Update
public:
    void ForwardAndLock (M_CPD *pObject) const;

    void RegisterObject (
	M_CPD					*pObjectCPD,
	Entry	M_KnownObjectTable	      ::*pKOTE,
	M_POP	ENVIR_BuiltInObjectPArray     ::*pKOTCE
    );

//  State
protected:
    M_CPD* const m_pKOTC;
public:
    Entry ENVIR_KOTEs (ENVIR_KOTE_Include, ENVIR_AKOE_Exclude);

};

/************************
 *****  'typedef's  *****
 ************************/

typedef M_KnownObjectTable::Entry M_KOTE;

typedef M_KOTE const M_KnownObjectTable::*M_KOTM;

typedef M_KnownObjectTable const M_KOT;


#endif
