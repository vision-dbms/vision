#ifndef VExternalGroundStore_Interface
#define VExternalGroundStore_Interface

/************************
 *****  Components  *****
 ************************/

#include "VGroundStore.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_ICollection.h"

#include "RTptoken.h"


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
private:
    ~VExternalGroundStore ();

//  Access
public:
    virtual unsigned int cardinality_ () const OVERRIDE;

    virtual rtPTOKEN_Handle *ptoken_() const OVERRIDE;

    Vxa::ISingleton *getInterface () const {
	return m_pCollection;
    }

//  Task Implementation
public:
    virtual bool groundImplementationSucceeded (VSNFTask* pTask) OVERRIDE;

//  State
protected:
    Vxa::ISingleton::Reference const m_pCollection;
    rtPTOKEN_Handle::Reference const m_pPToken;
};


#endif
