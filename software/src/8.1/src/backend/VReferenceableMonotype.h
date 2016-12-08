#ifndef VReferenceableMonotype_Interface
#define VReferenceableMonotype_Interface

/************************
 *****  Components  *****
 ************************/

#include "VBenderenceable.h"
#include "vdsc.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

class VReferenceableMonotype : public VBenderenceable {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VReferenceableMonotype, VBenderenceable);

//  Construction
public:
    VReferenceableMonotype (
	M_CPD *pF2, rtLINK_CType *pF1, DSC_Descriptor &rSource
    );
    VReferenceableMonotype (rtLINK_CType *pF1, DSC_Descriptor &rSource);
    VReferenceableMonotype (M_CPD *pF1, DSC_Descriptor &rSource);
    VReferenceableMonotype (DSC_Descriptor const &rSource);
    VReferenceableMonotype ();

//  Destruction
protected:
    ~VReferenceableMonotype ();

//  Access
public:
    operator DSC_Descriptor& () {
	return m_iMonotype;
    }
    DSC_Descriptor& monotype () {
	return m_iMonotype;
    }

//  State
protected:
    DSC_Descriptor m_iMonotype;
};


#endif
