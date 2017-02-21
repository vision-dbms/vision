#ifndef VCollectionOfStrings_Interface
#define VCollectionOfStrings_Interface

/************************
 *****  Components  *****
 ************************/

#include "VCollectionOfOrderables.h"

#include "VCPDReference.h"

#include "vdsc.h"

/**************************
 *****  Declarations  *****
 **************************/

class VAssociativeResult;

#include "RTlink.h"

/*************************
 *****  Definitions  *****
 *************************/

class VCollectionOfStrings : public VCollectionOfOrderables {
//  Construction Helpers
private:
    M_CPD* StringOriginsContainer (DSC_Descriptor& rSource);
    M_CPD* StringStorageContainer (DSC_Descriptor& rSource);

//  Construction
public:
    VCollectionOfStrings (M_CPD* pDPT, DSC_Descriptor& rSource);

//  Destruction
public:
    ~VCollectionOfStrings ();

//  Cache Maintenance
public:
    void refreshCache ();

//  Element Comparison
public:
    int compare (unsigned int xElement1, unsigned int xElement2) const;

//  Element Access
public:
    bool GetElement (unsigned int xElement, char const*& rElement) const {
	unsigned int xStringOrigin = m_pStringOriginsArray[xElement];
	bool fElementIsValid = xStringOrigin < m_sStringStorageArray;
	if (fElementIsValid)
	    rElement = m_pStringStorageArray + xStringOrigin;
	return fElementIsValid;
    }

//  Use
public:
    void insertInto (
	M_CPD* pSet, M_CPD*& rpReordering, VAssociativeResult& rAssociativeResult
    );
    void locateIn (
	M_CPD* pSet, M_CPD*& rpReordering, VAssociativeResult& rAssociativeResult
    );
    void deleteFrom (
	M_CPD* pSet, M_CPD*& rpReordering, VAssociativeResult& rAssociativeResult
    );

//  State
protected:
    VCPDReference const	m_pStringStore;
    VCPDReference const	m_pStringOriginsContainer;
    VCPDReference const	m_pStringStorageContainer;
    DSC_Scalar		m_iStringOriginsReference;
    unsigned int const*	m_pStringOriginsArray;
    unsigned int	m_sStringStorageArray;
    char const*		m_pStringStorageArray;
};


#endif
