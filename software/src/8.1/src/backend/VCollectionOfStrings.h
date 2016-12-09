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

#include "RTblock.h"
#include "RTlstore.h"
#include "RTlink.h"

/*************************
 *****  Definitions  *****
 *************************/

class VCollectionOfStrings : public VCollectionOfOrderables {
//  Aliases
public:
    typedef Vdd::Store Store;

//  Construction Helpers
private:
    M_CPD *StringIndexContainer (DSC_Descriptor &rSource);

//  Construction
public:
    VCollectionOfStrings (rtPTOKEN_Handle *pDPT, DSC_Descriptor &rSource);

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
    bool GetElement (unsigned int xElement, char const *&rpResult) const {
	unsigned int xString = m_pStringIndexArray[xElement];
	rpResult = m_pBlockStrings.isSet () ? m_pBlockStrings[xString]
		: m_pLStoreStrings.isSet () ? m_pLStoreStrings[xString]
		: 0;
	return IsntNil (rpResult);
    }

//  Use
public:
    void insertInto (
	Store *pSet, M_CPD *&rpReordering, VAssociativeResult& rAssociativeResult
    );
    void locateIn (
	Store *pSet, M_CPD *&rpReordering, VAssociativeResult& rAssociativeResult
    );
    void deleteFrom (
	Store *pSet, M_CPD *&rpReordering, VAssociativeResult& rAssociativeResult
    );

//  State
protected:
    Vdd::Store::Reference	m_pStringStore;
    rtBLOCK_Handle::Strings	m_pBlockStrings;
    rtLSTORE_Handle::Strings	m_pLStoreStrings;
    VCPDReference const		m_pStringIndexContainer;
    DSC_Scalar			m_iStringIndexReference;
    unsigned int const*		m_pStringIndexArray;
    unsigned int		m_sStringStorageArray;
};


#endif
