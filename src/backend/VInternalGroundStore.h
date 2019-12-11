#ifndef VInternalGroundStore_Interface
#define VInternalGroundStore_Interface

/************************
 *****  Components  *****
 ************************/

#include "VGroundStore.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "M_CPD.h"

class VSymbol;
class VSymbolImplementation;
class VSymbolSpace;

class VSNFTask;


/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT VInternalGroundStore : public VGroundStore {
//  Run Time Type
    DECLARE_ABSTRACT_RTT (VInternalGroundStore, VGroundStore);

//  Cluster Symbol Space
protected:
    static VSymbolSpace g_iClusterSymbolSpace;
    static VSymbol *CSym (char const *pName);

public:
    static void DisplayCSymBindings ();

//  Meta Maker
private:
    unsigned int referenceCountMetaMakerWorkaround () const {
	return referenceCount ();
    }
protected:
    static void MetaMaker ();

//  UID Generator
private:
    static unsigned int g_xUID32;

//  Construction
protected:
    VInternalGroundStore () : m_xUID32 (0) {
    }


//  Destruction
protected:
    ~VInternalGroundStore () {
    }

//  Access
public:

    unsigned int tuid32 () {
	return m_xUID32 ? m_xUID32 : (m_xUID32 = ++g_xUID32);
    }

//  Task Implementation
protected:
    virtual VSymbolImplementation const *implementationOf (char const *pName) const;

public:
    virtual bool groundImplementationSucceeded (VSNFTask* pTask) OVERRIDE;

//  State
private:
    unsigned int	m_xUID32;
};


#endif
