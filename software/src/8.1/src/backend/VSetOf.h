#ifndef VSetOf_Interface
#define VSetOf_Interface

/************************
 *****  Components  *****
 ************************/

#include "VSet.h"
#include "VArrayOf.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VAssociativeCursor.h"
#include "VCollectionOf.h"

/*************************
 *****  Definitions  *****
 *************************/

template <class ELType, class EVType> class VSetOf : public VSet {
//  Friends
    friend class VAssociativeCursor<VSetOf<ELType, EVType>, VCollectionOf<EVType>, EVType>;

//  Construction
public:
    VSetOf () {
    }

//  Destruction
public:
    ~VSetOf () {
    }

//  Deferred Initialization
protected:
    void initializeDPT ();

//  Alignment
public:
    void align ();

//  Access
public:
    ELType const& operator[] (unsigned int xElement) const {
	return m_iElementArray[xElement];
    }

    unsigned int cardinality () const {
	return m_iElementArray.cardinality ();
    }

    int compare (unsigned int xElement, EVType iKey) const;

    void Locate (
	VCollectionOf<EVType>* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );
    bool Locate (EVType iKey, unsigned int& rxElement);

//  Update
public:
    void Insert (
	VCollectionOf<EVType>* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );
    void Delete (
	VCollectionOf<EVType>* pKeys, M_CPD*& rpReordering, VAssociativeResult& rResult
    );

    bool Insert (EVType iKey, unsigned int& rxElement);
    bool Delete (EVType iKey);

protected:
    void install (unsigned int xElement, EVType iKey) {
	m_iElementArray[xElement] = iKey;
    }

    void install (
	rtLINK_CType* pAdditions, VOrdered<VCollectionOf<EVType>,EVType> &rAdditions
    );

//  State
protected:
    VCPDReference	m_pElementArrayDPT;
    VArrayOf<ELType>	m_iElementArray;
};


/*********************
 *****  Members  *****
 *********************/

#if defined(_AIX) && defined(__TEMPINC__)
#pragma implementation("VSetOf.i")
#else
#include "VSetOf.i"
#endif


#endif
