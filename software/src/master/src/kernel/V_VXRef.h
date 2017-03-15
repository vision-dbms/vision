#ifndef V_VXRef_Interface
#define V_VXRef_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VkMapOf.h"
#include "VkSetOf.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VComponentReference.h"

#include "V_VReferenceable_.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {

    /*******************
     *----  VXRef  ----*
     *******************/
    template<
	typename S1, typename V1, typename K1,
	typename S2, typename V2, typename K2
    > class VXRef : public virtual VTransient {
	typedef VXRef<S1,V1,K1,S2,V2,K2> ThisClassTypedef;
	DECLARE_FAMILY_MEMBERS (ThisClassTypedef, VTransient);

    //  Set Types
    public:
	typedef VkSetOf<S1,V1,K1> SetOf1s;
	typedef VkSetOf<S2,V2,K2> SetOf2s;

	typedef VComponentReference<const SetOf1s> SetOf1sQueryResult;
	typedef VComponentReference<const SetOf2s> SetOf2sQueryResult;

	typedef VReferenceable_<VkSetOf<S1,V1,K1> > ReferenceableSetOf1s;
	typedef VReferenceable_<VkSetOf<S2,V2,K2> > ReferenceableSetOf2s;

    //  Map Types
    public:
	typedef VkMapOf<S1,V1,K1,VReference<ReferenceableSetOf2s> > MapOf1s;
	typedef VkMapOf<S2,V2,K2,VReference<ReferenceableSetOf1s> > MapOf2s;

    //  Construction
    public:
	VXRef () {
	}

    //  Destruction
    public:
	~VXRef () {
	}

    //  Access/Query
    public:
	bool containsK1 (K1 iKey) const {
	    return m_iMapOf1s.Contains (iKey) ? true : false;
	}
	bool containsK2 (K2 iKey) const {
	    return m_iMapOf2s.Contains (iKey) ? true : false;
	}

	bool locateK1 (K1 iKey, SetOf2sQueryResult &rResult) const {
	    unsigned int xKey;
	    if (!m_iMapOf1s.Locate (iKey, xKey))
		return false;
	    
	    ReferenceableSetOf2s *pResult = m_iMapOf1s.value (xKey);
	    rResult.setTo (pResult, pResult);

	    return true;
	}
	bool locateK2 (K2 iKey, SetOf1sQueryResult &rResult) const {
	    unsigned int xKey;
	    if (!m_iMapOf2s.Locate (iKey, xKey))
		return false;

	    ReferenceableSetOf1s *pResult = m_iMapOf2s.value (xKey);
	    rResult.setTo (pResult, pResult);

	    return true;
	}

	//  The following two routines add the map keys to result set...
	void getSetOf1s (SetOf1s &rResult) const {
	    for (unsigned int xItem = 0; xItem < m_iMapOf1s.cardinality (); xItem++)
		rResult.Insert (m_iMapOf1s.association(xItem).key ());
	}
	void getSetOf2s (SetOf2s &rResult) const {
	    for (unsigned int xItem = 0; xItem < m_iMapOf1s.cardinality (); xItem++)
		rResult.Insert (m_iMapOf2s.association(xItem).key ());
	}

    //  Insertion
    private:
	bool insertXX (unsigned int x1, unsigned int x2) {
	    bool result = false;
	    if (m_iMapOf1s.value (x1)->Insert (m_iMapOf2s.key (x2)))
		result = true;
	    if (m_iMapOf2s.value (x2)->Insert (m_iMapOf1s.key (x1)))
		result = true;
	    return result;
	}

	unsigned int locateOrAddV1 (V1 iV) {
	    unsigned int xV;
	    if (!m_iMapOf1s.Locate (iV, xV))
		m_iMapOf1s.value (xV).setTo (new ReferenceableSetOf2s ());
	    return xV;
	}
	unsigned int locateOrAddV2 (V2 iV) {
	    unsigned int xV;
	    if (!m_iMapOf2s.Locate (iV, xV))
		m_iMapOf2s.value (xV).setTo (new ReferenceableSetOf1s ());
	    return xV;
	}

	bool insertXK (unsigned int x1, V2 i2) {
	    return insertXX (x1, locateOrAddV2 (i2));
	}
	bool insertKX (V1 i1, unsigned int x2) {
	    return insertXX (locateOrAddV1 (i1), x2);
	}

	bool insertXS (unsigned int x1, SetOf2s const &rSet) {
	    bool result = false;
	    for (unsigned int xSet = 0; xSet < rSet.cardinality (); xSet++)
		if (insertXX (x1, locateOrAddV2 (rSet[xSet])))
		    result = true;
	    return result;
	}
	bool insertSX (SetOf1s const &rSet, unsigned int x2) {
	    bool result = false;
	    for (unsigned int xSet = 0; xSet < rSet.cardinality (); xSet++)
		if (insertXX (locateOrAddV1 (rSet[xSet]), x2))
		    result = true;
	    return result;
	}

    public:
	bool insertKK (V1 i1, V2 i2) {
	    return insertXX (locateOrAddV1 (i1), locateOrAddV2 (i2));
	}

	bool insertKS (V1 iValue, SetOf2s const &rSet) {
	    return insertXS (locateOrAddV1 (iValue), rSet);
	}
	bool insertSK (SetOf1s const &rSet, V2 iValue) {
	    return insertSX (rSet, locateOrAddV2 (iValue));
	}

    //  Deletion
    private:
	bool removeXX (unsigned int x1, unsigned int x2) {
	    S1 iKey1 = m_iMapOf1s.key (x1);
	    S2 iKey2 = m_iMapOf2s.key (x2);

	    SetOf2s *pSet1 = m_iMapOf1s.value (x1);
	    SetOf1s *pSet2 = m_iMapOf2s.value (x2);

	    bool result = false;
	    if (pSet1->Delete (iKey2))
		result = true;
	    if (pSet2->Delete (iKey1))
		result = true;

	    if (pSet1->cardinality () == 0 && m_iMapOf1s.Delete (iKey1))
		result = true;
	    if (pSet2->cardinality () == 0 && m_iMapOf2s.Delete (iKey2))
		result = true;

	    return result;
	}

	bool removeXS (unsigned int x1, SetOf2s const &rSet) {
	    bool result = false;

	    //  Protect the internal set from deletion while we're using it:
	    VReference<ReferenceableSetOf2s const> pSet (m_iMapOf1s.value (x1));

	/*====================================================================
	 *  Iterate over the internal set so that it can be simultaneously
	 *  used as the set argument to this routine (see 'removeK1').
	 *===================================================================*/
	    unsigned int xSet = 0;
	    while (xSet < pSet->cardinality ()) {
		if (!rSet.Contains ((*pSet)[xSet]))
		    xSet++;
		else {
		    unsigned int x2;
		    if (m_iMapOf2s.Locate ((*pSet)[xSet], x2) && removeXX (x1, x2))
			result = true;
		    else
			xSet++;
		}
	    }

	    return result;
	}
	bool removeSX (SetOf1s const &rSet, unsigned int x2) {
	    bool result = false;

	    //  Protect the internal set from deletion while we're using it:
	    VReference<ReferenceableSetOf1s const> pSet (m_iMapOf2s.value (x2));

	/*====================================================================
	 *  Iterate over the internal set so that it can be simultaneously
	 *  used as the set argument to this routine (see 'removeK2').
	 *===================================================================*/
	    unsigned int xSet = 0;
	    while (xSet < pSet->cardinality ()) {
		if (!rSet.Contains ((*pSet)[xSet]))
		    xSet++;
		else {
		    unsigned int x1;
		    if (m_iMapOf1s.Locate ((*pSet)[xSet], x1) && removeXX (x1, x2))
			result = true;
		    else
			xSet++;
		}
	    }

	    return result;
	}

    public:
	bool removeKK (K1 i1, K2 i2) {
	    unsigned int x1, x2;
	    return m_iMapOf1s.Locate (i1, x1) && m_iMapOf2s.Locate (i2, x2) && removeXX (x1, x2);
	}

    public:
	bool removeKS (K1 iKey, SetOf2s const &rSet) {
	    unsigned int xKey;
	    return m_iMapOf1s.Locate (iKey, xKey) && removeXS (xKey, rSet);
	}
	bool removeSK (SetOf1s const &rSet, K2 iKey) {
	    unsigned int xKey;
	    return m_iMapOf2s.Locate (iKey, xKey) && removeSX (rSet, xKey);
	}
	bool removeK1 (K1 iKey) {
	    unsigned int xKey;
	    return m_iMapOf1s.Locate (iKey, xKey) && removeXS (xKey, *m_iMapOf1s.value(xKey));
	}
	bool removeK2 (K2 iKey) {
	    unsigned int xKey;
	    return m_iMapOf2s.Locate (iKey, xKey) && removeSX (*m_iMapOf2s.value(xKey), xKey);
	}

    //  State
    private:
	MapOf1s m_iMapOf1s;
	MapOf2s m_iMapOf2s;
    };
}


#endif
