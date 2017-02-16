/*****  VReferenceableMonotype Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VReferenceableMonotype.h"

/************************
 *****  Supporting  *****
 ************************/

#include "m.h"


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VReferenceableMonotype);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VReferenceableMonotype::VReferenceableMonotype (
    M_CPD *pF2, rtLINK_CType *pF1, DSC_Descriptor &rSource
) {
    DSC_Descriptor iTmpMonotype;
    iTmpMonotype.constructComposition (pF1, rSource);
    m_iMonotype.constructComposition (pF2, iTmpMonotype);
    iTmpMonotype.clear ();
}

VReferenceableMonotype::VReferenceableMonotype (rtLINK_CType *pF1, DSC_Descriptor &rSource) {
    m_iMonotype.constructComposition (pF1, rSource);
}

VReferenceableMonotype::VReferenceableMonotype (M_CPD *pF1, DSC_Descriptor &rSource) {
    m_iMonotype.constructComposition (pF1, rSource);
}

VReferenceableMonotype::VReferenceableMonotype (DSC_Descriptor const& rSource) {
    m_iMonotype.construct (rSource);
}

VReferenceableMonotype::VReferenceableMonotype () {
    m_iMonotype.construct ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VReferenceableMonotype::~VReferenceableMonotype () {
    m_iMonotype.clear ();
}
