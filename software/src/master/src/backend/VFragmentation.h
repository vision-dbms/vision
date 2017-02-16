#ifndef VFragmentation_Interface
#define VFragmentation_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "m.h"

#include "RTlink.h"
#include "RTvector.h"

class VFragment;

/*************************
 *****  Definitions  *****
 *************************/


/*---------------------------------------------------------------------------
 * Fragmentations are used to represent the polymorphic collection of objects
 * created by message dispatch and function evaluation.  A fragmentation
 * consists of a P-Token defining the positional state of the collection and a
 * list of 'fragments' defining the elements of the collection as a set of
 * mutually exclusive, collectively exhaustive subsets.
 *
 *  Fragmentation Field Descriptions:
 *	m_pPPT			- a handle for a P-Token defining the
 *				  positional state of the fragmentation.
 *	m_pFragmentList		- the head of a list of fragments defining
 *				  the elements of the fragmentation's object
 *				  collection.
 *	m_pCurrentFragment	- a pointer to a specific fragment.  Used by
 *				  the message dispatcher to point to the
 *				  fragment currently being generated or
 *				  processed.
 *---------------------------------------------------------------------------
 */
class VFragmentation {
//  Friends
    friend class VFragment;

//  Counters, Parameters, and Switches
public:
    static unsigned int CoalesceAttemptCount;
    static unsigned int CoalesceSuccessCount;
    static unsigned int CoalesceNonDJUCount;
    static unsigned int CoalesceExamineCount;
    static unsigned int CoalesceCandidateCount;
    static unsigned int CoalesceReductionCount;
    static unsigned int FlattenFragCount;
    static unsigned int MakeFragFromDscCount;
    static unsigned int MakeFragFromVCCount;
    static unsigned int MakeVectorFromFragCount;

    static bool TracingFragmentationOps;

//  Construction
protected:
    rtPTOKEN_Handle *NewPToken (unsigned int nElements) const {
	return new rtPTOKEN_Handle (m_pPPT->Space (), nElements);
    }

public:
    /*---------------------------------------------------------------------------*
     *	pPToken	    		- a standard CPD for the P-Token defining the
     *				  positional state of the fragmentation.  The
     *				  CPD's retention count will be incremented.
     *---------------------------------------------------------------------------*/
    void construct (rtPTOKEN_Handle *pPToken) {
	pPToken->retain ();
	m_pPPT		= pPToken;
	m_pFragmentList		=
	m_pCurrentFragment	= NilOf (VFragment*);
	m_iFragmentCount	=
	m_xCurrentFragment	= 0;
	m_fLock			= false;
    }
    void construct (VFragmentation const& rSource);

    void construct (rtVECTOR_CType* pVector);
    void construct (DSC_Descriptor& rSource, unsigned int iFragmentSize);

//  Query
public:
    rtPTOKEN_Handle*	ptoken			() const { return m_pPPT; }
    VFragment*		fragmentList		() const { return m_pFragmentList;}
    unsigned int	fragmentCount		() const { return m_iFragmentCount; }
    VFragment*		currentFragment		() const { return m_pCurrentFragment; }
    unsigned int	currentFragmentIndex	() const { return m_xCurrentFragment; }
    bool		fragmentationIsLocked	() const { return m_fLock; }

//  Access
public:
    void assignTo (rtLINK_CType *pElementSelector, Vdd::Store *pTarget);

    void getVector (rtVECTOR_Handle::Reference &rpResult);

    void goToFirstFragment () {
	m_pCurrentFragment = m_pFragmentList;
	m_xCurrentFragment = 0;
    }
    void goToNextFragment ();

    rtLINK_CType *subsetInStore (Vdd::Store *pStore, VDescriptor *pValueReturn);
    rtLINK_CType *subsetInStore (Vdd::Store *pStore) {
	return subsetInStore (pStore, 0);
    }

    rtLINK_CType *subsetOfType (
	M_ASD *pSubsetSpace, M_KOTM pKOTM, VDescriptor *ValueReturn
    );
    rtLINK_CType *subsetOfType (M_ASD *pSubsetSpace, M_KOTM pKOTM) {
	return subsetOfType (pSubsetSpace, pKOTM, 0);
    }

//  Conversion
public:
    void Coalesce ();
    void Flatten ();

//  Update
private:
    void setPPT (rtPTOKEN_Handle *pNewPPT) {
	pNewPPT->retain ();
	m_pPPT->release ();
	m_pPPT = pNewPPT;
    }
public:
    void clear ();

    VFragment *createFragment (rtLINK_CType *pSubset);

    void distribute (M_CPD *pDistribution);

//  Description
public:
    void describe (unsigned int xLevel);

//  Exception Generation
protected:
    void raiseIncorrectFragmentCountException ();

//  State
protected:
    rtPTOKEN_Handle*		m_pPPT;
    VFragment*			m_pFragmentList;
    unsigned int		m_iFragmentCount;
    VFragment*			m_pCurrentFragment;
    unsigned int		m_xCurrentFragment;
    bool			m_fLock;
};


#endif
