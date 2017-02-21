#ifndef VBenderenceable_Interface
#define VBenderenceable_Interface

/***************************************************************************
 *----  a.k.a., VBackENDrefERENCEABLE, pronounced v-bend-er-ence-able  ----*
 ***************************************************************************/

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

class VContainerHandle;

/*************************
 *****  Definitions  *****
 *************************/

/*****************************
 *----  VBenderenceable  ----*
 *****************************/

class ABSTRACT VBenderenceable : public VReferenceableNIL {
    DECLARE_ABSTRACT_RTT (VBenderenceable, VReferenceableNIL);

    friend class VContainerHandle;

//  Construction
protected:
    VBenderenceable (unsigned int iInitialRefcount) : BaseClass (iInitialRefcount) {
    }
    VBenderenceable () {
    }

//  Destruction
protected:
    ~VBenderenceable () {
    }

//  Transient Extension Support
public:
/*------------------------------------------------------------------------------*
 *  Any benderenceable can play the role of a container's transient extension.	*
 *  By convention, container handles reference transient extensions.  If	*
 *  transient extensions also referenced containers, a self-reference loop	*
 *  would result.  Nevertheless, transient extensions may need to keep track	*
 *  of the containers they extend.  Those that do should override the		*
 *  'endRoleAsTransientExtensionOf' virtual function to flush their container	*
 *  specific state.  This function will be called immediately before the	*
 *  transient extension is abandoned by the container handle.			*
 *------------------------------------------------------------------------------*/
    virtual void EndRoleAsTransientExtensionOf (VContainerHandle *pContainerHandle);

/*------------------------------------------------------------------------------*
 *  Caching reusable resources is a major application of transient extensions;	*
 *  however, speculatively cached resources must be freed when overall resource	*
 *  limits are encountered.  All transient extension classes used to cache	*
 *  speculatively reuseable resources should override the following virtual	*
 *  member.
 *------------------------------------------------------------------------------*/
    virtual void FlushCacheAsTransientExtensionOf (VContainerHandle *pContainerHandle);
};

/**************************
 *----  transientx_t  ----*
 **************************/

typedef VBenderenceable transientx_t;


#endif
