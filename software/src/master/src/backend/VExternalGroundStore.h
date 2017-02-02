#ifndef VExternalGroundStore_Interface
#define VExternalGroundStore_Interface

/************************
 *****  Components  *****
 ************************/

#include "VGroundStore.h"

#include "VCPDReference.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_ICollection.h"


/*************************
 *****  Definitions  *****
 *************************/

class VExternalGroundStore : public VGroundStore {
    DECLARE_CONCRETE_RTT (VExternalGroundStore, VGroundStore);

//  Construction
public:
    VExternalGroundStore (M_ASD *pSpace, Vxa::ICollection *pCollection, unsigned int sCluster);
    VExternalGroundStore (Vxa::ISingleton *pCollection);

//  Destruction
protected:
    ~VExternalGroundStore ();

//  Access
public:
    unsigned int cardinality_ () const;

    M_CPD *ptoken_() const;

    Vxa::ISingleton *getInterface () const {
	return m_pCollection;
    }

//  Task Implementation
public:
    bool groundImplementationSucceeded (VSNFTask* pTask);

//  State
protected:
    Vxa::ISingleton::Reference const m_pCollection;
    VCPDReference const m_pPToken;
};


#endif
