#ifndef VGroundStore_Interface
#define VGroundStore_Interface

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vdd_Store.h"

class M_CPD;
class VSNFTask;


/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT VGroundStore : public VBenderenceable {
    DECLARE_ABSTRACT_RTT (VGroundStore, VBenderenceable);

    friend class rtVSTORE_Handle;

//  Construction
protected:
    VGroundStore ();

//  Destruction
protected:
    ~VGroundStore ();

//  Access
public:
    virtual unsigned int cardinality_ () const = 0;
    virtual rtPTOKEN_Handle *ptoken_ () const = 0;

//  Query
public:
    virtual bool allowsInsert () const;
    virtual bool allowsDelete () const;

    bool hasASurrogate () const {
	return m_pSurrogate.isntNil ();
    }

//  Surrogate / Transient Extension Management
private:
    void detachSurrogate (rtVSTORE_Handle *pSurrogate);
public:
    void getSurrogate (VReference<rtVSTORE_Handle> &rpResult);

//  Task Implementation

public:
    virtual bool groundImplementationSucceeded (VSNFTask* pTask) = 0;

//  State
private:
    V::VAggregatePointer<rtVSTORE_Handle> m_pSurrogate;
};


#endif
