#ifndef VFragment_Interface
#define VFragment_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VCPDReference.h"
#include "VDescriptor.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/


/*---------------------------------------------------------------------------
 * A 'fragment' defines a subset of the elements of a 'fragmentation'.  Each
 * fragment contains an extended descriptor specifying the fragment's content
 * and a link constructor specifying the subset of fragmentation positions
 * defined by the fragment.  The fragments of a fragmentation are organized
 * into a singly linked list.
 *
 *  Fragment Field Descriptions:
 *	next			- the address of the next fragment in this list
 *				  of fragments.
 *	subset			- a link constructor specifying the subset of
 *				  fragmentation positions defined by this
 *				  fragment.
 *	content			- an extended descriptor for the content of
 *				  this fragment.
 *---------------------------------------------------------------------------
 */

class VFragment : public VTransient {
    friend class VFragmentation;

//  Descriptor
protected:
    class Descriptor : public VTransient {
	friend class VFragment;

    //  Construction
    public:
	Descriptor () : m_pStoreHandle (0), m_pAssignmentLinkc (0) {
	}

    //  Destruction
    public:
	~Descriptor () {
	    if (m_pAssignmentLinkc)
		m_pAssignmentLinkc->release ();
	}

    //  Initialization
    public:
	void initializeFrom (VFragment* pFragment);

    //  Comparison
    public:
	bool operator <  (Descriptor const& rOther) const {
	    return m_pStoreHandle <  rOther.m_pStoreHandle;
	}
	bool operator <= (Descriptor const& rOther) const {
	    return m_pStoreHandle <= rOther.m_pStoreHandle;
	}
	bool operator == (Descriptor const& rOther) const {
	    return m_pStoreHandle == rOther.m_pStoreHandle;
	}
	bool operator != (Descriptor const& rOther) const {
	    return m_pStoreHandle != rOther.m_pStoreHandle;
	}
	bool operator >= (Descriptor const& rOther) const {
	    return m_pStoreHandle >= rOther.m_pStoreHandle;
	}
	bool operator >  (Descriptor const& rOther) const {
	    return m_pStoreHandle >  rOther.m_pStoreHandle;
	}

    //  Query
    public:
	bool canMergeWith (Descriptor const& rOther) const {
	    return *this == rOther;
	}

	bool getFastMergeData (unsigned int& rCumulativeRRDCount) const;

    //  Fragment Merging
    public:
	void fastMergeBegin (
	    Descriptor** &rppDescriptor, unsigned int* &rpReference, unsigned int* &rpSize
	);

	void fastMergeAppend (unsigned int iOrigin, unsigned int iSize) {
	    rtLINK_AppendRange (m_pAssignmentLinkc, iOrigin, iSize);
	}

	void fastMergeEnd (M_CPD* pAssignmentLinkRPT) {
	    m_pAssignmentLinkc->Close (pAssignmentLinkRPT);
	}

	void buildSubset (rtLINK_CType* pNewSubset);

	M_CPD* createContent (rtLINK_CType* pNewSubset);

	void createFragment (
	    rtLINK_CType* pNewSubset, M_CPD* pNewContent, VFragmentation* pParent
	);
	VFragment* moveContent (M_CPD* pNewContent);

    //  State
    protected:
	VFragment		*m_pFragment;
	VCPDReference		 m_pStoreCPD;
	VContainerHandle	*m_pStoreHandle;
	rtLINK_CType		*m_pAssignmentLinkc;
    };

    static int __cdecl CompareDescriptors (
	Descriptor const *pDescriptor1, Descriptor const *pDescriptor2
    );

//  Construction
public:
    VFragment (VFragment* pNext, rtLINK_CType* pSubset);

//  Destruction
protected:
    ~VFragment ();

    void removeFrom (VFragmentation* pParent);

public:
    void discard ();

//  Access
public:
    VFragment*		next		() const { return m_pNext; }
    VFragment*		previous	() const { return m_pPrevious; }

    rtLINK_CType*	subset		() const { return m_pSubset; }
    VDescriptor&	datum		()	 { return m_iDatum; }

//  Update
protected:
    void setSubsetTo (rtLINK_CType* pSubset);

//  Coalesce
protected:
    bool IsACoalesceCandidate () const {
	return m_iDatum.isStandard () && IsntNil (
	    m_iDatum.contentAsMonotype ().storeCPDIfAvailable ()
	);
    }

    void initializeDescriptor (Descriptor& rDescriptor) {
	rDescriptor.initializeFrom (this);
    }

//  Description
public:
    void describe (unsigned int xLevel);

//  State
protected:
    VFragment*		m_pNext;
    VFragment*		m_pPrevious;
    rtLINK_CType*	m_pSubset;
    VDescriptor		m_iDatum;
};


#endif
