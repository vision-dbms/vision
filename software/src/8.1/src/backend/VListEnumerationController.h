#ifndef VListEnumerationController_Interface
#define VListEnumerationController_Interface

/************************
 *****  Components  *****
 ************************/

#include "VPrimitiveTaskController.h"

#include "VDescriptor.h"
#include "VLinkCReference.h"
#include "VCPDReference.h"

/**************************
 *****  Declarations  *****
 **************************/

class rtVECTOR_CType;


/*******************************
 *****  Primitive Indices  *****
 *******************************/

#define XIterate			179
#define XIIC				180
#define XIICForValue			181
#define XIICForEnvironment		182
#define XIICForSubset			183
#define XIICForGrouping			184
#define XIICForAscendingSort		185
#define XIICForDescendingSort		186

#define XIICForNumericTotal		190
#define XIICForNumericAverage		191
#define XIICForNumericMinimum		192
#define XIICForNumericMaximum		193
#define XIICForNumericCount		194
#define XIICForAscendingRank		195
#define XIICForDescendingRank		196
#define XIICForNumericProduct		197
#define XIICForRunningTotal		198


/*************************
 *****  Definitions  *****
 *************************/

class VListEnumerationController : public VPrimitiveTaskController<VListEnumerationController> {
//  Run Time Type
    DECLARE_CONCRETE_RTT (
	VListEnumerationController,VPrimitiveTaskController<VListEnumerationController>
    );

//  Construction
public:
    VListEnumerationController (
	ConstructionData const&	rTCData,
	VPrimitiveDescriptor*	pDescriptor,
	unsigned short		iFlags
    );

//  Destruction
protected:
    ~VListEnumerationController ();

//  Continuations
protected:
    void startup	();
    void returnSelf	();
    void returnValue	();
    void returnSubset	();
    void returnSummary	();
    void returnGroup	();
    void returnSort	();
    void returnOrder	();

//  Utilities
protected:
    rtVECTOR_CType *convertDucToVectorC () {
	normalizeDuc ();
	return VPrimitiveTaskController<VListEnumerationController>::convertDucToVectorC ();
    }

    void loadDucWithInitializedLists (
	rtLINK_CType *pDefinitionLink, bool fLStoreDesired
    );

    void loadDucWithInitializedLStore (rtLINK_CType *pDefinitionLink) {
	loadDucWithInitializedLists (pDefinitionLink, true);
    }
    void loadDucWithInitializedLStoreOrIndex (rtLINK_CType *pDefinitionLink) {
	loadDucWithInitializedLists (pDefinitionLink, false);
    }

    void loadDucWithNewIndex (Vdd::Store *pContentPrototype = 0);

    void loadDucWithSelectedElements (unsigned int iterationFactor = 0);

//  State
protected:
    VDescriptor		m_iCollector;
    VDescriptor		m_iSelectedElements;
    VLinkCReference	m_pExpansionLink;
    VCPDReference	m_pExpansionReordering;
    rtINDEX_Key*	m_pTemporalContext;
};


#endif
