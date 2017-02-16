/*****  List Index Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtINDEX

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "popvector.h"

#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTptoken.h"
#include "RTlink.h"

#include "RTlstore.h"
#include "RTvector.h"
#include "RTvstore.h"

#include "RTcharuv.h"
#include "RTdoubleuv.h"
#include "RTfloatuv.h"
#include "RTintuv.h"
#include "RTrefuv.h"
#include "RTundefuv.h"
#include "RTu64uv.h"
#include "RTu96uv.h"
#include "RTu128uv.h"

/*****  Self  *****/
#include "RTindex.h"


/******************************
 ******************************
 *****                    *****
 *****  Container Handle  *****
 *****                    *****
 ******************************
 ******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (rtINDEX_Handle);


/**************************
 *****  Global State  *****
 **************************/

PrivateVarDef bool	    rtINDEX_KeyMapAutoConversionEnabled	= true;
PrivateVarDef RTYPE_Type    rtINDEX_KeyMapDefaultType		= RTYPE_C_RefUV;


/****************************
 *****  Usage Counters  *****
 ****************************/

PrivateVarDef int
    NewKeyCount			= 0,
    NewFutureKeyCount		= 0,
    RealizeKeySequenceCount	= 0,
    RealizeKeySetCount		= 0,

    NewCount			= 0,
    NewTimeSeriesCount		= 0,

    DefineMKeyMListCount	= 0,
    DefineSKeyMListCount	= 0,
    DefineSKeySListCount	= 0,

    LookupMKeyMListCount	= 0,
    LookupSKeyMListCount	= 0,
    LookupSKeySListCount	= 0,

    LookupMListLimitCount	= 0,
    LookupSListLimitCount	= 0,

    DeleteMListKeysCount	= 0,
    DeleteSListKeyCount		= 0,

    ExtractCount		= 0;


/*********************************************
 *********************************************
 *****  Default Key Map Type Management  *****
 *********************************************
 *********************************************/

PublicFnDef void rtINDEX_GetDefaultKeyMapType (
    RTYPE_Type &xKeyMapType, bool &fKeyMapAutoConversionEnabled
) {
    xKeyMapType			= rtINDEX_KeyMapDefaultType;
    fKeyMapAutoConversionEnabled= rtINDEX_KeyMapAutoConversionEnabled;
}

PublicFnDef void rtINDEX_SetDefaultKeyMapType (
    RTYPE_Type xKeyMapType, bool fKeyMapAutoConversionEnabled
) {
    switch (xKeyMapType) {
    case RTYPE_C_Link:
    case RTYPE_C_RefUV:
	rtINDEX_KeyMapDefaultType = xKeyMapType;
	break;
    default:
	break;
    }
    rtINDEX_KeyMapAutoConversionEnabled = fKeyMapAutoConversionEnabled;
}


/*************************
 *************************
 *****  Key Manager  *****
 *************************
 *************************/

/***************************
 *****  Run Time Type  *****
 ***************************/

DEFINE_CONCRETE_RTT (rtINDEX_Key);


/**************************
 *****  Construction  *****
 **************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new key.
 *
 *  Argument:
 *	posPToken	- the positional p-token of 'source'.  Redundant but
 *			  available.
 *	source		- a standard descriptor for the elements to be held
 *                        in this key.
 *
 *  Returns:
 *	The address of the created key.
 *
 *****/
rtINDEX_Key::rtINDEX_Key (M_CPD *posPToken, DSC_Descriptor *source) : BaseClass (1) {
    NewKeyCount++;

/*****  Set the p-token...*****/
    posPToken->retain ();
    m_pPPT = posPToken;

/*****  Decode the source descriptor ... *****/
    if (source->isScalar ()) {
/*****  ... create a 'Scalar' key ... *****/
	m_xType = rtINDEX_KeyType_Scalar;
	m_iSet.construct (*source);
	m_iSequence.construct ();
    }
    else {
/*****  ... else create a 'Sequence' key ...  *****/
	m_xType = rtINDEX_KeyType_Sequence;
	m_iSequence.construct (*source);

	/*****  ... attempt to simplify the key to a scalar, ...  *****/
	AttemptSequenceToScalarKeyConv ();
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new 'future' key.
 *
 *  Arguments:
 *	parent			- the address of the key upon which the
 *				  derivative key will be based.
 *	subset			- an optional ('Nil' if absent) address of a
 *				  link constructor specifying the subset of
 *				  'parent' represented in the derivative key.
 *				  A pointer to this constructor may be placed
 *				  in the key returned by this routine.
 *	reordering		- an optional ('Nil' if absent) address of a
 *				  CPD for a reference u-vector specifying the
 *				  reordering of 'subset' applicable to the
 *				  subset key.  A pointer to this CPD may be
 *				  placed in the key returned by this routine.
 *
 *  Returns:
 *	The address of the subset key created.  If both 'subset' and
 *	'reordering' are omitted, 'parent' will be returned as the
 *	result of this routine.  As a consequence of this and
 *
 *****/
rtINDEX_Key::rtINDEX_Key (
    rtINDEX_Key *parent, rtLINK_CType *subset, M_CPD *reordering
) : BaseClass (1) {
/*****  Make sure that 'reordering' is really a reference uvector ... *****/
    M_CPD *ptoken;
    if (reordering) {
	if ((RTYPE_Type)M_CPD_RType (reordering) != RTYPE_C_RefUV) raiseException (
	    EC__InternalInconsistency,
	    "rtINDEX_NewFutureKey: 'reordering' must be a reference uvector"
	);

	if (rtPTOKEN_BaseElementCount (reordering, UV_CPx_RefPToken) !=
	    rtPTOKEN_BaseElementCount (reordering, UV_CPx_PToken)
	) raiseException (
	    EC__InternalInconsistency,
	    "rtINDEX_NewFutureKey: Reordering P-Tokens Differ In Size"
	);

	ptoken = UV_CPD_PosPTokenCPD (reordering);
    }
    else {
	ptoken = subset->PPT ();
	ptoken->retain ();
    }

/*****  ... and initialize a new key, ...  *****/
    m_pPPT = ptoken;

/***** ... as a new scalar key if 'parent' is scalar, ...  *****/
    if (rtINDEX_KeyType_Scalar == rtINDEX_Key_Type (parent)) {
	m_xType = rtINDEX_KeyType_Scalar;

	m_iSequence.construct ();
	m_iSet.construct (rtINDEX_Key_Set (parent));
    }
    else {
/*****  ... and as a subset key otherwise.  *****/
	m_xType = rtINDEX_KeyType_Future;

	if (IsntNil (rtINDEX_Key_Reordering (this) = reordering))
	    reordering->retain ();

	if (IsntNil (m_pSubset = subset))
	    subset->retain ();

	m_iSequence.construct ();
	m_iSet.construct ();
	parent->retain ();
	m_pParent = parent;
    }
}


rtINDEX_Key *rtINDEX_Key::NewFutureKey (rtLINK_CType *subset, M_CPD *reordering) {
    NewFutureKeyCount++;

//  Return 'this' if neither 'subset' nor 'reordering' were specified, ...
    if (IsNil (subset) && IsNil (reordering)) {
    	retain ();
        return this;
    }

//  ... otherwise create a new key:
    return new rtINDEX_Key (this, subset, reordering);
}


/*************************
 *****  Destruction  *****
 *************************/

/*---------------------------------------------------------------------------
 *  Note:
 *	This routine is NOT for public use; it is intended solely for use by
 *	'rtINDEX_Key::release'.
 *****/
rtINDEX_Key::~rtINDEX_Key () {
    m_pPPT->release ();

    switch (m_xType) {
    case rtINDEX_KeyType_Set:
	rtINDEX_Key_Enumerator (this)->release ();
	/*****  !!!  NO BREAK  !!!  *****/

    case rtINDEX_KeyType_Scalar:
	m_iSet.clear ();
	/*****  !!!  NO BREAK  !!!  *****/

    case rtINDEX_KeyType_Sequence:
	m_iSequence.clear ();
        break;

    case rtINDEX_KeyType_Future:
	if (rtINDEX_Key_Reordering (this))
	    rtINDEX_Key_Reordering (this)->release ();

	if (m_pSubset)
	    m_pSubset->release ();

	m_pParent->release ();
        break;

    default:
	raiseException (
	    EC__InternalInconsistency,
	    "rtINDEX_Key::~rtINDEX_Key: Unknown Key Type: %d",
	    m_xType
	);
        break;
    }
}


/***************************************************
 *****  Sequence to Scalar Conversion Utility  *****
 ***************************************************/

/*---------------------------------------------------------------------------
 *****  Internal utility to convert a sequence key to a scalar key if possible.
 *
 *  Argument:
 *	this			- the key to be converted.
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	This routine trys to simplify the key by turning it into a 'Scalar'
 *	key if possible.
 *
 *	If the key is not of type 'Sequence' it does nothing.
 *	For 'Sequence' keys, it looks at the structure contained in the
 *	sequence component.  If all of the values in this structure are
 *	the same, it will be converted to a 'Scalar' key.
 *
 *****/
void rtINDEX_Key::AttemptSequenceToScalarKeyConv () {
/*****  If the key is not a sequence... return  *****/
    if (!IsSequence ())
	return;

/*****  Determine if this sequence can be converted into a scalar ... *****/
    if (m_iSequence.isACoercedScalar ()) {
	// ... and convert to scalar if possible... *****/
	M_CPD *store = m_iSequence.storeCPD ();
	store->retain ();

	m_iSet.constructScalarComposition (
	    store, 0, DSC_Descriptor_Value (m_iSequence)
	);

	m_xType = rtINDEX_KeyType_Scalar;
    }
}


/*****************************
 *****  Key Realization  *****
 *****************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to realize a key given a key future.
 *
 *  Argument:
 *	this			- the key to be realized if represented as a
 *				  'future'.
 *
 *  Returns:
 *	NOTHING - Executed for possible side effect on '*this' only.
 *
 *  Notes:
 *	This routine does nothing if 'key' isn't a 'Future'.
 *	The 'type' of the realized 'Future' key will be either 'Scalar' or
 *	'Sequence'.
 *
 *****/
void rtINDEX_Key::Realize () {
/*****  If the key is not a future ... all done  *****/
    if (m_xType != rtINDEX_KeyType_Future)
	return;

/*****  Force the parent to not be a future ... *****/
    m_pParent->Realize ();

/*****  If the parent is a scalar this key becomes scalar ... *****/
    if (m_pParent->isScalar ()) {
	m_xType = rtINDEX_KeyType_Scalar;

	m_iSet.construct (rtINDEX_Key_Set (m_pParent));

	m_iSequence.clear ();
	if (m_pSubset) {
	    m_pSubset->release ();
	    m_pSubset = NilOf (rtLINK_CType *);
	}
	if (IsntNil (rtINDEX_Key_Reordering (this))) {
	    rtINDEX_Key_Reordering (this)->release ();
	    rtINDEX_Key_Reordering (this) = NilOf (M_CPD*);
	}
    }

/***** ... else, this key becomes a sequence key ... *****/
    else {
	m_xType = rtINDEX_KeyType_Sequence;

	m_pParent->RealizeSequence ();

	if (IsntNil (m_pSubset)) {
	    m_iSequence.constructComposition (
		m_pSubset, rtINDEX_Key_Sequence (m_pParent)
	    );
	    m_pSubset->release ();
	    m_pSubset = NilOf (rtLINK_CType *);
	}
	else {
	    m_iSequence.construct (rtINDEX_Key_Sequence (m_pParent));
	}

	if (IsntNil (rtINDEX_Key_Reordering (this))) {
	    m_iSequence.reorder (rtINDEX_Key_Reordering (this));
	    rtINDEX_Key_Reordering (this)->release ();
	    rtINDEX_Key_Reordering (this) = NilOf (M_CPD*);
	}
    }

    m_pParent->release ();

/*****  Try to convert the key to a scalar key ... *****/
    AttemptSequenceToScalarKeyConv ();
}


/**********************************
 *****  Sequence Realization  *****
 **********************************/

/*---------------------------------------------------------------------------
 *****  Routine to realize the sequence component of a key.
 *
 *  Arguments:
 *	this			- the key whose element sequence is to be
 *				  realized.
 *
 *  Returns:
 *	NOTHING - Executed for possible side effect on '*this' only.
 *
 *  Notes:
 *	This routine does two things: it makes sure that the 'sequence'
 *	component is present and it may also change the 'type' of the key
 *	based on the following rules.
 *	- If the key is of type 'Future' the result key will be either type
 *	  'Scalar' or type 'Sequence'.
 *	- All other key types will stay the same.  This is because they all
 *	  have a valid 'sequence' field and contain more information in the
 *	  form that they are in than in the 'Sequence' form.
 *
 *****/
void rtINDEX_Key::RealizeSequence () {
    RealizeKeySequenceCount++;

/*****  Realize any key 'futures', ...  *****/
    Realize ();

/*****  If sequence already exists... all done  *****/
    if (m_iSequence.isntEmpty ())
	return;

/*****  ... if Set ... *****/
    if (IsSet ()) {
	m_iSequence.constructComposition (rtINDEX_Key_Enumerator (this), m_iSet);
    }

/*****  ... if Scalar ...  *****/
    else if (isScalar ()) {
	m_iSequence.construct (m_iSet);
	m_iSequence.coerce (m_pPPT);
    }

/*****  ... else error ... *****/
    else raiseException (
	EC__InternalInconsistency,
	"rtINDEX_RealizeKeySequence: Unknown Key Type: %d",
	m_xType
    );
}


/*****************************
 *****  Set Realization  *****
 *****************************/

/*---------------------------------------------------------------------------
 *****  Routine to realize the set component of a key.
 *
 *  Arguments:
 *	this			- the key whose element set is to be
 *				  realized.
 *
 *  Returns:
 *	NOTHING - Executed for possible side effect on '*this' only.
 *
 *  Notes:
 *	This routine does two things: it makes sure that the 'set' component
 *	is present and it may also change the 'type' of the key based on the
 *	following rules.
 *	- If the key is of type 'Scalar' it stays a 'Scalar'.
 *	- If the key is of type 'Sequence' it becomes type 'Set'.
 *	- If the key is of type 'Future' it becomes either type 'Set' or
 *	  type 'Scalar'.
 *
 *****/
void rtINDEX_Key::RealizeSet () {
    RealizeKeySetCount++;

/*****  Realize any key 'futures', ...  *****/
    Realize ();

/*****  If scalar or set key ... all done  *****/
    if (isScalar () || IsSet ())
	return;

/*****  ... its a sequence key ... *****/
    m_iSet.constructSet (
	m_pPPT->Space (), rtINDEX_Key_Enumerator (this), m_iSequence
    );

    m_xType = rtINDEX_KeyType_Set;
}


/***********************
 *****  Key Query  *****
 ***********************/

bool rtINDEX_Key::HoldsKeysInStore (M_CPD *pStoreRefCPD, int xStoreRef) {
    rtINDEX_Key *pKey = this;
    DSC_Descriptor* pKeyDescriptor = NilOf (DSC_Descriptor*);
    while (IsNil (pKeyDescriptor)) {
	switch (rtINDEX_Key_Type (pKey)) {
	case rtINDEX_KeyType_Scalar:
	case rtINDEX_KeyType_Set:
	    pKeyDescriptor = &rtINDEX_Key_Set (pKey);
	    break;
	case rtINDEX_KeyType_Sequence:
	    pKeyDescriptor = &rtINDEX_Key_Sequence (pKey);
	    break;
	case rtINDEX_KeyType_Future:
	    pKey = rtINDEX_Key_Parent (pKey);
	    break;
	default:
	    raiseException (
		EC__InternalInconsistency,
		"rtINDEX_Key_HoldsKeysInStore: Unknown Key Type: %d",
		rtINDEX_Key_Type (pKey)
	    );
	    break;
	}
    }

    return pStoreRefCPD->ReferenceNames (xStoreRef, pKeyDescriptor->storeCPD ());
}


bool rtINDEX_Key::ConformsToIndex (M_CPD *pIndexCPD) {
    return HoldsKeysInStore (pIndexCPD, rtINDEX_CPx_KeyStore);
}

bool rtINDEX_Key::ConformsToIndex (DSC_Descriptor *pIndexDescriptor) {
    return HoldsKeysInStore (pIndexDescriptor->storeCPD (), rtINDEX_CPx_KeyStore);
}


/***************************
 ***************************
 *****  Index Manager  *****
 ***************************
 ***************************/

/***************************************
 *****  Internal Cluster Creation  *****
 ***************************************/

/*---------------------------------------------------------------------------
 *****  Routine to create a new index and its associated L-Store.
 *
 *  Arguments:
 *	pInstancePTokenCPD	- the address of a CPD for the list p-token of the
 *				  index.
 *	xKeyMapRType		- the index of the new index's key map r-type.  Must
 *				  be either RTYPE_C_Link or RTYPE_C_RefUV.
 *	xKeySetRType		- the index of the new index's key set r-type.  Must
 *				  be one of the value u-vector types or RTYPE_C_Vector.
 *	pKeySetRefPTokenCPD/xKeySetRefPTokenRef
 *				- a CPD address/index pair referencing a POP for the
 *				  reference p-token of the key set.  Required if
 *				  'xKeySetRType' is one of the u-vector types, ignored
 *				  otherwise.
 *	pKeyStoreRef/xKeyStoreRef
 *				- a CPD address/index pair referencing a POP for the
 *				  key store for this index.  Required if 'xKeySetRType'
 *				  is one of the u-vector types, ignored otherwise.
 *	pContentPrototypeCPD	- a CPD for the content prototype of the contained
 *				  l-store (Nil to request a Vector).
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *
 *****/
PrivateFnDef M_CPD *rtINDEX_NewCluster (
    M_CPD*			pInstancePTokenCPD,
    RTYPE_Type			xKeyMapRType,
    RTYPE_Type			xKeySetRType,
    M_CPD*			pKeySetRefPTokenRef,
    int				xKeySetRefPTokenRef,
    M_CPD*			pKeyStoreRef,
    int				xKeyStoreRef,
    M_CPD*			pContentPrototypeCPD
)
{
/*****  Superficially validate the key set creation arguments, ...  *****/
    if (RTYPE_C_Vector != xKeySetRType && IsNil (pKeyStoreRef)) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "rtINDEX_NewCluster: Key Store Required For Key Type %s",
	    RTYPE_TypeIdAsString (xKeySetRType)
	)
    );

/*****  ... create the key set, ...  *****/
    M_ASD *pContainerSpace = pInstancePTokenCPD->Space ();
    M_CPD *pKeySetPTokenCPD = rtPTOKEN_New (pContainerSpace, 0);
    M_CPD *pKeySetCPD;
    switch (xKeySetRType) {
    case RTYPE_C_Vector:
	pKeySetCPD = rtVECTOR_NewSetVector (pKeySetPTokenCPD);
	pKeyStoreRef = NilOf (M_CPD*);
	break;
    case RTYPE_C_CharUV:
	pKeySetCPD = rtCHARUV_New (
	    pKeySetPTokenCPD, pKeySetRefPTokenRef, xKeySetRefPTokenRef
	);
	UV_CPD_IsASetUV (pKeySetCPD) = true;
	break;
    case RTYPE_C_DoubleUV:
	pKeySetCPD = rtDOUBLEUV_New (
	    pKeySetPTokenCPD, pKeySetRefPTokenRef, xKeySetRefPTokenRef
	);
	UV_CPD_IsASetUV (pKeySetCPD) = true;
	break;
    case RTYPE_C_FloatUV:
	pKeySetCPD = rtFLOATUV_New (
	    pKeySetPTokenCPD, pKeySetRefPTokenRef, xKeySetRefPTokenRef
	);
	UV_CPD_IsASetUV (pKeySetCPD) = true;
	break;
    case RTYPE_C_IntUV:
	pKeySetCPD = rtINTUV_New (
	    pKeySetPTokenCPD, pKeySetRefPTokenRef, xKeySetRefPTokenRef
	);
	UV_CPD_IsASetUV (pKeySetCPD) = true;
	break;
    case RTYPE_C_Unsigned64UV:
	pKeySetCPD = rtU64UV_New (
	    pKeySetPTokenCPD, pKeySetRefPTokenRef, xKeySetRefPTokenRef
	);
	UV_CPD_IsASetUV (pKeySetCPD) = true;
	break;
    case RTYPE_C_Unsigned96UV:
	pKeySetCPD = rtU96UV_New (
	    pKeySetPTokenCPD, pKeySetRefPTokenRef, xKeySetRefPTokenRef
	);
	UV_CPD_IsASetUV (pKeySetCPD) = true;
	break;
    case RTYPE_C_Unsigned128UV:
	pKeySetCPD = rtU128UV_New (
	    pKeySetPTokenCPD, pKeySetRefPTokenRef, xKeySetRefPTokenRef
	);
	UV_CPD_IsASetUV (pKeySetCPD) = true;
	break;
    default:
	ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"rtINDEX_NewCluster: Invalid Key Set Type: %s",
		RTYPE_TypeIdAsString (xKeySetRType)
	    )
	);
    }

/*****  ... create the lstore ... *****/
    M_CPD *pLStoreCPD = rtLSTORE_NewCluster (pInstancePTokenCPD, pContentPrototypeCPD, 0);

/*****  create the key map, ... *****/
    M_CPD *pElementPTokenCPD = rtLSTORE_CPD_ContentPTokenCPD (pLStoreCPD);
    M_CPD *pKeyMapCPD;
    switch (xKeyMapRType) {
    case RTYPE_C_Link: {
	    M_CPD *pKeyMapRefPTokenCPD = rtPTOKEN_New (
		pContainerSpace, pInstancePTokenCPD, -1, pKeySetPTokenCPD, -1
	    );
	    pKeyMapCPD = rtLINK_NewEmptyLink (pElementPTokenCPD, pKeyMapRefPTokenCPD);
	    pKeyMapRefPTokenCPD->release ();
	}
	break;
    case RTYPE_C_RefUV:
	pKeyMapCPD = rtREFUV_New (pElementPTokenCPD, pKeySetPTokenCPD);
	break;
    default:
	ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"rtINDEX_NewCluster: Invalid Key Map Type: %s",
		RTYPE_TypeIdAsString (xKeyMapRType)
	    )
	);
	break;
    }

/*****  ... create and initialize the index, ... *****/
    M_CPD *pIndexCPD = POPVECTOR_New (
	pContainerSpace, RTYPE_C_Index, rtINDEX_CPD_StdPtrCount
    );

    pIndexCPD->StoreReference (rtINDEX_CPx_Lists	, pLStoreCPD);
    pIndexCPD->StoreReference (rtINDEX_CPx_Map		, pKeyMapCPD);
    pIndexCPD->StoreReference (rtINDEX_CPx_KeyValues	, pKeySetCPD);
    if (pKeyStoreRef)
	pIndexCPD->StoreReference (rtINDEX_CPx_KeyStore, pKeyStoreRef, xKeyStoreRef);

/*****  ... cleanup, ...  *****/
    pKeyMapCPD->release ();
    pElementPTokenCPD->release ();
    pLStoreCPD->release ();
    pKeySetCPD->release ();
    pKeySetPTokenCPD->release ();

/*****  ... and return:  *****/
    NewCount++;

    return pIndexCPD;
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new index and its associated L-Store.
 *
 *  Arguments:
 *	pInstancePTokenCPD	- the address of a CPD for the list p-token of the
 *				  index.
 *	xKeySetRType		- the index of the new index's key set r-type.  Must
 *				  be one of the value u-vector types or RTYPE_C_Vector.
 *	pKeySetRefPTokenCPD	- a CPD for the reference p-token of the key set.
 *				  Required if 'xKeySetRType' is one of the u-vector
 *				  types, ignored otherwise.
 *	pKeyStoreCPD		- a CPD for the key store for this index.  Required
 *				  if 'xKeySetRType' is one of the u-vector types,
 *				  ignored otherwise.
 *	pContentPrototypeCPD	- a CPD for the content prototype of the contained
 *				  l-store (Nil to request a Vector).
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *
 *****/
PublicFnDef M_CPD *rtINDEX_NewCluster (
    M_CPD*			pInstancePTokenCPD,
    RTYPE_Type			xKeySetRType,
    M_CPD*			pKeySetRefPTokenCPD,
    M_CPD*			pKeyStoreCPD,
    M_CPD*			pContentPrototypeCPD
)
{
    return rtINDEX_NewCluster (
	pInstancePTokenCPD,
	rtINDEX_KeyMapDefaultType,
	xKeySetRType,
	pKeySetRefPTokenCPD,
	-1,
	pKeyStoreCPD,
	-1,
	pContentPrototypeCPD
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new indexed list cluster.
 *
 *  Arguments:
 *	pInstancePTokenCPD	- a standard CPD for the positional p-token
 *				  of the index.
 *	pContentPrototypeCPD	- a CPD for the content prototype of the contained
 *				  l-store (Nil to request a Vector).
 *	--			- an unused argument that dis-ambiguates the content
 *				  and cluster prototype overloads of rtINDEX_NewCluster.
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *****/
PublicFnDef M_CPD *rtINDEX_NewCluster (
    M_CPD *pInstancePTokenCPD, M_CPD *pContentPrototypeCPD, int
)
{
    return rtINDEX_NewCluster (
	pInstancePTokenCPD,
	rtINDEX_KeyMapDefaultType,
	RTYPE_C_Vector,
	NilOf (M_CPD*),
	-1,
	NilOf (M_CPD*),
	-1,
	pContentPrototypeCPD
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new indexed list cluster.
 *
 *  Arguments:
 *	pInstancePTokenCPD	- the address of a CPD for the positional p-token
 *				  of the index.
 *	xKeyMapRType		- the index of the new index's key map r-type.  Must
 *				  be either RTYPE_C_Link or RTYPE_C_RefUV.
 *	pKeySetPrototype	- the address of a descriptor which provides the key's
 * 				  type information.
 *	pContentPrototypeCPD	- a CPD for the content prototype of the contained
 *				  l-store (Nil to request a Vector).
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *****/
PrivateFnDef M_CPD *rtINDEX_NewCluster (
    M_CPD*			pInstancePTokenCPD,
    RTYPE_Type			xKeyMapRType,
    DSC_Descriptor*	pKeySetPrototype,
    M_CPD*			pContentPrototypeCPD
)
{
    RTYPE_Type xKeySetRType = pKeySetPrototype->pointerRType ();

    M_CPD *pKeySetRefPTokenRef; int xKeySetRefPTokenRef;
    pKeySetPrototype->getRPTReference (pKeySetRefPTokenRef, xKeySetRefPTokenRef);

    M_CPD *pIndexCPD = rtINDEX_NewCluster (
	pInstancePTokenCPD,
	xKeyMapRType,
	xKeySetRType,
	pKeySetRefPTokenRef,
	xKeySetRefPTokenRef,
	pKeySetPrototype->storeCPDIfAvailable (),
	-1,
	pContentPrototypeCPD
    );
    pKeySetRefPTokenRef->release ();

    return pIndexCPD;
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new indexed list cluster.
 *
 *  Arguments:
 *	pInstancePTokenCPD	- the address of a CPD for the positional p-token
 *				  of the index.
 *	pKeySetPrototype	- the address of a descriptor which provides the key's
 * 				  type information.
 *	pContentPrototypeCPD	- a CPD for the content prototype of the contained
 *				  l-store (Nil to request a Vector).
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *****/
PublicFnDef M_CPD *rtINDEX_NewCluster (
    M_CPD*			pInstancePTokenCPD,
    DSC_Descriptor*	pKeySetPrototype,
    M_CPD*			pContentPrototypeCPD
)
{
    return rtINDEX_NewCluster (
	pInstancePTokenCPD, rtINDEX_KeyMapDefaultType, pKeySetPrototype, pContentPrototypeCPD
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new indexed list cluster.
 *
 *  Arguments:
 *	pInstancePTokenCPD	- the address of a CPD for the positional p-token
 *				  of the index.
 *	pPrototypeIndexCPD	- the address of a CPD for an existing index which will
 *				  be cloned.
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *****/
PublicFnDef M_CPD *rtINDEX_NewCluster (
    M_CPD *pInstancePTokenCPD, M_CPD *pPrototypeIndexCPD
)
{
    M_CPD *pKeySetCPD = pPrototypeIndexCPD->GetCPD (rtINDEX_CPx_KeyValues);
    M_CPD *pKeyMapCPD = pPrototypeIndexCPD->GetCPD (rtINDEX_CPx_Map);
    M_CPD *pContentPrototypeCPD; {
	M_CPD *pLStoreCPD = rtINDEX_CPD_ListStoreCPD (pPrototypeIndexCPD);
	pContentPrototypeCPD = rtLSTORE_CPD_ContentCPD (pLStoreCPD);
	pLStoreCPD->release ();
    }

    M_CPD *pIndexCPD = rtINDEX_NewCluster (
	pInstancePTokenCPD,
	rtINDEX_KeyMapAutoConversionEnabled
	    ? rtINDEX_KeyMapDefaultType
	    : (RTYPE_Type)M_CPD_RType (pKeyMapCPD),
	(RTYPE_Type)M_CPD_RType (pKeySetCPD),
	pKeySetCPD,
	UV_CPx_RefPToken,
	pPrototypeIndexCPD,
	rtINDEX_CPx_KeyStore,
	pContentPrototypeCPD
    );

    pContentPrototypeCPD->release ();
    pKeyMapCPD->release ();
    pKeySetCPD->release ();

    return pIndexCPD;
}


/*---------------------------------------------------------------------------
 * Routine to make a new indexed list store given a number of initialization
 * parameters.
 *
 *  Arguments:
 *	pContentPartition	- a link constructor whose reference ptoken
 *				  denotes the set of lists in this Index,
 *				  whose positional ptoken denotes the set of
 *				  list elements in the Index, and whose
 *				  transitional state defines the breakpoint
 *				  array of the underlying L-Store.
 *	pContent		- a CPD for the index's list elements.
 *	xKeyMapRType		- the index of the new index's key map r-type.
 *				  Must be RTYPE_C_Link or RTYPE_C_RefUV.
 *	pKey			- an 'rtINDEX_Key *' defining the key
 * 				  for this index.
 *
 *  Returns:
 *	A cpd for the index created.
 *
 *****/
PrivateFnDef M_CPD *rtINDEX_NewCluster (
    rtLINK_CType	*pContentPartition,
    M_CPD		*pContent,
    RTYPE_Type		 xKeyMapRType,
    rtINDEX_Key		*pKey
) {

/*****  Make the LStore ... *****/
    M_CPD *pLStoreCPD = rtLSTORE_NewCluster (pContentPartition, pContent, true);
    M_ASD *pContainerSpace = pLStoreCPD->Space ();

/*****  ... obtain the key set, ... *****/
    pKey->RealizeSet ();

    M_CPD *pKeySetCPD;
    if (pKey->isScalar ()) {
	DSC_Pointer setPointer;
	M_CPD *pKeyEnumeratorCPD;

	pKey->RealizeSequence ();
	setPointer.constructSet (
	    pContainerSpace,
	    pKeyEnumeratorCPD,
	    DSC_Descriptor_Pointer (rtINDEX_Key_Sequence (pKey))
	);
	pKeySetCPD = DSC_Pointer_Value (setPointer);
	pKeyEnumeratorCPD->release ();
    }
    else pKeySetCPD = M_DuplicateCPDPtr (
	DSC_Descriptor_Value (rtINDEX_Key_Set (pKey))
    );

/*****  ... create the key map, ...  *****/
    M_CPD *pKeyMapCPD;
    switch (xKeyMapRType) {
    case RTYPE_C_Link: {
	    M_CPD *pKeyMapRefPTokenCPD = rtPTOKEN_New (
		pContainerSpace, pLStoreCPD, rtLSTORE_CPx_RowPToken, pKeySetCPD, UV_CPx_PToken
	    );

	    rtLINK_CType *pKeyMapLC = rtLINK_RefConstructor (pKeyMapRefPTokenCPD, -1);
	    unsigned int const numberOfRows = rtPTOKEN_BaseElementCount (
		pLStoreCPD, rtLSTORE_CPx_RowPToken
	    );
	    unsigned int const numberOfColumns = rtPTOKEN_BaseElementCount (
		pKeySetCPD, UV_CPx_PToken
	    );
	    unsigned int const *pBreakpoint = rtLSTORE_CPD_BreakpointArray (
		pLStoreCPD
	    ) + 1;

	    unsigned int elementNumber = 0;
	    if (pKey->isScalar ()) {
		for (
		    unsigned int rowNumber = 0;
		    rowNumber < numberOfRows;
		    rowNumber++, pBreakpoint++
		)
		{
		    if (elementNumber < *pBreakpoint) {
			elementNumber++;
			rtLINK_AppendRange (pKeyMapLC, rowNumber, 1);
		    }
		}
	    }
	    else {
		unsigned int const *enumeration = (unsigned int *)rtREFUV_CPD_Array (
		    rtINDEX_Key_Enumerator (pKey)
		);
		for (
		    unsigned int rowNumber = 0;

		    rowNumber < numberOfRows;

		    rowNumber++, pBreakpoint++
		)
		{
		    while (elementNumber < *pBreakpoint)
		    {
			elementNumber++;
			rtLINK_AppendRange (
			    pKeyMapLC, rowNumber * numberOfColumns + *enumeration++, 1
			);
		    }
		}
	    }
	    pKeyMapLC->Close (pContentPartition->PPT ());
	    pKeyMapCPD = pKeyMapLC->ToLink ();

	    pKeyMapRefPTokenCPD->release ();
	}
	break;
    case RTYPE_C_RefUV:
	if (pKey->isScalar ()) {
	    pKeyMapCPD = rtREFUV_New (
		pContentPartition->PPT (), pKeySetCPD, (int)UV_CPx_PToken
	    );
	    memset (
		rtREFUV_CPD_Array (pKeyMapCPD),
		0,
		UV_CPD_ElementCount (pKeyMapCPD) * UV_CPD_Granularity (pKeyMapCPD)
	    );
	}
	else pKeyMapCPD = M_DuplicateCPDPtr (rtINDEX_Key_Enumerator (pKey));
	break;
    default:
	ERR_SignalFault (
	    EC__UnknownUVectorRType, UTIL_FormatMessage (
		"rtINDEX_NewCluster: Invalid Key Map Type: %s",
		RTYPE_TypeIdAsString (xKeyMapRType)
	    )
	);
	break;
    }

/*****  ....  Make the index  .... *****/
    M_CPD *pIndexCPD = POPVECTOR_New (
	pContainerSpace, RTYPE_C_Index, rtINDEX_CPD_StdPtrCount
    );

/*****  ...  initialize it  ...  *****/
    pIndexCPD->StoreReference (rtINDEX_CPx_Lists, pLStoreCPD);
    pIndexCPD->StoreReference (
	rtINDEX_CPx_KeyStore, rtINDEX_Key_Set (pKey).storeCPDIfAvailable ()
    );
    pIndexCPD->StoreReference (rtINDEX_CPx_KeyValues, pKeySetCPD);
    pIndexCPD->StoreReference (rtINDEX_CPx_Map, pKeyMapCPD);

/*****  ...  clean up  ...  *****/
    pLStoreCPD->release ();
    pKeyMapCPD->release ();
    pKeySetCPD->release ();

/*****  ... and return the result.  *****/
    return pIndexCPD;
}


/*---------------------------------------------------------------------------
 * Routine to make a new indexed list store given a number of initialization
 * parameters.
 *
 *  Arguments:
 *	pContentPartition	- a link constructor whose reference ptoken
 *				  denotes the set of lists in this Index,
 *				  whose positional ptoken denotes the set of
 *				  list elements in the Index, and whose
 *				  transitional state defines the breakpoint
 *				  array of the underlying L-Store.
 *	pContent		- a CPD for the index's list elements.
 *	pKey			- an 'rtINDEX_Key *' defining the key
 * 				  for this index.
 *
 *  Returns:
 *	A cpd for the index created.
 *
 *****/
PublicFnDef M_CPD *rtINDEX_NewCluster (
    rtLINK_CType *pContentPartition, M_CPD *pContent, rtINDEX_Key *pKey
) {
    return rtINDEX_NewCluster (
	pContentPartition, pContent, rtINDEX_KeyMapDefaultType, pKey
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new time-series indexed list cluster.
 *
 *  Arguments:
 *	pInstancePTokenCPD	- a standard CPD for the positional p-token
 *				  of the index.
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *****/
PublicFnDef M_CPD *rtINDEX_NewTimeSeries (M_CPD *pInstancePTokenCPD) {
    NewTimeSeriesCount++;

    M_KOTE const &rDate = pInstancePTokenCPD->TheDateClass ();

    return rtINDEX_NewCluster (
	pInstancePTokenCPD,
	rtINDEX_KeyMapDefaultType,
	RTYPE_C_IntUV,
	rDate.PTokenCPD (),
	-1,
	rDate,
	-1,
	NilOf (M_CPD*)
    );
}


/*****************************
 *****  Simple Alignment *****
 *****************************/

/*---------------------------------------------------------------------------
 *****  Routine to align an Index when the row ptoken is not current.
 *
 *  Arguments:
 *	pIndexCPD		- the address of a CPD for the index to align.
 *	fDeletingKeyMapNils	- a flag that, when non-zero, requests the deletion
 *				  of nils from partitioned key maps.
 *
 *  Returns:
 *	'pIndexCPD'
 *
 *****/
PublicFnDef M_CPD *rtINDEX_Align (M_CPD *pIndexCPD, bool fDeletingKeyMapNils) {
/*****
 *  Align the l-store first so that the key map alignment does not cause
 *  the list and element p-tokens of the l-store to be out of alignment
 *  simultaneously, ...
 *****/
    M_CPD *pLStoreCPD = rtINDEX_CPD_ListStoreCPD (pIndexCPD);
    rtLSTORE_Align (pLStoreCPD);

/*****
 *  Align the key map, and, if element deletions were possible, align the
 *  l-store again to reflect those deletions, ...
 *****/
    M_CPD *pKeyMapCPD = pIndexCPD->GetCPD (rtINDEX_CPx_Map);
    switch ((RTYPE_Type)M_CPD_RType (pKeyMapCPD))
    {
    case RTYPE_C_Link:
	rtLINK_AlignLink 	(pKeyMapCPD);
	rtLSTORE_Align		(pLStoreCPD);
	break;

    case RTYPE_C_RefUV:
	if (fDeletingKeyMapNils)
	{
	    rtREFUV_DeleteNils	(pKeyMapCPD);
	    rtLSTORE_Align	(pLStoreCPD);
	}
	else rtREFUV_Align (pKeyMapCPD);
	break;

    default:
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    UTIL_FormatMessage (
		"rtINDEX_Align: Invalid Index Map Type: %s",
		RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (pKeyMapCPD))
	    )
	);
	break;
    }
    pKeyMapCPD->release ();
    pLStoreCPD->release ();

    return pIndexCPD;
}

/*---------------------------------------------------------------------------
 *****  Routine to align an Index when the row ptoken is not current.
 *
 *  Arguments:
 *	pIndexCPD		- the address of a CPD for the index to align.
 *
 *  Returns:
 *	'pIndexCPD'
 *
 *****/
PublicFnDef M_CPD *rtINDEX_Align (
    M_CPD*			pIndexCPD
)
{
    return rtINDEX_Align (pIndexCPD, false);
}


/*********************************
 *****  Recursive Alignment  *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Routine to comprehensively align an Index.
 *
 *  Arguments:
 *	pIndexCPD		- a standard CPD for the Index to be aligned.
 *	deletingEmptyUSegments	- a boolean which, when true, requests that
 *				  unreferenced u-segments be deleted from
 *				  vectors.
 *
 *  Returns:
 *	true if an alignment was performed, false otherwise.
 *
 *****/
PublicFnDef bool rtINDEX_AlignAll (M_CPD *pIndexCPD, bool deletingEmptyUSegments) {
    bool result; {
	M_CPD *pLStoreCPD = rtINDEX_CPD_ListStoreCPD (pIndexCPD);
	result = rtLSTORE_AlignAll (pLStoreCPD, deletingEmptyUSegments);
	pLStoreCPD->release ();
    }
    {
	M_CPD *pKeySetCPD = pIndexCPD->GetCPD (rtINDEX_CPx_KeyValues);
	switch ((RTYPE_Type)M_CPD_RType (pKeySetCPD)) {
	case RTYPE_C_Vector:
	    result = rtVECTOR_AlignAll (pKeySetCPD, deletingEmptyUSegments) || result;
	    break;
	case RTYPE_C_CharUV:
	    rtCHARUV_Align (pKeySetCPD);
	    break;
	case RTYPE_C_DoubleUV:
	    rtDOUBLEUV_Align (pKeySetCPD);
	    break;
	case RTYPE_C_FloatUV:
	    rtFLOATUV_Align (pKeySetCPD);
	    break;
	case RTYPE_C_IntUV:
	    rtINTUV_Align (pKeySetCPD);
	    break;
	case RTYPE_C_Unsigned64UV:
	    rtU64UV_Align (pKeySetCPD);
	    break;
	case RTYPE_C_Unsigned96UV:
	    rtU96UV_Align (pKeySetCPD);
	    break;
	case RTYPE_C_Unsigned128UV:
	    rtU128UV_Align (pKeySetCPD);
	    break;
	default:
	    pKeySetCPD->release ();
	    ERR_SignalFault (
		EC__InternalInconsistency, UTIL_FormatMessage (
		    "rtINDEX_AlignAll: Invalid Index Key Type: %s",
		    RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (pKeySetCPD))
		)
	    );
	    break;
	}
	pKeySetCPD->release ();
    }
    {
	M_CPD *pKeyMapCPD = pIndexCPD->GetCPD (rtINDEX_CPx_Map);
	switch ((RTYPE_Type)M_CPD_RType (pKeyMapCPD)) {
	case RTYPE_C_Link:
	    if (!rtPTOKEN_IsCurrent (pKeyMapCPD, rtLINK_CPx_PosPToken)
	    ||  !rtPTOKEN_IsCurrent (pKeyMapCPD, rtLINK_CPx_RefPToken)
	    )
	    {
		rtLINK_AlignLink (pKeyMapCPD);
		result = true;
	    }
	    break;

	case RTYPE_C_RefUV:
	    if (!rtPTOKEN_IsCurrent (pKeyMapCPD, UV_CPx_PToken)
	    ||  !rtPTOKEN_IsCurrent (pKeyMapCPD, UV_CPx_RefPToken)
	    ) result = true;

	    result = rtREFUV_DeleteNils (pKeyMapCPD) || result;
	    break;

	default:
	    pKeyMapCPD->release ();
	    ERR_SignalFault (
		EC__InternalInconsistency, UTIL_FormatMessage (
		    "rtINDEX_AlignAll: Invalid Index Map Type: %s",
		    RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (pKeyMapCPD))
		)
	    );
	    break;
	}
	pKeyMapCPD->release ();
    }
    {
	M_CPD *pLStoreCPD = rtINDEX_CPD_ListStoreCPD (pIndexCPD);
	if (rtLSTORE_AlignAll (pLStoreCPD, deletingEmptyUSegments))
	    result = true;
	pLStoreCPD->release ();
    }

    return result;
}


/*************************************************
 *****  Product Key Map Construction Helper  *****
 *************************************************/

PrivateFnDef M_CPD *rtINDEX_PartitionedMapAsProductMap (M_CPD *pKeyMapCPD, M_CPD *pLStoreCPD) {
    rtLINK_CType *pKeyMapLC; {
	M_CPD *pKeyMapRefPTokenCPD = rtPTOKEN_New (
	    pLStoreCPD->Space (),
	    pLStoreCPD, rtLSTORE_CPx_RowPToken,
	    pKeyMapCPD, UV_CPx_RefPToken
	);

	pKeyMapLC = rtLINK_RefConstructor (pKeyMapRefPTokenCPD, -1);

	pKeyMapRefPTokenCPD->release ();
    }

    unsigned int const numberOfRows = rtPTOKEN_BaseElementCount (
	pLStoreCPD, rtLSTORE_CPx_RowPToken
    );
    unsigned int const numberOfColumns = rtPTOKEN_BaseElementCount (
	pKeyMapCPD, UV_CPx_RefPToken
    );
    unsigned int const *pBreakpoint = rtLSTORE_CPD_BreakpointArray (
	pLStoreCPD
    ) + 1;

    unsigned int elementNumber = 0;
    unsigned int const *enumeration = (unsigned int *)rtREFUV_CPD_Array (pKeyMapCPD);
    for (
	unsigned int rowNumber = 0; rowNumber < numberOfRows; rowNumber++, pBreakpoint++
    ) {
	while (elementNumber < *pBreakpoint) {
	    elementNumber++;
	    rtLINK_AppendRange (
		pKeyMapLC, rowNumber * numberOfColumns + *enumeration++, 1
	    );
	}
    }
    pKeyMapLC->Close (pKeyMapCPD, UV_CPx_PToken);

    return pKeyMapLC->ToLink ();
}


/********************************
 *****  Key Map Conversion  *****
 ********************************/

PublicFnDef void rtINDEX_GetKeyMapType (
    M_CPD*			pIndexCPD,
    RTYPE_Type&			xKeyMapType
)
{
    M_CPD *pKeyMapCPD = pIndexCPD->GetCPD (rtINDEX_CPx_Map);
    xKeyMapType = (RTYPE_Type)M_CPD_RType (pKeyMapCPD);
    pKeyMapCPD->release ();
}

PublicFnDef void rtINDEX_SetKeyMapType (
    M_CPD*			pIndexCPD,
    RTYPE_Type			xKeyMapType
)
{
    rtINDEX_Align (pIndexCPD, true);

    M_CPD *pKeyMapCPD = pIndexCPD->GetCPD (rtINDEX_CPx_Map);
    M_CPD *pNewMapCPD = NilOf (M_CPD*);
    switch ((RTYPE_Type)M_CPD_RType (pKeyMapCPD)) {
    case RTYPE_C_Link:
	if (RTYPE_C_RefUV == xKeyMapType) {
	    rtLINK_CType *pKeyMapLC = rtLINK_ToConstructor (pKeyMapCPD);
	    pNewMapCPD = rtLINK_ColumnProjection (pKeyMapLC);
	    pKeyMapLC->release ();
	}
	break;
    case RTYPE_C_RefUV:
	if (RTYPE_C_Link == xKeyMapType) {
	    M_CPD *pLStoreCPD = rtINDEX_CPD_ListStoreCPD (pIndexCPD);
	    pNewMapCPD = rtINDEX_PartitionedMapAsProductMap (pKeyMapCPD, pLStoreCPD);
	    pLStoreCPD->release ();
	}
	break;
    default:
	break;
    }
    if (IsntNil (pNewMapCPD)) {
	pIndexCPD->StoreReference (rtINDEX_CPx_Map, pNewMapCPD);
	pNewMapCPD->release ();
    }
    pKeyMapCPD->release ();
}


/********************************
 *****  Key Set Correction  *****
 ********************************/

/*---------------------------------------------------------------------------
 ***** Private Routine to ensure that an index's key meets the definition of
 ***** a vector/uvector set.
 *
 *  Arguments:
 *	pIndexCPD		- a pointer to a CPD for the index.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void EnsureIndexKeyMeetsSetDefinition (M_CPD *pIndexCPD) {

#define no_op(position,count,origin)

#define handleRepeat(position, count, origin) {\
    pKeySetDomainAdjustment->AppendAdjustment (origin + 1 - sKeySetDomainAdjustment, -1);\
    sKeySetDomainAdjustment++;\
}

#define handleRange(position, count, origin) {\
    pKeySetDomainAdjustment->AppendAdjustment (\
	origin + count - sKeySetDomainAdjustment, -count\
    );\
    sKeySetDomainAdjustment += count;\
}

/*****  Obtain the key set, ... *****/
    M_CPD *pKeySetCPD = pIndexCPD->GetCPD (rtINDEX_CPx_KeyValues);
    rtLINK_CType *refNilsLC;
    switch ((RTYPE_Type)M_CPD_RType (pKeySetCPD)) {
    case RTYPE_C_Vector:
/*****  If the key set is valid, do a simple alignment...  *****/
	if (rtVECTOR_IsASet (pKeySetCPD, &refNilsLC))
	    rtINDEX_Align (pIndexCPD);

/*****  If the key set can be fixed by deleting its nil elements, do so...  *****/
	else if (IsntNil (refNilsLC)) {
	    rtPTOKEN_CType *pKeySetDomainAdjustment = rtPTOKEN_NewShiftPTConstructor (
		refNilsLC->RPT (), -1
	    );
	    unsigned int sKeySetDomainAdjustment = 0;

	    rtLINK_TraverseRRDCList (refNilsLC, no_op, handleRepeat, handleRange);
	    refNilsLC->release ();

	    pKeySetDomainAdjustment->ToPToken ()->release ();

	    if (rtVECTOR_RestoreSetAttribute (pKeySetCPD))
		rtINDEX_Align (pIndexCPD, true);
	    else ERR_SignalFault (
		EC__InternalInconsistency,
		"rtINDEX_EnsureIndexKeyMeetsSetDefinition: Set Restore Failed"
	    );
	}

/*****  Otherwise, fail...  *****/
	else ERR_SignalFault (
	    EC__InternalInconsistency,
	    "rtINDEX_EnsureIndexKeyMeetsSetDefinition: Not a Vector Set"
	);
	break;

    case RTYPE_C_CharUV:
    case RTYPE_C_DoubleUV:
    case RTYPE_C_FloatUV:
    case RTYPE_C_IntUV:
    case RTYPE_C_UndefUV:
    case RTYPE_C_Unsigned64UV:
    case RTYPE_C_Unsigned96UV:
    case RTYPE_C_Unsigned128UV:
/*****  If the key set is valid, do a simple alignment...  *****/
	if (UV_CPD_IsASetUV (pKeySetCPD))
	    rtINDEX_Align (pIndexCPD);

/*****  Otherwise, fail...  *****/
	else ERR_SignalFault (
	    EC__InternalInconsistency,
	    "rtINDEX_EnsureIndexKeyMeetsSetDefinition: Not a UV Set"
	);
	break;

    default:
	ERR_SignalFault (
	    EC__InternalInconsistency,
	    UTIL_FormatMessage (
		"rtINDEX_EnsureIndexKeyMeetsSetDefinition: Invalid Index Key Type: %s",
		RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (pKeySetCPD))
	    )
	);
	break;
    }
    pKeySetCPD->release ();

#undef handleRange
#undef handleRepeat
#undef no_op
}


/*****************************
 *****  Key Set Cleanup  *****
 *****************************/

/*---------------------------------------------------------------------------
 *****  Key Set Cleanup.
 *
 *  Arguments:
 *	pIndexCPD		- a cpd for the index being cleaned up.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtINDEX_DeleteUnusedKeys (M_CPD *pIndexCPD) {

#define handleNil(cumulative, count, columnNumber) {\
    if (columnNumber > lastColumn) pKeyPTokenConstructor->AppendAdjustment (\
	    columnNumber + totalAdjustment, lastColumn - columnNumber\
    );\
}

#define handleRepeat(cumulative, count, columnNumber) {\
    if (columnNumber > lastColumn) {\
        pKeyPTokenConstructor->AppendAdjustment (\
	    columnNumber + totalAdjustment, adjustment = lastColumn - columnNumber\
	);\
	totalAdjustment += adjustment;\
    }\
    lastColumn = columnNumber + 1;\
}

#define handleRange(cumulative, count, columnNumber) {\
    if (columnNumber > lastColumn) {\
        pKeyPTokenConstructor->AppendAdjustment (\
	    columnNumber + totalAdjustment, adjustment = lastColumn - columnNumber\
	);\
	totalAdjustment += adjustment;\
    }\
    lastColumn = columnNumber + count;\
}

/*****  Check index integrity ... *****/
    EnsureIndexKeyMeetsSetDefinition (pIndexCPD);

/*****  ... get the key projection of the presence map, ...  *****/
    M_CPD *pKeyMapCPD = pIndexCPD->GetCPD (rtINDEX_CPx_Map);
    rtLINK_CType *pKeySelectorLC;
    switch ((RTYPE_Type)M_CPD_RType (pKeyMapCPD)) {
    case RTYPE_C_Link: {
	    rtLINK_CType *pKeyMapLC = rtLINK_ToConstructor (pKeyMapCPD);

	    M_CPD *pKeyEnumerationCPD = rtLINK_ColumnProjection (pKeyMapLC);
	    pKeyMapLC->release ();

	    pKeySelectorLC = rtLINK_RefUVToConstructor (pKeyEnumerationCPD);
	    pKeyEnumerationCPD->release ();
	}
	break;

    case RTYPE_C_RefUV:
	pKeySelectorLC = rtLINK_RefUVToConstructor (pKeyMapCPD);
	break;

    default:
	ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"rtINDEX_Align: Invalid Index Map Type: %s",
		RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (pKeyMapCPD))
	    )
	);
	break;
    }

/*****  Prepare the key ptoken for editing  *****/
    M_CPD *pKeyPTokenCPD = pKeySelectorLC->RPT ();
    rtPTOKEN_CType *pKeyPTokenConstructor = rtPTOKEN_NewShiftPTConstructor (
	pKeyPTokenCPD, -1
    );

/*****  Traverse the key selector, editing the column ptoken  *****/
    int totalAdjustment = 0;
    int adjustment = 0;
    int lastColumn = 0;
    rtLINK_TraverseRRDCList (pKeySelectorLC, handleNil, handleRepeat, handleRange);
    handleNil (0, 0, (int)rtPTOKEN_CPD_BaseElementCount (pKeyPTokenCPD));

/*****  Commit the changes  *****/
    pKeyPTokenConstructor->ToPToken ()->release ();
    pKeySelectorLC->release ();

    pKeyMapCPD->release ();

#undef handleNil
#undef handleRepeat
#undef handleRange
}


/**********************************
 *****  Object Instantiation  *****
 **********************************/

/*---------------------------------------------------------------------------
 *****  Routine to add a specified number of rows to the end of an index.
 *
 *  Arguments:
 *	index			- a standard CPD for an index.
 *	newListsPToken		- a standard CPD for a P-Token which
 *				  specifies the number of rows to be added.
 *
 *  Returns:
 *	An link constructor referencing the rows added to the index.  The
 *	positional P-Token of this link constructor will be 'newListsPToken'.
 *
 *****/
PublicFnDef rtLINK_CType *rtINDEX_AddLists (M_CPD *index, M_CPD *newListsPToken) {
/*****  Check index integrity ... *****/
    EnsureIndexKeyMeetsSetDefinition (index);

    M_CPD *pLStoreCPD = rtINDEX_CPD_ListStoreCPD (index);
    rtLINK_CType* result = rtLSTORE_AddLists (pLStoreCPD, newListsPToken);
    pLStoreCPD->release ();

    return result;
}


/*********************************************
 *****  Subscripted (Positional) Access  *****
 *********************************************/

/*---------------------------------------------------------------------------
 ***** Index Subscript Routine.
 *
 *  Arguments:
 *	sourceDsc	- a pointer to a descriptor whose store
 *		          contains the Index to subscript from.
 *			  The descriptor must be either a Link
 *                        Descriptor or a Reference Scalar Descriptor
 *		          whose pointer specifies which lists to
 *                        subscript in.
 *	keyDsc		- a pointer to a descriptor whose Pointer contains
 *			  either an integer scalar pointer or an integer
 *			  value pointer.  This set of integers specifies
 *			  the list positions to subscript in the
 *			  selected lists.  The Pointer in 'keyDsc' must
 *                        be positionally related to the Pointer in
 *                        'sourceDsc'.
 *	keyModifier	- an integer to be added to each value in 'keyDsc'.
 *			  This usually either 0 or -1.  It is used to
 *                        simulate 1 based subscripts.
 *	resultDsc	- a pointer to a descriptor which this routine will
 *                        set to the result of the subscript.
 *	locatedLinkc	- a pointer to a link constructor which this routine
 *			  will create ONLY if needed.  It will be created if
 *			  not all of the values were found or not found
 *                        (if some were found and some not found).  It will
 *                        contain the positions in 'keyDsc' for which a value
 *			  was found.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtINDEX_Subscript (
    DSC_Descriptor	 *sourceDsc,
    DSC_Descriptor	 *keyDsc,
    int			  keyModifier,
    DSC_Descriptor	 *resultDsc,
    rtLINK_CType	**locatedLinkc
)
{
/*****
 *  Create a new descriptor containing the lstore and sourceDsc's pointer ...
 *****/
    M_CPD *index = sourceDsc->storeCPD ();

/*****  Check its integrity ... *****/
    EnsureIndexKeyMeetsSetDefinition (index);

    DSC_Descriptor indexDsc;
    indexDsc.store ().construct (rtINDEX_CPD_ListStoreCPD (index));
    DSC_Descriptor_Pointer (indexDsc).construct (DSC_Descriptor_Pointer (*sourceDsc));

/*****  Perform the function ...  *****/
    rtLSTORE_Subscript (
	&indexDsc, keyDsc, keyModifier, resultDsc, locatedLinkc
    );

/*****  Cleanup and exit ...  *****/
    indexDsc.clear ();
}


/**************************************
 *****  Time Zero Access Routine  *****
 **************************************/

/*---------------------------------------------------------------------------
 *****  Routine to lookup point(s) in an index.
 *
 *  Arguments:
 *	pResultDescriptor	- the address of a descriptor which will be set to
 *				  contain the point(s) found.
 *	pIndexDescriptor	- the address of a descriptor naming the index.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PublicFnDef void rtINDEX_TimeZero (
    DSC_Descriptor*	pResultDescriptor,
    DSC_Descriptor*	pIndexDescriptor
)
{
/*****  Get the index, ... *****/
    M_CPD *pIndexCPD = pIndexDescriptor->storeCPD ();

/*****  ... validate that it is a time-series, ...  *****/
    if (!rtINDEX_CPD_IsATimeSeries (pIndexCPD)) ERR_SignalFault (
	EC__InternalInconsistency, "rtINDEX_TimeZero: Not A Time-Series"
    );

/*****  ... get the result's positional p-token, ...  *****/
    M_CPD *pPosPTokenCPD = pIndexDescriptor->PPT ();

/*****  ... get the key set, ...  *****/
    M_CPD *pKeySetCPD = pIndexCPD->GetCPD (rtINDEX_CPx_KeyValues);

/*****  ... construct the result, ...  *****/
    pResultDescriptor->constructZero (
	pIndexCPD->GetCPD (rtINDEX_CPx_KeyStore),
	(RTYPE_Type)M_CPD_RType (pKeySetCPD),
	pPosPTokenCPD,
	UV_CPD_RefPTokenCPD (pKeySetCPD)
    );

/*****  ... and cleanup and return.  *****/
    pKeySetCPD->release ();
    pPosPTokenCPD->release ();
}


/*********************************
 *****  Extraction Routines  *****
 *********************************/

PublicFnDef void rtINDEX_Extract (
    DSC_Descriptor	 &rIndexDescriptor,
    M_CPD		*&pElementCluster,
    rtLINK_CType	*&elementSelector,
    rtLINK_CType	*&expansionLinkc,
    M_CPD		*&elementDistribution,
    rtINDEX_Key		*&pKey
)
{
    ExtractCount++;

/*****  Get the index out of 'pIndexDescriptor' ... *****/
    M_CPD *pIndexCPD = rIndexDescriptor.storeCPD ();

/*****  Check its integrity ... *****/
    EnsureIndexKeyMeetsSetDefinition (pIndexCPD);

/*****  Extract elements from the l-store ... *****/
    {
	DSC_Descriptor lstoreDsc;
	lstoreDsc.construct (rIndexDescriptor);

	lstoreDsc.setStoreTo (rtINDEX_CPD_ListStoreCPD (pIndexCPD));

	rtLSTORE_Extract (
	    lstoreDsc, pElementCluster, elementSelector, expansionLinkc, elementDistribution
	);

	lstoreDsc.clear ();
    }

/*****
 *  If the index isn't a time-series, then its keys are stored in a vector.
 *  As of now (6/3/87) the handling of this type of index has not been
 *  defined and no rtINDEX_Key can be created. Thus, cleanup and return.
 *****/
    if (!rtINDEX_CPD_IsATimeSeries (pIndexCPD)) {
	pKey = NilOf (rtINDEX_Key *);
	return;
    }

/*****  Get a selector for the keys ... *****/
    M_CPD *pKeySelectorCPD; {
	M_CPD *pKeyMapCPD = pIndexCPD->GetCPD (rtINDEX_CPx_Map);
	switch ((RTYPE_Type)M_CPD_RType (pKeyMapCPD)) {
	case RTYPE_C_Link: {
		rtLINK_CType *pMapSelectorLC = rtLINK_LCExtract (
		    pKeyMapCPD, elementSelector
		);

	    /*****  Get the column projection ... *****/
		pKeySelectorCPD = rtLINK_ColumnProjection (pMapSelectorLC);
		pMapSelectorLC->release ();
	    }
	    break;

	case RTYPE_C_RefUV:
	    pKeySelectorCPD = rtREFUV_LCExtract (pKeyMapCPD, elementSelector);
	    break;

	default:
	    ERR_SignalFault (
		EC__InternalInconsistency,
		UTIL_FormatMessage (
		    "rtINDEX_Extract: Invalid Index Map Type: %s",
		    RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (pKeyMapCPD))
		)
	    );
	    break;
	}
	pKeyMapCPD->release ();
    }

/*****  Make the new key ... *****/
    DSC_Descriptor newKeyDsc; {
	M_CPD *pKeyStoreCPD	= pIndexCPD->GetCPD (rtINDEX_CPx_KeyStore);
	M_CPD *pKeySetCPD	= pIndexCPD->GetCPD (rtINDEX_CPx_KeyValues);
	DSC_Descriptor keyDsc;

	/*** Make a descriptor from the index's pKeyStoreCPD and pKeySetCPD ... ***/
	keyDsc.constructMonotype (pKeyStoreCPD, pKeySetCPD);
	pKeySetCPD->release ();

	/***  Extract using 'pKeySelectorCPD' ... ***/
	newKeyDsc.constructComposition (pKeySelectorCPD, keyDsc);
	pKeySelectorCPD->release ();

	keyDsc.clear ();
    }

    /***  Distribute if neccessary ... ***/
    if (elementDistribution)
	newKeyDsc.distribute (elementDistribution);

    /*** And make the new key ... ***/
    M_CPD *ptoken = newKeyDsc.PPT ();
    pKey = new rtINDEX_Key (ptoken, &newKeyDsc);
    ptoken->release ();

    newKeyDsc.clear ();
}


/**************************************
 **************************************
 *****  Operation Abstract Class  *****
 **************************************
 **************************************/

/************************
 *****  Definition  *****
 ************************/

class rtINDEX_Operation {
/*****  Delegation  *****/
protected:
    virtual char const *WhatIAmDoingDescription() const = 0;

/*****  Construction  *****/
protected:
    rtINDEX_Operation (M_CPD *pIndexCPD);

/*****  Destruction  *****/
    ~rtINDEX_Operation ();

/*****  Component Access Helpers  *****/
protected:
    M_CPD *IndexElementsCPD () const {
	return rtLSTORE_CPD_ContentCPD (m_pLStoreCPD);
    }
    M_CPD *IndexElementPTokenCPD () const {
	return rtLSTORE_CPD_ContentPTokenCPD (m_pLStoreCPD);
    }

/*****  Fault Generation Helpers  *****/
protected:
    void SignalKeySetTypeFault (char const* pWhereDescription) const;

    void SignalKeyMapTypeFault (char const* pWhereDescription) const;

/*****  State  *****/
protected:
    M_CPD *const m_pIndexCPD;
    M_CPD *m_pLStoreCPD;
    M_CPD *m_pKeySetCPD;
    M_CPD *m_pKeyMapCPD;
};


/**************************
 *****  Construction  *****
 **************************/

rtINDEX_Operation::rtINDEX_Operation (M_CPD *pIndexCPD)
: m_pIndexCPD (pIndexCPD)
{
/*****  Check the integrity of the index, ... *****/
    EnsureIndexKeyMeetsSetDefinition (m_pIndexCPD);

/*****  ... and unpack and cache its components:  *****/
    m_pLStoreCPD = rtINDEX_CPD_ListStoreCPD (m_pIndexCPD);
    m_pKeySetCPD = m_pIndexCPD->GetCPD (rtINDEX_CPx_KeyValues);
    m_pKeyMapCPD = m_pIndexCPD->GetCPD (rtINDEX_CPx_Map);
}

/*************************
 *****  Destruction  *****
 *************************/

rtINDEX_Operation::~rtINDEX_Operation () {
    m_pKeyMapCPD->release ();
    m_pKeySetCPD->release ();
    m_pLStoreCPD->release ();
}


/******************************
 *****  Fault Generation  *****
 ******************************/

void rtINDEX_Operation::SignalKeySetTypeFault (char const* pWhereDescription) const {
    ERR_SignalFault (
	EC__InternalInconsistency,
	UTIL_FormatMessage (
	    "rtINDEX::%s:%s: Invalid Key Set Type: %s",
	    WhatIAmDoingDescription (),
	    pWhereDescription,
	    RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (m_pKeySetCPD))
	)
    );
}

void rtINDEX_Operation::SignalKeyMapTypeFault (
    char const* pWhereDescription
) const
{
    ERR_SignalFault (
	EC__InternalInconsistency,
	UTIL_FormatMessage (
	    "rtINDEX::%s:%s: Invalid Key Map Type: %s",
	    WhatIAmDoingDescription (),
	    pWhereDescription,
	    RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (m_pKeyMapCPD))
	)
    );
}


/********************************************
 ********************************************
 *****  Keyed Operation Abstract Class  *****
 ********************************************
 ********************************************/

/*******************
 *****  Notes  *****
 *******************/

/*---------------------------------------------------------------------------
 *  The functions in this class that return references to the key-map
 *  co-domain (currently, LocateKeyAsKeyMapComainReference and
 *  ConvertKeySetReferenceToKeyMapComainReference) return their result via
 *  one of two mutually exclusive sets of reference variables.  Which set is
 *  used is determined by the key map representation. If the key map is a
 *  product mapped link, the result variable 'fKeyMapComainIsAProduct' is
 *  set to true and the co-domain reference is returned via the variables:
 *
 *	rtLINK_CType	*&pKeyMapComainReferenceLC
 *	M_CPD		*&pKeyMapComainDistribution
 *
 *  If the key map is a partitioned reference, 'fKeyMapComainIsAProduct' is
 *  set to false and the co-domain reference is returned via the variable:
 *
 *	M_CPD		*&pKeyMapComainReferenceCPD
 *
 *****/


/************************
 *****  Definition  *****
 ************************/

class rtINDEX_KeyedOperation : public rtINDEX_Operation {

/*****  Construction  *****/
protected:
    rtINDEX_KeyedOperation (
	DSC_Descriptor	&iResultDescriptor,
	DSC_Descriptor	&iIndexDescriptor,
	rtINDEX_Key	*pKey
    );

/*****  Component Access Helpers  *****/
protected:
    M_CPD *OperationPPT () const {
	return m_iIndexDescriptor.PPT ();
    }

    rtLINK_CType *OperationIndexReferenceLC () const {
	return DSC_Descriptor_Link (m_iIndexDescriptor);
    }

    DSC_Scalar& OperationIndexReference () const {
	return DSC_Descriptor_Scalar (m_iIndexDescriptor);
    }

    M_CPD *OperationKeySetReference () const {
	return rtINDEX_Key_Enumerator (m_pKey);
    }

    DSC_Descriptor& OperationKeySet () const {
	return rtINDEX_Key_Set (m_pKey);
    }

/*****  Locate Helpers  *****/
protected:
    void LocateKeyAsKeySetReference (
	rtLINK_LookupCase	xLookupCase,
	rtLINK_CType		*&pKeySetDomainReference,
	rtLINK_CType		*&pKeySetDomainReferenceGuard
    );

    int LocateKeyAsKeySetReference (
	rtLINK_LookupCase	xLookupCase,
	rtREFUV_Type_Reference&	iKeySetDomainReference
    );

    void LocateKeyAsKeyMapDomainReference (
	rtLINK_LookupCase	xLookupCase,
	rtLINK_CType		*&pKeyMapDomainReference,
	rtLINK_CType		*&pKeyMapDomainReferenceGuard,
	M_CPD*&			pKeyMapDistribution
    );

    int LocateKeyAsKeyMapDomainReference (
	rtLINK_LookupCase	xLookupCase,
	rtLINK_CType		*&pKeyMapDomainReference,
	rtLINK_CType		*&pKeyMapDomainReferenceGuard
    );

    int LocateKeyAsKeyMapDomainReference (
	rtLINK_LookupCase	xLookupCase,
	rtREFUV_Type_Reference&	iKeyMapDomainReference
    );

    void LocateKeyAsKeyMapComainReference (
	rtLINK_LookupCase	xLookupCase,
	bool		&fKeyMapComainIsAProduct,
	rtLINK_CType		*&pKeyMapComainReferenceLC,
	M_CPD			*&pKeyMapComainDistribution,
	M_CPD			*&pKeyMapComainReferenceCPD
    );

    int LocateKeyAsKeyMapComainReference (
	rtLINK_LookupCase	xLookupCase,
	bool		&fKeyMapComainIsAProduct,
	rtLINK_CType		*&pKeyMapComainReferenceLC,
	rtREFUV_Type_Reference	&iKeyMapComainReference
    );

    int LocateKeyAsKeyMapComainReference (
	rtLINK_LookupCase	xLookupCase,
	bool		&fKeyMapComainIsAProduct,
	rtREFUV_Type_Reference	&iKeyMapComainReference
    );

/*****  Conversion Helpers  *****/
protected:
    void ConvertKeySetReferenceToKeyMapDomainReference (
	rtLINK_LookupCase	xLookupCase,
	rtLINK_CType		*pKeySetDomainReference,
	rtLINK_CType		*pKeySetDomainReferenceGuard,
	rtLINK_CType		*&pKeyMapDomainReference,
	rtLINK_CType		*&pKeyMapDomainReferenceGuard,
	M_CPD			*&pKeyMapDistribution
    );

    void ConvertKeySetReferenceToKeyMapDomainReference (
	rtLINK_LookupCase	xLookupCase,
	rtREFUV_Type_Reference&	iKeySetDomainReference,
	rtLINK_CType		*&pKeyMapDomainReference,
	rtLINK_CType		*&pKeyMapDomainReferenceGuard
    );

    void ConvertKeySetReferenceToKeyMapDomainReference (
	rtLINK_LookupCase	xLookupCase,
	rtREFUV_Type_Reference&	iKeySetDomainReference,
	rtREFUV_Type_Reference&	iKeyMapDomainReference,
	bool		&fKeyMapDomainReferenceIsValid
    );

    void ConvertKeySetReferenceToKeyMapComainReference (
	rtLINK_CType		*pKeySetDomainReference,
	rtLINK_CType		*pKeySetDomainReferenceGuard,
	bool		&fKeyMapComainIsAProduct,
	rtLINK_CType		*&pKeyMapComainReferenceLC,
	M_CPD			*&pKeyMapComainDistribution,
	M_CPD			*&pKeyMapComainReferenceCPD
    );

    void ConvertKeySetReferenceToKeyMapComainReference (
	rtREFUV_Type_Reference&	iKeySetDomainReference,
	bool		&fKeyMapComainIsAProduct,
	rtLINK_CType		*&pKeyMapComainReferenceLC,
	rtREFUV_Type_Reference&	iKeyMapComainReference
    );

    void ConvertKeySetReferenceToKeyMapComainReference (
	rtREFUV_Type_Reference&	iKeySetDomainReference,
	bool		&fKeyMapComainIsAProduct,
	rtREFUV_Type_Reference&	iKeyMapComainReference
    );

/*****  Result Generation Helpers  *****/
protected:
    void SetResultToNilElementReference ();

    void SetResultToIntervalLimit (
	rtLINK_CType		*pKeyMapDomainReference,
	rtLINK_CType		*pKeyMapDomainReferenceGuard,
	M_CPD			*pKeyMapDistribution
    );

    void SetResultToIntervalLimit (
	rtREFUV_Type_Reference&	iKeyMapDomainReference
    );

    void SetResultToTimeZero ();

/*****  State  *****/
protected:
    DSC_Descriptor&	m_iResultDescriptor;
    DSC_Descriptor&	m_iIndexDescriptor;
    rtINDEX_Key *const	m_pKey;
};


/**************************
 *****  Construction  *****
 **************************/

rtINDEX_KeyedOperation::rtINDEX_KeyedOperation (
    DSC_Descriptor	&iResultDescriptor,
    DSC_Descriptor	&iIndexDescriptor,
    rtINDEX_Key		*pKey
)
: rtINDEX_Operation	(iIndexDescriptor.storeCPD ())
, m_iResultDescriptor	(iResultDescriptor)
, m_iIndexDescriptor	(iIndexDescriptor)
, m_pKey		(pKey)
{
/*****  Realize the key set, ...  *****/
    m_pKey->RealizeSet ();
}


/*******************************************
 *****  Key -> Key Set Locate Helpers  *****
 *******************************************/

void rtINDEX_KeyedOperation::LocateKeyAsKeySetReference (
    rtLINK_LookupCase		xLookupCase,
    rtLINK_CType		*&pKeySetDomainReference,
    rtLINK_CType		*&pKeySetDomainReferenceGuard
)
{
    switch ((RTYPE_Type)M_CPD_RType (m_pKeySetCPD)) {
    case RTYPE_C_Vector: {
	    DSC_Pointer locationsPointer;
	    DSC_Pointer foundPointer;
	    rtVECTOR_LookupFromValueD (
		m_pKeySetCPD,
		&OperationKeySet (),
		xLookupCase,
		&locationsPointer,
		&foundPointer
	    );
	    pKeySetDomainReferenceGuard = foundPointer.isEmpty ()
		? NilOf (rtLINK_CType*) : (rtLINK_CType*)DSC_Pointer_Link (foundPointer);
	    pKeySetDomainReference = DSC_Pointer_Link (locationsPointer);
	}
        break;
    case RTYPE_C_CharUV:
	rtCHARUV_Lookup (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_DoubleUV:
	rtDOUBLEUV_Lookup (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_FloatUV:
	rtFLOATUV_Lookup (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_IntUV:
	rtINTUV_Lookup (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_Unsigned64UV:
	rtU64UV_Lookup (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_Unsigned96UV:
	rtU96UV_Lookup (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_Unsigned128UV:
	rtU128UV_Lookup (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    default:
	SignalKeySetTypeFault ("LocateKeyAsKeySetReference[M]");
        break;
    }
}


int rtINDEX_KeyedOperation::LocateKeyAsKeySetReference (
    rtLINK_LookupCase		xLookupCase,
    rtREFUV_Type_Reference&	iKeySetDomainReference
)
{
    bool fKeySetDomainReferenceValid;
    if (RTYPE_C_Vector == (RTYPE_Type)M_CPD_RType (m_pKeySetCPD)) {
	DSC_Pointer locationsPointer;
	DSC_Pointer locatedPointer;
	rtVECTOR_LookupFromValueD (
	    m_pKeySetCPD,
	    &OperationKeySet (),
	    xLookupCase,
	    &locationsPointer,
	    &locatedPointer
	);
	fKeySetDomainReferenceValid = locationsPointer.isntEmpty ();
	iKeySetDomainReference = DSC_Pointer_Scalar (locationsPointer);
	locatedPointer.clear ();
    }
    else {
	int xKeySetDomainReference;
	switch ((RTYPE_Type)M_CPD_RType (m_pKeySetCPD)) {
	case RTYPE_C_CharUV:
	    fKeySetDomainReferenceValid = rtCHARUV_ScalarLookup (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Char (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_DoubleUV:
	    fKeySetDomainReferenceValid = rtDOUBLEUV_ScalarLookup (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Double (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_FloatUV:
	    fKeySetDomainReferenceValid = rtFLOATUV_ScalarLookup (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Float (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_IntUV:
	    fKeySetDomainReferenceValid = rtINTUV_ScalarLookup (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Int (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned64UV:
	    fKeySetDomainReferenceValid = rtU64UV_ScalarLookup (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Unsigned64 (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned96UV:
	    fKeySetDomainReferenceValid = rtU96UV_ScalarLookup (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Unsigned96 (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned128UV:
	    fKeySetDomainReferenceValid = rtU128UV_ScalarLookup (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Unsigned128 (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	default:
	    fKeySetDomainReferenceValid = false;
	    xKeySetDomainReference = 0;
	    SignalKeySetTypeFault ("LocateKeyAsKeySetReference[S]");
	    break;
	}

	if (fKeySetDomainReferenceValid)
	{
	    DSC_InitReferenceScalar (
		iKeySetDomainReference,
		UV_CPD_PosPTokenCPD (m_pKeySetCPD),
		xKeySetDomainReference
	    );
	}
    }

    return fKeySetDomainReferenceValid;
}


/**************************************************
 *****  Key -> Key Map Domain Locate Helpers  *****
 **************************************************/

void rtINDEX_KeyedOperation::LocateKeyAsKeyMapDomainReference (
    rtLINK_LookupCase		xLookupCase,
    rtLINK_CType		*&pKeyMapDomainReference,
    rtLINK_CType		*&pKeyMapDomainReferenceGuard,
    M_CPD			*&pKeyMapDistribution
)
{
    rtLINK_CType *pKeySetDomainReference;
    rtLINK_CType *pKeySetDomainReferenceGuard;
    LocateKeyAsKeySetReference (
	xLookupCase, pKeySetDomainReference, pKeySetDomainReferenceGuard
    );

    ConvertKeySetReferenceToKeyMapDomainReference (
	xLookupCase,
	pKeySetDomainReference,
	pKeySetDomainReferenceGuard,
	pKeyMapDomainReference,
	pKeyMapDomainReferenceGuard,
	pKeyMapDistribution
    );

    if (IsntNil (pKeySetDomainReferenceGuard))
	pKeySetDomainReferenceGuard->release ();
    pKeySetDomainReference->release ();
}

int rtINDEX_KeyedOperation::LocateKeyAsKeyMapDomainReference (
    rtLINK_LookupCase		xLookupCase,
    rtLINK_CType		*&pKeyMapDomainReference,
    rtLINK_CType		*&pKeyMapDomainReferenceGuard
)
{
    rtREFUV_Type_Reference iKeySetDomainReference;
    if (!LocateKeyAsKeySetReference (xLookupCase, iKeySetDomainReference))
	return false;

    ConvertKeySetReferenceToKeyMapDomainReference (
	xLookupCase,
	iKeySetDomainReference,
	pKeyMapDomainReference,
	pKeyMapDomainReferenceGuard
    );

    DSC_ClearScalar (iKeySetDomainReference);

    return true;
}

int rtINDEX_KeyedOperation::LocateKeyAsKeyMapDomainReference (
    rtLINK_LookupCase		xLookupCase,
    rtREFUV_Type_Reference&	iKeyMapDomainReference
)
{
    rtREFUV_Type_Reference iKeySetDomainReference;
    if (!LocateKeyAsKeySetReference (xLookupCase, iKeySetDomainReference))
	return false;

    bool fKeyMapDomainReferenceIsValid;
    ConvertKeySetReferenceToKeyMapDomainReference (
	xLookupCase,
	iKeySetDomainReference,
	iKeyMapDomainReference,
	fKeyMapDomainReferenceIsValid
    );

    DSC_ClearScalar (iKeySetDomainReference);

    return fKeyMapDomainReferenceIsValid;
}


/*****************************************************
 *****  Key -> Key Map Co-Domain Locate Helpers  *****
 *****************************************************/

void rtINDEX_KeyedOperation::LocateKeyAsKeyMapComainReference (
    rtLINK_LookupCase		xLookupCase,
    bool			&fKeyMapComainIsAProduct,
    rtLINK_CType		*&pKeyMapComainReferenceLC,
    M_CPD			*&pKeyMapComainDistribution,
    M_CPD			*&pKeyMapComainReferenceCPD
)
{
    rtLINK_CType *pKeySetDomainReference;
    rtLINK_CType *pKeySetDomainReferenceGuard;
    LocateKeyAsKeySetReference (
	xLookupCase, pKeySetDomainReference, pKeySetDomainReferenceGuard
    );

    ConvertKeySetReferenceToKeyMapComainReference (
	pKeySetDomainReference,
	pKeySetDomainReferenceGuard,
	fKeyMapComainIsAProduct,
	pKeyMapComainReferenceLC,
	pKeyMapComainDistribution,
	pKeyMapComainReferenceCPD
    );

    if (IsntNil (pKeySetDomainReferenceGuard))
	pKeySetDomainReferenceGuard->release ();
    pKeySetDomainReference->release ();
}


int rtINDEX_KeyedOperation::LocateKeyAsKeyMapComainReference (
    rtLINK_LookupCase		xLookupCase,
    bool			&fKeyMapComainIsAProduct,
    rtLINK_CType		*&pKeyMapComainReferenceLC,
    rtREFUV_Type_Reference&	iKeyMapComainReference
)
{
    rtREFUV_Type_Reference iKeySetDomainReference;
    if (!LocateKeyAsKeySetReference (xLookupCase, iKeySetDomainReference))
	return false;

    ConvertKeySetReferenceToKeyMapComainReference (
	iKeySetDomainReference,
	fKeyMapComainIsAProduct,
	pKeyMapComainReferenceLC,
	iKeyMapComainReference
    );

    DSC_ClearScalar (iKeySetDomainReference);

    return true;
}

int rtINDEX_KeyedOperation::LocateKeyAsKeyMapComainReference (
    rtLINK_LookupCase		xLookupCase,
    bool			&fKeyMapComainIsAProduct,
    rtREFUV_Type_Reference&	iKeyMapComainReference
)
{
    rtREFUV_Type_Reference iKeySetDomainReference;
    if (!LocateKeyAsKeySetReference (xLookupCase, iKeySetDomainReference))
	return false;

    ConvertKeySetReferenceToKeyMapComainReference (
	iKeySetDomainReference, fKeyMapComainIsAProduct, iKeyMapComainReference
    );

    DSC_ClearScalar (iKeySetDomainReference);

    return true;
}


/************************************************************
 *****  Key Set -> Key Map Domain Reference Conversion  *****
 ************************************************************/

void rtINDEX_KeyedOperation::ConvertKeySetReferenceToKeyMapDomainReference (
    rtLINK_LookupCase		xLookupCase,
    rtLINK_CType		*pKeySetDomainReference,
    rtLINK_CType		*pKeySetDomainReferenceGuard,
    rtLINK_CType		*&pKeyMapDomainReference,
    rtLINK_CType		*&pKeyMapDomainReferenceGuard,
    M_CPD			*&pKeyMapDomainDistribution
)
{
    bool		fKeyMapComainIsAProduct;
    rtLINK_CType	*pKeyMapComainReferenceLC;
    M_CPD		*pKeyMapComainReferenceCPD;

    ConvertKeySetReferenceToKeyMapComainReference (
	pKeySetDomainReference,
	pKeySetDomainReferenceGuard,
	fKeyMapComainIsAProduct,
	pKeyMapComainReferenceLC,
	pKeyMapDomainDistribution,
	pKeyMapComainReferenceCPD
    );

    if (fKeyMapComainIsAProduct) {
	rtLINK_LookupUsingLCKey (
	    m_pKeyMapCPD,
	    pKeyMapComainReferenceLC,
	    rtLINK_LookupCase_GT == xLookupCase ? rtLINK_LookupCase_GE : xLookupCase,
	    rtPTOKEN_CPD_BaseElementCount (pKeySetDomainReference->RPT ()),
	    &pKeyMapDomainReference,
	    &pKeyMapDomainReferenceGuard
	);
	pKeyMapComainReferenceLC->release ();
    }
    else {
	rtREFUV_PartitionedLocate (
	    m_pKeyMapCPD,
	    rtLSTORE_CPD_BreakpointArray (m_pLStoreCPD),
	    OperationIndexReferenceLC (),
	    pKeyMapComainReferenceCPD,
	    rtLINK_LookupCase_GT == xLookupCase ? rtLINK_LookupCase_GE : xLookupCase,
	    pKeyMapDomainReference,
	    pKeyMapDomainReferenceGuard,
	    pKeyMapDomainDistribution
	);
	pKeyMapComainReferenceCPD->release ();
    }
}


void rtINDEX_KeyedOperation::ConvertKeySetReferenceToKeyMapDomainReference (
    rtLINK_LookupCase		xLookupCase,
    rtREFUV_Type_Reference&	iKeySetDomainReference,
    rtLINK_CType		*&pKeyMapDomainReference,
    rtLINK_CType		*&pKeyMapDomainReferenceGuard
)
{
    bool			fKeyMapComainIsAProduct;
    rtLINK_CType		*pKeyMapComainReferenceLC;
    rtREFUV_Type_Reference	iKeyMapComainReference;

    ConvertKeySetReferenceToKeyMapComainReference (
	iKeySetDomainReference,
	fKeyMapComainIsAProduct,
	pKeyMapComainReferenceLC,
	iKeyMapComainReference
    );

    if (fKeyMapComainIsAProduct)
    {
	rtLINK_LookupUsingLCKey (
	    m_pKeyMapCPD,
	    pKeyMapComainReferenceLC,
	    rtLINK_LookupCase_GT == xLookupCase ? rtLINK_LookupCase_GE : xLookupCase,
	    rtPTOKEN_CPD_BaseElementCount (DSC_Scalar_RefPToken (iKeySetDomainReference)),
	    &pKeyMapDomainReference,
	    &pKeyMapDomainReferenceGuard
	);
	pKeyMapComainReferenceLC->release ();
    }
    else {
	rtREFUV_PartitionedLocate (
	    m_pKeyMapCPD,
	    rtLSTORE_CPD_BreakpointArray (m_pLStoreCPD),
	    OperationIndexReferenceLC (),
	    iKeyMapComainReference,
	    rtLINK_LookupCase_GT == xLookupCase ? rtLINK_LookupCase_GE : xLookupCase,
	    pKeyMapDomainReference,
	    pKeyMapDomainReferenceGuard
	);
	DSC_ClearScalar (iKeyMapComainReference);
    }
}


void rtINDEX_KeyedOperation::ConvertKeySetReferenceToKeyMapDomainReference (
    rtLINK_LookupCase		xLookupCase,
    rtREFUV_Type_Reference&	iKeySetDomainReference,
    rtREFUV_Type_Reference&	iKeyMapDomainReference,
    bool			&fKeyMapDomainReferenceIsValid
)
{
    bool			fKeyMapComainIsAProduct;
    rtREFUV_Type_Reference	iKeyMapComainReference;

    ConvertKeySetReferenceToKeyMapComainReference (
	iKeySetDomainReference, fKeyMapComainIsAProduct, iKeyMapComainReference
    );

    fKeyMapDomainReferenceIsValid = fKeyMapComainIsAProduct ? rtLINK_LookupUsingRefKey (
	m_pKeyMapCPD,
	&iKeyMapComainReference,
	rtLINK_LookupCase_GT == xLookupCase ? rtLINK_LookupCase_GE : xLookupCase,
	rtPTOKEN_CPD_BaseElementCount (DSC_Scalar_RefPToken (iKeySetDomainReference)),
	&iKeyMapDomainReference
    ) : rtREFUV_PartitionedLocate (
	m_pKeyMapCPD,
	rtLSTORE_CPD_BreakpointArray (m_pLStoreCPD),
	OperationIndexReference (),
	iKeyMapComainReference,
	rtLINK_LookupCase_GT == xLookupCase ? rtLINK_LookupCase_GE : xLookupCase,
	iKeyMapDomainReference
    );

    DSC_ClearScalar (iKeyMapComainReference);
}


/**************************************************************
 *****  Key Set -> Key Map Comain Reference Conversion  *****
 **************************************************************/

void rtINDEX_KeyedOperation::ConvertKeySetReferenceToKeyMapComainReference (
    rtLINK_CType		*pKeySetDomainReference,
    rtLINK_CType		*pKeySetDomainReferenceGuard,
    bool			&fKeyMapComainIsAProduct,
    rtLINK_CType		*&pKeyMapComainReferenceLC,
    M_CPD			*&pKeyMapComainDistribution,
    M_CPD			*&pKeyMapComainReferenceCPD
)
{
    if (IsntNil (pKeySetDomainReferenceGuard)) {
	M_CPD *pPartialKeySetReferences = rtLINK_Distribute (
	    pKeySetDomainReferenceGuard, pKeySetDomainReference
	);
    
	switch ((RTYPE_Type)M_CPD_RType (m_pKeyMapCPD)) {
	case RTYPE_C_Link:
	    fKeyMapComainIsAProduct = true;
	    rtLINK_AlignLink (m_pKeyMapCPD);
	    {
		M_CPD *pKeyMapComainPTokenCPD = rtLINK_CPD_RefPTokenCPD (m_pKeyMapCPD);

		pKeyMapComainReferenceCPD = rtLINK_LinearizeLCrUVic (
		    pKeyMapComainPTokenCPD,
		    OperationIndexReferenceLC (),
		    pPartialKeySetReferences,
		    OperationKeySetReference ()
		);
		pKeyMapComainReferenceLC = rtLINK_RefUVToConstructor (
		    pKeyMapComainReferenceCPD, &pKeyMapComainDistribution, true
		);
		pKeyMapComainReferenceCPD->release ();
		pKeyMapComainPTokenCPD->release ();

		pKeyMapComainReferenceCPD = NilOf (M_CPD*);
	    }
	    break;

	case RTYPE_C_RefUV:
	    fKeyMapComainIsAProduct	= false;
	    pKeyMapComainReferenceLC	= NilOf (rtLINK_CType *);
	    pKeyMapComainDistribution	= NilOf (M_CPD*);
	    pKeyMapComainReferenceCPD	= rtREFUV_UVExtract (
		pPartialKeySetReferences, OperationKeySetReference ()
	    );
	    break;

	default:
	    SignalKeyMapTypeFault ("ConvertKeySetReferenceToKeyMapComainReference");
	    break;
	}

	pPartialKeySetReferences->release ();
    }
    else switch ((RTYPE_Type)M_CPD_RType (m_pKeyMapCPD)) {
    case RTYPE_C_Link:
	fKeyMapComainIsAProduct = true;
	rtLINK_AlignLink (m_pKeyMapCPD); {
	    M_CPD *pKeyMapComainPTokenCPD = rtLINK_CPD_RefPTokenCPD (m_pKeyMapCPD);

	    pKeyMapComainReferenceCPD = rtLINK_LinearizeLCrLCic (
		pKeyMapComainPTokenCPD,
		OperationIndexReferenceLC (),
		pKeySetDomainReference,
		OperationKeySetReference ()
	    );

	    pKeyMapComainReferenceLC = rtLINK_RefUVToConstructor (
		pKeyMapComainReferenceCPD, &pKeyMapComainDistribution, true
	    );

	    pKeyMapComainReferenceCPD->release ();
	    pKeyMapComainPTokenCPD->release ();

	    pKeyMapComainReferenceCPD = NilOf (M_CPD*);
	}
	break;

    case RTYPE_C_RefUV:
	fKeyMapComainIsAProduct   = false;
	pKeyMapComainReferenceLC  = NilOf (rtLINK_CType *);
	pKeyMapComainDistribution = NilOf (M_CPD*);
	pKeyMapComainReferenceCPD = pKeySetDomainReference->Extract (
	    OperationKeySetReference ()
	);
	break;

    default:
	SignalKeyMapTypeFault ("ConvertKeySetReferenceToKeyMapComainReference");
	break;
    }
}


void rtINDEX_KeyedOperation::ConvertKeySetReferenceToKeyMapComainReference (
    rtREFUV_Type_Reference&	iKeySetDomainReference,
    bool			&fKeyMapComainIsAProduct,
    rtLINK_CType		*&pKeyMapComainReferenceLC,
    rtREFUV_Type_Reference&	iKeyMapComainReference
)
{
    switch ((RTYPE_Type)M_CPD_RType (m_pKeyMapCPD))
    {
    case RTYPE_C_Link:
	fKeyMapComainIsAProduct = true;
	rtLINK_AlignLink (m_pKeyMapCPD);
	{
	    M_CPD *pKeyMapComainPTokenCPD = rtLINK_CPD_RefPTokenCPD (m_pKeyMapCPD);

	    pKeyMapComainReferenceLC = rtLINK_LinearizeLCrRc (
		pKeyMapComainPTokenCPD,
		OperationIndexReferenceLC (),
		&iKeySetDomainReference
	    );

	    pKeyMapComainPTokenCPD->release ();
	}
	break;

    case RTYPE_C_RefUV:
	fKeyMapComainIsAProduct = false;
	pKeyMapComainReferenceLC = NilOf (rtLINK_CType *);
	DSC_DuplicateScalar (iKeyMapComainReference, iKeySetDomainReference);
	break;

    default:
	SignalKeyMapTypeFault ("ConvertKeySetReferenceToKeyMapComainReference");
	break;
    }
}


void rtINDEX_KeyedOperation::ConvertKeySetReferenceToKeyMapComainReference (
    rtREFUV_Type_Reference&	iKeySetDomainReference,
    bool			&fKeyMapComainIsAProduct,
    rtREFUV_Type_Reference&	iKeyMapComainReference
)
{
    switch ((RTYPE_Type)M_CPD_RType (m_pKeyMapCPD))
    {
    case RTYPE_C_Link:
	fKeyMapComainIsAProduct = true;
	rtLINK_AlignLink (m_pKeyMapCPD);
	{
	    M_CPD *pKeyMapComainPTokenCPD = rtLINK_CPD_RefPTokenCPD (m_pKeyMapCPD);

	    rtREFUV_LinearizeRrRc (
		&iKeyMapComainReference,
		pKeyMapComainPTokenCPD,
		&OperationIndexReference (),
		&iKeySetDomainReference
	    );

	    pKeyMapComainPTokenCPD->release ();
	}
	break;

    case RTYPE_C_RefUV:
	fKeyMapComainIsAProduct = false;
	DSC_DuplicateScalar (iKeyMapComainReference, iKeySetDomainReference);
	break;

    default:
	SignalKeyMapTypeFault ("ConvertKeySetReferenceToKeyMapComainReference");
	break;
    }
}


/****************************************************
 *****  Nil Element Reference Result Generator  *****
 ****************************************************/

void rtINDEX_KeyedOperation::SetResultToNilElementReference ()
{
    M_CPD *pPosPTokenCPD = OperationPPT ();
    M_CPD *pRefPTokenCPD = IndexElementPTokenCPD ();

    m_iResultDescriptor.constructReferenceConstant (
	pPosPTokenCPD,
	IndexElementsCPD (),
	pRefPTokenCPD,
	rtPTOKEN_CPD_BaseElementCount (pRefPTokenCPD)
    );

    pPosPTokenCPD->release ();
}


/**********************************************
 *****  Interval Limit Result Generators  *****
 **********************************************/

void rtINDEX_KeyedOperation::SetResultToIntervalLimit (
    rtLINK_CType	*pKeyMapDomainReference,
    rtLINK_CType	*pKeyMapDomainReferenceGuard,
    M_CPD		*pKeyMapDistribution
)
{
    LookupMListLimitCount++;

    M_CPD *pKeySetDomainReference;
    switch ((RTYPE_Type)M_CPD_RType (m_pKeyMapCPD))
    {
    case RTYPE_C_Link:
	{
	    rtLINK_CType *pKeyMapComainReferences = rtLINK_LCExtract (
		m_pKeyMapCPD, pKeyMapDomainReference
	    );
	    pKeySetDomainReference = rtLINK_ColumnProjection (pKeyMapComainReferences);
	    pKeyMapComainReferences->release ();
	}
	break;

    case RTYPE_C_RefUV:
	pKeySetDomainReference = rtREFUV_LCExtract (m_pKeyMapCPD, pKeyMapDomainReference);
	break;

    default:
	SignalKeyMapTypeFault ("SetResultToIntervalLimit");
	break;
    }

    if (IsntNil (pKeyMapDomainReferenceGuard)) {
	M_CPD *pTempCPD = pKeySetDomainReference;
	rtREFUV_LCDistribute (pKeySetDomainReference, pKeyMapDomainReferenceGuard, pTempCPD);
	pTempCPD->release ();
    }

    {
	DSC_Descriptor iKeySetDescriptor;
	iKeySetDescriptor.constructMonotype (
	    m_pIndexCPD->GetCPD (rtINDEX_CPx_KeyStore), m_pKeySetCPD
	);
	m_iResultDescriptor.constructComposition (pKeySetDomainReference, iKeySetDescriptor);
	iKeySetDescriptor.clear ();
    }

    if (pKeyMapDistribution)
        m_iResultDescriptor.distribute (pKeyMapDistribution);

    pKeySetDomainReference->release ();

}


void rtINDEX_KeyedOperation::SetResultToIntervalLimit (
    rtREFUV_Type_Reference &iKeyMapDomainReference
) {
    LookupSListLimitCount++;

    rtREFUV_Type_Reference iKeySetDomainReference;
    switch ((RTYPE_Type)M_CPD_RType (m_pKeyMapCPD)) {
    case RTYPE_C_Link: {
	    rtREFUV_Type_Reference iKeyMapComainReference;
	    rtLINK_RefExtract (
		&iKeyMapComainReference, m_pKeyMapCPD, &iKeyMapDomainReference
	    );

	    M_CPD *pKeySetDomainPTokenCPD = rtPTOKEN_CPD_ColPTokenCPD (
		DSC_Scalar_RefPToken (iKeyMapComainReference)
	    );

	    DSC_InitReferenceScalar (
		iKeySetDomainReference,
		pKeySetDomainPTokenCPD,
		DSC_Scalar_Int (
		    iKeyMapComainReference
		) % rtPTOKEN_CPD_BaseElementCount (pKeySetDomainPTokenCPD)
	    );
	    DSC_ClearScalar (iKeyMapComainReference);
	}
	break;

    case RTYPE_C_RefUV:
	rtREFUV_RFExtract (&iKeySetDomainReference, m_pKeyMapCPD, &iKeyMapDomainReference);
	break;

    default:
	SignalKeyMapTypeFault ("SetResultToIntervalLimit");
	break;
    }

    {
	DSC_Descriptor iKeySetDescriptor;
	iKeySetDescriptor.constructMonotype (
	    m_pIndexCPD->GetCPD (rtINDEX_CPx_KeyStore), m_pKeySetCPD
	);
	m_iResultDescriptor.constructComposition (iKeySetDomainReference, iKeySetDescriptor);
	iKeySetDescriptor.clear ();
    }

    DSC_ClearScalar (iKeySetDomainReference);
}


/****************************************
 *****  Time Zero Result Generator  *****
 ****************************************/

void rtINDEX_KeyedOperation::SetResultToTimeZero () {
    M_CPD *pPosPTokenCPD = OperationPPT ();

    m_iResultDescriptor.constructZero (
	m_pIndexCPD->GetCPD (rtINDEX_CPx_KeyStore),
	(RTYPE_Type)M_CPD_RType (m_pKeySetCPD),
	pPosPTokenCPD,
	UV_CPD_RefPTokenCPD (m_pKeySetCPD)
    );

    pPosPTokenCPD->release ();
}


/****************************************************
 ****************************************************
 *****  Keyed Element Operation Abstract Class  *****
 ****************************************************
 ****************************************************/

/************************
 *****  Definition  *****
 ************************/

class rtINDEX_KeyedElementOperation : public rtINDEX_KeyedOperation {

/*****  Operation  *****/
public:
    void operate ();

/*****  Delegation  *****/
protected:
    virtual void ProcessRecursiveCase (rtINDEX_Key *pKey) = 0;

    virtual void ProcessMKMLCase () = 0;

    virtual void ProcessSKMLCase () = 0;

    virtual void ProcessSKSLCase () = 0;

    virtual void ProduceNAResult () = 0;

/*****  Construction  *****/
    rtINDEX_KeyedElementOperation (
	DSC_Descriptor	&iResultDescriptor,
	DSC_Descriptor	&iIndexDescriptor,
	rtINDEX_Key	*pKey
    ) : rtINDEX_KeyedOperation (iResultDescriptor, iIndexDescriptor, pKey)
    {
    }
};


/***********************
 *****  Operation  *****
 ***********************/

void rtINDEX_KeyedElementOperation::operate () {
/*****  Decode the arguments ... *****/
    if (m_iIndexDescriptor.holdsAScalarReference ()) {
	if (m_pKey->isScalar ())
	    ProcessSKSLCase ();
	else
	    ProduceNAResult ();
    }

    else if (m_iIndexDescriptor.holdsALink ()) {
	if (m_pKey->isScalar ())
	    ProcessSKMLCase ();
	else
	    ProcessMKMLCase ();
    }

    else if (m_iIndexDescriptor.holdsNonScalarReferences ()) {
	M_CPD *pDistribution = m_iIndexDescriptor.factor ();
	if (pDistribution) {
	    rtINDEX_Key *newKey = m_pKey->NewFutureKey (
		NilOf (rtLINK_CType *), pDistribution
	    );

	    ProcessRecursiveCase (newKey);

	    /* reset 'm_pIndexDescriptor' before exiting */
	    m_iIndexDescriptor.distribute (pDistribution);
	    m_iResultDescriptor.distribute (pDistribution);
	    pDistribution->release ();
	    newKey->release ();
	}
	else if (m_pKey->isScalar ())
	    ProcessSKMLCase ();
	else
	    ProcessMKMLCase ();
    }

    else if (m_iIndexDescriptor.holdsValuesInAnyForm ())
	ProduceNAResult ();
    else ERR_SignalFault (
	EC__InternalInconsistency,
	UTIL_FormatMessage (
	    "rtINDEX::%s: Invalid Index Descriptor", WhatIAmDoingDescription ()
	)
    );
}


/*******************************************************************
 *******************************************************************
 *****  LocateOrAdd Class and 'rtINDEX_Define' Implementation  *****
 *******************************************************************
 *******************************************************************/

/************************
 *****  Definition  *****
 ************************/

class rtINDEX_LocateOrAddOperation : public rtINDEX_KeyedElementOperation {

/*****  Construction  *****/
public:
    rtINDEX_LocateOrAddOperation (
	DSC_Descriptor	&iResultDescriptor,
	DSC_Descriptor	&iIndexDescriptor,
	rtINDEX_Key	*pKey
    ) : rtINDEX_KeyedElementOperation (iResultDescriptor, iIndexDescriptor, pKey)
    {
    }

/*****  Delegation  *****/
protected:
    virtual char const *WhatIAmDoingDescription() const;

    virtual void ProcessRecursiveCase (rtINDEX_Key *pKey);

    virtual void ProcessMKMLCase ();

    virtual void ProcessSKMLCase ();

    virtual void ProcessSKSLCase ();

    virtual void ProduceNAResult ();

/*****  Helpers  *****/
    void DefineScalarKey (
	rtREFUV_Type_Reference&	xKeySetDomainReference
    );
};


/**************************************************************************************
 *****  'WhatIAmDoingDescription' and 'ProcessRecursiveCase' Delegation Handlers  *****
 **************************************************************************************/

char const *rtINDEX_LocateOrAddOperation::WhatIAmDoingDescription() const {
    return "LocateOrAdd";
}

void rtINDEX_LocateOrAddOperation::ProcessRecursiveCase (rtINDEX_Key *pKey) {
    rtINDEX_Define (&m_iResultDescriptor, &m_iIndexDescriptor, pKey);
}


/*************************************
 *****  MKML Delegation Handler  *****
 *************************************/

void rtINDEX_LocateOrAddOperation::ProcessMKMLCase () {
    DefineMKeyMListCount++;

/*****  ... update the key set, ... *****/
    rtLINK_CType *pKeySetDomainReference;
    switch ((RTYPE_Type)M_CPD_RType (m_pKeySetCPD)) {
    case RTYPE_C_Vector: {
	    DSC_Pointer locationsPointer;
	    rtVECTOR_LocateOrAdd (m_pKeySetCPD, &OperationKeySet (), &locationsPointer);
	    pKeySetDomainReference = DSC_Pointer_Link (locationsPointer);
	}
	break;
    case RTYPE_C_CharUV:
	rtCHARUV_LocateOrAdd (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_DoubleUV:
	rtDOUBLEUV_LocateOrAdd (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_FloatUV:
	rtFLOATUV_LocateOrAdd (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_IntUV:
	rtINTUV_LocateOrAdd (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_Unsigned64UV:
	rtU64UV_LocateOrAdd (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_Unsigned96UV:
	rtU96UV_LocateOrAdd (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_Unsigned128UV:
	rtU128UV_LocateOrAdd (
	    m_pKeySetCPD,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    default:
	SignalKeySetTypeFault ("MKML");
	break;
    }

/*****  ... update the key map, ... *****/
    rtLINK_CType	*pKeyMapDomainReference;
    M_CPD		*pKeyMapDomainReferenceDistribution; {
	bool		 fKeyMapComainIsAProduct;
	rtLINK_CType	*pKeyMapComainReferenceLC;
	M_CPD		*pKeyMapComainReferenceCPD;

	ConvertKeySetReferenceToKeyMapComainReference (
	    pKeySetDomainReference,
	    NilOf (rtLINK_CType *),
	    fKeyMapComainIsAProduct,
	    pKeyMapComainReferenceLC,
	    pKeyMapDomainReferenceDistribution,
	    pKeyMapComainReferenceCPD
	);

	rtLINK_CType *pKeyMapDomainReferenceAdditions;
	if (fKeyMapComainIsAProduct) {
	    pKeyMapDomainReference = rtLINK_LocateOrAddFromLC (
		m_pKeyMapCPD, pKeyMapComainReferenceLC, &pKeyMapDomainReferenceAdditions
	    );
	    pKeyMapComainReferenceLC->release ();
	}
	else {
	    rtREFUV_PartitionedLocateOrAdd (
		m_pKeyMapCPD,
		rtLSTORE_CPD_BreakpointArray (m_pLStoreCPD),
		OperationIndexReferenceLC (),
		pKeyMapComainReferenceCPD,
		pKeyMapDomainReference,
		pKeyMapDomainReferenceAdditions,
		pKeyMapDomainReferenceDistribution
	    );
	    pKeyMapComainReferenceCPD->release ();
	}

/*****  ... align the l-store if anything was added, ... *****/
	if (IsntNil (pKeyMapDomainReferenceAdditions)) {
	    M_CPD *pElementAddCounts = rtLINK_LCCountReferences (
		pKeyMapDomainReferenceAdditions,
		m_pLStoreCPD->TheIntegerClass ().PTokenCPD (),
		pKeyMapDomainReferenceDistribution
	    );
	    rtLSTORE_AlignUsingLCSelctLists (
		m_pLStoreCPD, OperationIndexReferenceLC (), pElementAddCounts
	    );
	    pElementAddCounts->release ();
	    pKeyMapDomainReferenceAdditions->release ();
	}
    }
    pKeySetDomainReference->release ();

/*****  ... and initialize the result descriptor.  *****/
    m_iResultDescriptor.constructReference (
	IndexElementsCPD (), pKeyMapDomainReferenceDistribution, pKeyMapDomainReference
    );
}


/*************************************
 *****  SKML Delegation Handler  *****
 *************************************/

void rtINDEX_LocateOrAddOperation::ProcessSKMLCase ()
{
    DefineSKeyMListCount++;

/*****  ... update the key set, ... *****/
    rtREFUV_Type_Reference keySelectorRef;
    DefineScalarKey (keySelectorRef);

/*****  ... update the key map, ... *****/
    rtLINK_CType *pKeyMapDomainReferences; {
	rtLINK_CType *pKeyMapDomainAdditions;
	switch ((RTYPE_Type)M_CPD_RType (m_pKeyMapCPD))
	{
	case RTYPE_C_Link:
	    rtLINK_AlignLink (m_pKeyMapCPD);
	    {
		M_CPD *pKeyMapComainPTokenCPD = rtLINK_CPD_RefPTokenCPD (m_pKeyMapCPD);
		rtLINK_CType *pKeyMapComainReferenceLC = rtLINK_LinearizeLCrRc (
		    pKeyMapComainPTokenCPD, OperationIndexReferenceLC (), &keySelectorRef
		);
		pKeyMapComainPTokenCPD->release ();

		pKeyMapDomainReferences = rtLINK_LocateOrAddFromLC (
		    m_pKeyMapCPD, pKeyMapComainReferenceLC, &pKeyMapDomainAdditions
		);

		pKeyMapComainReferenceLC->release ();
	    }
	    break;

	case RTYPE_C_RefUV:
	    rtREFUV_PartitionedLocateOrAdd (
		m_pKeyMapCPD,
		rtLSTORE_CPD_BreakpointArray (m_pLStoreCPD),
		OperationIndexReferenceLC (),
		keySelectorRef,
		pKeyMapDomainReferences,
		pKeyMapDomainAdditions
	    );
	    break;

	default:
	    SignalKeyMapTypeFault ("SKML");
	    break;
	}

/*****  ... align the l-store if anything was added, ... *****/
	if (IsntNil (pKeyMapDomainAdditions)) {
	    M_CPD *pElementAddCounts = rtLINK_LCCountReferences (
		pKeyMapDomainAdditions,
		m_pLStoreCPD->TheIntegerClass ().PTokenCPD (),
		NilOf (M_CPD*)
	    );
	    rtLSTORE_AlignUsingLCSelctLists (
		m_pLStoreCPD, OperationIndexReferenceLC (), pElementAddCounts
	    );
	    pElementAddCounts->release ();
	    pKeyMapDomainAdditions->release ();
	}
    }
    DSC_ClearScalar (keySelectorRef);

/*****  ... and initialize the result descriptor.  *****/
    m_iResultDescriptor.constructLink (IndexElementsCPD (), pKeyMapDomainReferences);
}


/*************************************
 *****  SKSL Delegation Handler  *****
 *************************************/

void rtINDEX_LocateOrAddOperation::ProcessSKSLCase ()
{
    DefineSKeySListCount++;

/*****  ... update the key set, ... *****/
    rtREFUV_Type_Reference keySelectorRef;
    DefineScalarKey (keySelectorRef);

/*****  ... update the key map, ... *****/
    rtREFUV_Type_Reference elementSelectorRef; {
	bool added;
	switch ((RTYPE_Type)M_CPD_RType (m_pKeyMapCPD)) {
	case RTYPE_C_Link:
	    rtLINK_AlignLink (m_pKeyMapCPD); {
		M_CPD *pKeyMapComainPTokenCPD = rtLINK_CPD_RefPTokenCPD (m_pKeyMapCPD);

		rtREFUV_Type_Reference cartesianKeyRef;
		rtREFUV_LinearizeRrRc (
		    &cartesianKeyRef,
		    pKeyMapComainPTokenCPD,
		    &OperationIndexReference (),
		    &keySelectorRef
		);
		pKeyMapComainPTokenCPD->release ();

		added = rtLINK_LocateOrAddFromRef (
		    m_pKeyMapCPD, &cartesianKeyRef, &elementSelectorRef
		);
		DSC_ClearScalar (cartesianKeyRef);
	    }
	    break;

	case RTYPE_C_RefUV:
	    rtREFUV_PartitionedLocateOrAdd (
		m_pKeyMapCPD,
		rtLSTORE_CPD_BreakpointArray (m_pLStoreCPD),
		OperationIndexReference (),
		keySelectorRef,
		elementSelectorRef,
		added
	    );
	    break;

	default:
	    SignalKeyMapTypeFault ("SKSL");
	    break;
	}

/*****  ... align the lstore if anything was added... *****/
	if (added) rtLSTORE_AlignUsingRefSelctList (
	    m_pLStoreCPD, &OperationIndexReference (), 1
	);
    }
    DSC_ClearScalar (keySelectorRef);

/*****  ... and initialize the result descriptor. *****/
    m_iResultDescriptor.constructScalar (IndexElementsCPD (), elementSelectorRef);
}


/******************************************
 *****  NA Result Generation Handler  *****
 ******************************************/

void rtINDEX_LocateOrAddOperation::ProduceNAResult ()
{
    SetResultToNilElementReference ();
}


/******************************************
 *****  Scalar Key Definition Helper  *****
 ******************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to locate or add a scalar key to a key set.
 *
 *  Arguments:
 *	xKeySetDomainReference	- the address of a scalar reference object
 *				  which will be initialized to reference the
 *				  location of the query key in the key set.
 *---------------------------------------------------------------------------
 */
void rtINDEX_LocateOrAddOperation::DefineScalarKey (
    rtREFUV_Type_Reference&	rKeySetDomainReference
)
{
    if (RTYPE_C_Vector == M_CPD_RType (m_pKeySetCPD)) {
	DSC_Pointer locationPointer;
	rtVECTOR_LocateOrAdd (m_pKeySetCPD, &OperationKeySet (), &locationPointer);
	rKeySetDomainReference = DSC_Pointer_Scalar (locationPointer);
    }
    else {
	int xKeySetDomainReference;
	switch (M_CPD_RType (m_pKeySetCPD)) {
	case RTYPE_C_CharUV:
	    rtCHARUV_ScalarLocateOrAdd (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Char (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_DoubleUV:
	    rtDOUBLEUV_ScalarLocateOrAdd (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Double (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_FloatUV:
	    rtFLOATUV_ScalarLocateOrAdd (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Float (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_IntUV:
	    rtINTUV_ScalarLocateOrAdd (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Int (OperationKeySet ()),
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned64UV:
	    rtU64UV_ScalarLocateOrAdd (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Unsigned64 (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned96UV:
	    rtU96UV_ScalarLocateOrAdd (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Unsigned96 (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned128UV:
	    rtU128UV_ScalarLocateOrAdd (
		m_pKeySetCPD,
		&DSC_Descriptor_Scalar_Unsigned128 (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	default:
	    SignalKeySetTypeFault ("DefineScalarKey");
	    break;
	}
	DSC_InitReferenceScalar (
	    rKeySetDomainReference,
	    rtPTOKEN_BasePToken (m_pKeySetCPD, UV_CPx_PToken),
	    xKeySetDomainReference
	);
    }
}


/*********************************************
 *****  'rtINDEX_Define' Implementation  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to define new point(s) in an index.
 *
 *  Arguments:
 *	pResultDescriptor	- the address of a descriptor which will be initialized
 *				  to reference the elements found or added.
 *	pIndexDescriptor	- the address of a descriptor referencing the affected
 *				  lists.
 *	pKey			- the address of a 'rtINDEX_Key' containing the keys
 *				  to add.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtINDEX_Define (
    DSC_Descriptor *pResultDescriptor, DSC_Descriptor *pIndexDescriptor, rtINDEX_Key *pKey
) {
    rtINDEX_LocateOrAddOperation operation (*pResultDescriptor, *pIndexDescriptor, pKey);
    operation.operate ();
}


/*************************************************************
 *************************************************************
 *****  Locate Class and 'rtINDEX_Lookup' Implementation *****
 *************************************************************
 *************************************************************/

/************************
 *****  Definition  *****
 ************************/

class rtINDEX_LocateOperation : public rtINDEX_KeyedElementOperation {

/*****  Construction  *****/
public:
    rtINDEX_LocateOperation (
	DSC_Descriptor		&iResultDescriptor,
	DSC_Descriptor		&iIndexDescriptor,
	rtINDEX_Key		*pKey,
	rtLINK_LookupCase	 xLookupCase
    )
    : rtINDEX_KeyedElementOperation (iResultDescriptor, iIndexDescriptor, pKey)
    , m_xLookupCase (xLookupCase)
    {
    }

/*****  Delegation  *****/
protected:
    virtual char const *WhatIAmDoingDescription() const;

    virtual void ProcessRecursiveCase (rtINDEX_Key *pKey);

    virtual void ProcessMKMLCase ();

    virtual void ProcessSKMLCase ();

    virtual void ProcessSKSLCase ();

    virtual void ProduceNAResult ();

/*****  State  *****/
protected:
    rtLINK_LookupCase const m_xLookupCase;
};


/**************************************************************************************
 *****  'WhatIAmDoingDescription' and 'ProcessRecursiveCase' Delegation Handlers  *****
 **************************************************************************************/

char const *rtINDEX_LocateOperation::WhatIAmDoingDescription() const {
    return "Locate";
}

void rtINDEX_LocateOperation::ProcessRecursiveCase (rtINDEX_Key *pKey) {
    rtINDEX_Lookup (&m_iResultDescriptor, &m_iIndexDescriptor, pKey, m_xLookupCase);
}


/*************************************
 *****  MKML Delegation Handler  *****
 *************************************/

void rtINDEX_LocateOperation::ProcessMKMLCase ()
{
    LookupMKeyMListCount++;

    rtLINK_CType *pKeyMapDomainReference;
    rtLINK_CType *pKeyMapDomainReferenceGuard;
    M_CPD	 *pKeyMapDistribution;
    LocateKeyAsKeyMapDomainReference (
	m_xLookupCase, pKeyMapDomainReference, pKeyMapDomainReferenceGuard, pKeyMapDistribution
    );

    if (IsntNil (pKeyMapDomainReferenceGuard)) {
	M_CPD *pKeyMapReferenceUV = rtLINK_Distribute (
	    pKeyMapDomainReferenceGuard, pKeyMapDomainReference
	);
	pKeyMapDomainReferenceGuard->release ();
	pKeyMapDomainReference->release ();

	m_iResultDescriptor.constructMonotype (IndexElementsCPD (), pKeyMapReferenceUV);
	m_iResultDescriptor.distribute (pKeyMapDistribution);

	pKeyMapReferenceUV->release ();
	pKeyMapDistribution->release ();
    }
    else m_iResultDescriptor.constructReference (
	IndexElementsCPD (), pKeyMapDistribution, pKeyMapDomainReference
    );
}


/*************************************
 *****  SKML Delegation Handler  *****
 *************************************/

void rtINDEX_LocateOperation::ProcessSKMLCase ()
{
    LookupSKeyMListCount++;

    rtLINK_CType *pKeyMapDomainReference;
    rtLINK_CType *pKeyMapDomainReferenceGuard;
    if (!LocateKeyAsKeyMapDomainReference (
	    m_xLookupCase, pKeyMapDomainReference, pKeyMapDomainReferenceGuard
	)
    ) ProduceNAResult ();
    else if (IsntNil (pKeyMapDomainReferenceGuard)) {
	M_CPD *pKeyMapReferenceUV = rtLINK_Distribute (
	    pKeyMapDomainReferenceGuard, pKeyMapDomainReference
	);
	pKeyMapDomainReference->release ();
	pKeyMapDomainReferenceGuard->release ();

	m_iResultDescriptor.constructMonotype (IndexElementsCPD (), pKeyMapReferenceUV);
	pKeyMapReferenceUV->release ();
    }
    else {
	m_iResultDescriptor.constructLink (IndexElementsCPD (), pKeyMapDomainReference);
    }
}


/*************************************
 *****  SKSL Delegation Handler  *****
 *************************************/

void rtINDEX_LocateOperation::ProcessSKSLCase ()
{
    LookupSKeySListCount++;

    rtREFUV_Type_Reference iKeyMapDomainReference;
    if (LocateKeyAsKeyMapDomainReference (m_xLookupCase, iKeyMapDomainReference))
	m_iResultDescriptor.constructScalar (IndexElementsCPD (), iKeyMapDomainReference);
    else
	ProduceNAResult ();
}


/**********************************
 *  NA Result Generation Handler  *
 **********************************/

void rtINDEX_LocateOperation::ProduceNAResult ()
{
    SetResultToNilElementReference ();
}


/*********************************************
 *****  'rtINDEX_Lookup' Implementation  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Routine to lookup point(s) in an index.
 *
 *  Arguments:
 *	pResultDescriptor	- the address of a descriptor which will be initialized
 *				  to reference the elements found.
 *	pIndexDescriptor	- the address of a descriptor which names the lists
 *				  to search.
 *	pKey			- a 'rtINDEX_Key *' containing the keys to locate.
 *	comparisonType		- a 'rtLINK_LookupCase' enumeration value specifying
 *				  the index/query key comparison type.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtINDEX_Lookup (
    DSC_Descriptor	*pResultDescriptor,
    DSC_Descriptor	*pIndexDescriptor,
    rtINDEX_Key		*pKey,
    rtLINK_LookupCase	 comparisonType
)
{
    rtINDEX_LocateOperation operation (
	*pResultDescriptor, *pIndexDescriptor, pKey, comparisonType
    );

    operation.operate ();
}


/******************************************************************************
 ******************************************************************************
 *****  Interval Limit Class and 'rtINDEX_IntervalLimits' Implementation  *****
 ******************************************************************************
 ******************************************************************************/

/************************
 *****  Definition  *****
 ************************/

class rtINDEX_IntervalLimitOperation : public rtINDEX_KeyedElementOperation {

/*****  Construction  *****/
public:
    rtINDEX_IntervalLimitOperation (
	DSC_Descriptor		&iResultDescriptor,
	DSC_Descriptor		&iIndexDescriptor,
	rtINDEX_Key		*pKey,
	rtLINK_LookupCase	 xLookupCase
    )
    : rtINDEX_KeyedElementOperation (iResultDescriptor, iIndexDescriptor, pKey)
    , m_xLookupCase (xLookupCase)
    {
    }

/*****  Delegation  *****/
protected:
    virtual char const *WhatIAmDoingDescription() const;

    virtual void ProcessRecursiveCase (rtINDEX_Key *pKey);

    virtual void ProcessMKMLCase ();

    virtual void ProcessSKMLCase ();

    virtual void ProcessSKSLCase ();

    virtual void ProduceNAResult ();

/*****  State  *****/
protected:
    rtLINK_LookupCase const m_xLookupCase;
};


/**************************************************************************************
 *****  'WhatIAmDoingDescription' and 'ProcessRecursiveCase' Delegation Handlers  *****
 **************************************************************************************/

char const *rtINDEX_IntervalLimitOperation::WhatIAmDoingDescription() const {
    return "IntervalLimit";
}

void rtINDEX_IntervalLimitOperation::ProcessRecursiveCase (rtINDEX_Key *pKey) {
    rtINDEX_IntervalLimits (&m_iResultDescriptor, &m_iIndexDescriptor, pKey, m_xLookupCase);
}


/*************************************
 *****  MKML Delegation Handler  *****
 *************************************/

void rtINDEX_IntervalLimitOperation::ProcessMKMLCase () {
    rtLINK_CType *pKeyMapDomainReference;
    rtLINK_CType *pKeyMapDomainReferenceGuard;
    M_CPD	 *pKeyMapDistribution;
    LocateKeyAsKeyMapDomainReference (
	m_xLookupCase,
	pKeyMapDomainReference,
	pKeyMapDomainReferenceGuard,
	pKeyMapDistribution
    );

    SetResultToIntervalLimit (
	pKeyMapDomainReference, pKeyMapDomainReferenceGuard, pKeyMapDistribution
    );

    pKeyMapDomainReference->release ();
    if (pKeyMapDomainReferenceGuard)
	pKeyMapDomainReferenceGuard->release ();
    pKeyMapDistribution->release ();
}


/*************************************
 *****  SKML Delegation Handler  *****
 *************************************/

void rtINDEX_IntervalLimitOperation::ProcessSKMLCase ()
{
    rtLINK_CType *pKeyMapDomainReference;
    rtLINK_CType *pKeyMapDomainReferenceGuard;
    if (LocateKeyAsKeyMapDomainReference (
	    m_xLookupCase, pKeyMapDomainReference, pKeyMapDomainReferenceGuard
	)
    )
    {
	SetResultToIntervalLimit (
	    pKeyMapDomainReference, pKeyMapDomainReferenceGuard, NilOf (M_CPD*)
	);

	pKeyMapDomainReference->release ();
	if (pKeyMapDomainReferenceGuard)
	    pKeyMapDomainReferenceGuard->release ();
    }
    else ProduceNAResult ();
}


/*************************************
 *****  SKSL Delegation Handler  *****
 *************************************/

void rtINDEX_IntervalLimitOperation::ProcessSKSLCase ()
{
    rtREFUV_Type_Reference iKeyMapDomainReference;
    if (LocateKeyAsKeyMapDomainReference (m_xLookupCase, iKeyMapDomainReference))
    {
	SetResultToIntervalLimit (iKeyMapDomainReference);
	DSC_ClearScalar (iKeyMapDomainReference);
    }
    else ProduceNAResult ();
}


/******************************************
 *****  NA Result Generation Handler  *****
 ******************************************/

void rtINDEX_IntervalLimitOperation::ProduceNAResult ()
{
    SetResultToTimeZero ();
}


/*****************************************************
 *****  'rtINDEX_IntervalLimits' Implementation  *****
 *****************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to lookup point(s) in an index.
 *
 *  Arguments:
 *	pResultDescriptor	- the address of a descriptor which will be set to
 *				  contain the point(s) found.
 *	pIndexDescriptor	- the address of a descriptor naming the lists
 *				  to search.
 *	pKey			- the address of a 'rtINDEX_Key *' naming the
 *				  keys to locate.
 *	comparisonType		- a 'rtLINK_LookupCase' enumeration value specifying
 *				  the index/query key comparison type.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtINDEX_IntervalLimits (
    DSC_Descriptor	*pResultDescriptor,
    DSC_Descriptor	*pIndexDescriptor,
    rtINDEX_Key		*pKey,
    rtLINK_LookupCase	 comparisonType
) {
    rtINDEX_IntervalLimitOperation operation (
	*pResultDescriptor, *pIndexDescriptor, pKey, comparisonType
    );

    operation.operate ();
}


/*********************************************************************
 *********************************************************************
 *****  Delete Element Class and 'rtINDEX_Delete' Implementation *****
 *********************************************************************
 *********************************************************************/

/************************
 *****  Definition  *****
 ************************/

class rtINDEX_DeleteElementOperation : public rtINDEX_KeyedElementOperation {
/*****  Construction  *****/
public:
    rtINDEX_DeleteElementOperation (
	DSC_Descriptor	&iResultDescriptor,
	DSC_Descriptor	&iIndexDescriptor,
	rtINDEX_Key	*pKey
    ) : rtINDEX_KeyedElementOperation (iResultDescriptor, iIndexDescriptor, pKey)
    {
    }

/*****  Cleanup  *****/
public:
    void DeleteUnusedKeys ()
    {
	rtINDEX_DeleteUnusedKeys (m_pIndexCPD);
    }

/*****  Delegation  *****/
protected:
    virtual char const *WhatIAmDoingDescription() const;

    virtual void ProcessRecursiveCase (rtINDEX_Key *pKey);

    virtual void ProcessMKMLCase ();

    virtual void ProcessSKMLCase ();

    virtual void ProcessSKSLCase ();

    virtual void ProduceNAResult ();
};


/**************************************************************************************
 *****  'WhatIAmDoingDescription' and 'ProcessRecursiveCase' Delegation Handlers  *****
 **************************************************************************************/

char const *rtINDEX_DeleteElementOperation::WhatIAmDoingDescription() const {
    return "DeleteElement";
}

void rtINDEX_DeleteElementOperation::ProcessRecursiveCase (rtINDEX_Key *pKey) {
    rtINDEX_Delete (&m_iIndexDescriptor, pKey);
}


/*************************************
 *****  MKML Delegation Handler  *****
 *************************************/

void rtINDEX_DeleteElementOperation::ProcessMKMLCase () {
    bool		 fKeyMapComainIsAProduct;
    rtLINK_CType	*pKeyMapComainReferenceLC;
    M_CPD		*pKeyMapComainDistribution;
    M_CPD		*pKeyMapComainReferenceCPD;
    LocateKeyAsKeyMapComainReference (
	rtLINK_LookupCase_EQ,
	fKeyMapComainIsAProduct,
	pKeyMapComainReferenceLC,
	pKeyMapComainDistribution,
	pKeyMapComainReferenceCPD
    );

    if (fKeyMapComainIsAProduct) {
	rtLINK_DeleteLCSelectedElements (
	    m_pKeyMapCPD, pKeyMapComainReferenceLC, NilOf (rtLINK_CType **)
	);
	pKeyMapComainReferenceLC->release ();
    }
    else {
	rtREFUV_PartitionedDelete (
	    m_pKeyMapCPD,
	    rtLSTORE_CPD_BreakpointArray (m_pLStoreCPD),
	    OperationIndexReferenceLC (),
	    pKeyMapComainReferenceCPD
	);
	pKeyMapComainDistribution = NilOf (M_CPD*);

	pKeyMapComainReferenceCPD->release ();
    }

    if (pKeyMapComainDistribution)
	pKeyMapComainDistribution->release ();

    rtLSTORE_Align (m_pLStoreCPD);
}


/*************************************
 *****  SKML Delegation Handler  *****
 *************************************/

void rtINDEX_DeleteElementOperation::ProcessSKMLCase () {
    bool			 fKeyMapComainIsAProduct;
    rtLINK_CType		*pKeyMapComainReferenceLC;
    rtREFUV_Type_Reference	iKeyMapComainReference;
    if (!LocateKeyAsKeyMapComainReference (
	    rtLINK_LookupCase_EQ,
	    fKeyMapComainIsAProduct,
	    pKeyMapComainReferenceLC,
	    iKeyMapComainReference
	)
    ) ProduceNAResult ();
    else if (fKeyMapComainIsAProduct) {
	rtLINK_DeleteLCSelectedElements (
	    m_pKeyMapCPD, pKeyMapComainReferenceLC, NilOf (rtLINK_CType **)
	);
	pKeyMapComainReferenceLC->release ();

	rtLSTORE_Align (m_pLStoreCPD);
    }
    else {
	rtREFUV_PartitionedDelete (
	    m_pKeyMapCPD,
	    rtLSTORE_CPD_BreakpointArray (m_pLStoreCPD),
	    OperationIndexReferenceLC (),
	    iKeyMapComainReference
	);
	DSC_ClearScalar (iKeyMapComainReference);

	rtLSTORE_Align (m_pLStoreCPD);
    }
}


/*************************************
 *****  SKSL Delegation Handler  *****
 *************************************/

void rtINDEX_DeleteElementOperation::ProcessSKSLCase () {
    bool fKeyMapComainIsAProduct;
    rtREFUV_Type_Reference iKeyMapComainReference;
    if (!LocateKeyAsKeyMapComainReference (
	    rtLINK_LookupCase_EQ, fKeyMapComainIsAProduct, iKeyMapComainReference
	)
    ) ProduceNAResult ();
    else if (fKeyMapComainIsAProduct) {
	rtLINK_DeleteRefSelectedElement (m_pKeyMapCPD, &iKeyMapComainReference);
	DSC_ClearScalar (iKeyMapComainReference);

	rtLSTORE_Align (m_pLStoreCPD);
    }
    else {
	rtREFUV_PartitionedDelete (
	    m_pKeyMapCPD,
	    rtLSTORE_CPD_BreakpointArray (m_pLStoreCPD),
	    OperationIndexReference (),
	    iKeyMapComainReference
	);
	DSC_ClearScalar (iKeyMapComainReference);

	rtLSTORE_Align (m_pLStoreCPD);
    }
}


/******************************************
 *****  NA Result Generation Handler  *****
 ******************************************/

void rtINDEX_DeleteElementOperation::ProduceNAResult ()
{
}


/*********************************************
 *****  'rtINDEX_Delete' Implementation  *****
 *********************************************/

/*---------------------------------------------------------------------------
 *****  Key Deletion.
 *
 *  Arguments:
 *	pIndexDescriptor	- the address of a descriptor naming the affected lists.
 *	pKey			- the address of a 'rtINDEX_Key *' naming the keys
 *				  to delete.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtINDEX_Delete (DSC_Descriptor *pIndexDescriptor, rtINDEX_Key *pKey) {
    DSC_Descriptor placeHolder;
    rtINDEX_DeleteElementOperation operation (placeHolder, *pIndexDescriptor, pKey);

    operation.operate ();
    operation.DeleteUnusedKeys ();
}


/*********************************************************************
 *********************************************************************
 *****  Delete Key Class and 'rtINDEX_KeyDelete' Implementation  *****
 *********************************************************************
 *********************************************************************/

/************************
 *****  Definition  *****
 ************************/

class rtINDEX_DeleteKeyOperation : public rtINDEX_KeyedOperation {

/*****  Operation  *****/
public:
    void operate ();

/*****  Delegation  *****/
protected:
    virtual char const *WhatIAmDoingDescription() const;

/*****  Construction  *****/
public:
    rtINDEX_DeleteKeyOperation (
	DSC_Descriptor	&iResultDescriptor,
	DSC_Descriptor	&iIndexDescriptor,
	rtINDEX_Key	*pKey
    ) : rtINDEX_KeyedOperation (iResultDescriptor, iIndexDescriptor, pKey)
    {
	m_pKeySetDomainAdjustment = NilOf (rtPTOKEN_CType *);
    }

/*****  Destruction  *****/
public:
    ~rtINDEX_DeleteKeyOperation () {
	if (m_pKeySetDomainAdjustment)
	    m_pKeySetDomainAdjustment->discard ();
    }

/*****  Update *****/
protected:
    void update ();
    void commit ();

/*****  Traversal  *****/
protected:
    void handleTraversalNil (
	unsigned int Unused(xDomainRegion), unsigned int Unused(sDomainRegion), unsigned int Unused(xComainRegion)
    )
    {
    }
    
    inline void handleTraversalRepetition (
	unsigned int Unused(xDomainRegion), unsigned int Unused(sDomainRegion), unsigned int xComainRegion
    )
    {
	m_pKeySetDomainAdjustment->AppendAdjustment (
	    (int)(xComainRegion + 1 - m_sKeySetDomainAdjustment), -1
	);
	m_sKeySetDomainAdjustment++;
    }

    inline void handleTraversalRange (
	unsigned int Unused(xDomainRegion), unsigned int sDomainRegion, unsigned int xComainRegion
    )
    {
	m_pKeySetDomainAdjustment->AppendAdjustment (
	    (int)(xComainRegion + sDomainRegion - m_sKeySetDomainAdjustment),
	    -(int)sDomainRegion
	);
	m_sKeySetDomainAdjustment += sDomainRegion;
    }

/*****  State  *****/
protected:
    rtPTOKEN_CType*		m_pKeySetDomainAdjustment;
    unsigned int		m_sKeySetDomainAdjustment;
};


/********************
 *****  Update  *****
 ********************/

void rtINDEX_DeleteKeyOperation::update () {
    m_sKeySetDomainAdjustment = 0;
    m_pKeySetDomainAdjustment = rtPTOKEN_NewShiftPTConstructor (
	m_pKeySetCPD, RTYPE_C_Vector == (RTYPE_Type)M_CPD_RType (
	    m_pKeySetCPD
	) ? rtVECTOR_CPx_PToken : UV_CPx_PToken
    );
}

void rtINDEX_DeleteKeyOperation::commit () {
    m_pKeySetDomainAdjustment->ToPToken ()->release ();
    m_pKeySetDomainAdjustment = NilOf (rtPTOKEN_CType *);

    rtINDEX_Align (m_pIndexCPD, true);
}


/***********************
 *****  Operation  *****
 ***********************/

void rtINDEX_DeleteKeyOperation::operate () {
    if (m_pKey->isScalar ()) {
	rtREFUV_Type_Reference iKeySetDomainReference;
	if (LocateKeyAsKeySetReference (rtLINK_LookupCase_EQ, iKeySetDomainReference)) {
	    update ();
	    m_pKeySetDomainAdjustment->AppendAdjustment (
		DSC_Scalar_Int (iKeySetDomainReference) + 1, -1
	    );
	    commit ();

	    DSC_ClearScalar (iKeySetDomainReference);
	}
    }
    else {
	rtLINK_CType *pKeySetDomainReference;
	rtLINK_CType *pKeySetDomainReferenceGuard;
	LocateKeyAsKeySetReference (
	    rtLINK_LookupCase_EQ, pKeySetDomainReference, pKeySetDomainReferenceGuard
	);

	if (pKeySetDomainReferenceGuard)
	    pKeySetDomainReferenceGuard->release ();

	update ();
	rtLINK_TraverseRRDCList (
	    pKeySetDomainReference,
	    handleTraversalNil,
	    handleTraversalRepetition,
	    handleTraversalRange
	);
	commit ();

	pKeySetDomainReference->release ();
    }
}


/***********************************************************
 *****  'WhatIAmDoingDescription' Delegation Handlers  *****
 ***********************************************************/

char const *rtINDEX_DeleteKeyOperation::WhatIAmDoingDescription() const
{
    return "DeleteKey";
}


/************************************************
 *****  'rtINDEX_KeyDelete' Implementation  *****
 ************************************************/

/*---------------------------------------------------------------------------
 *****  Routine to delete an entire column of an indexed list store.
 *
 *  Arguments:
 *	pIndexDescriptor	- the address of a descriptor naming the affected lists.
 *	pKey			- the address of a 'rtINDEX_Key *' naming the keys to
 *				  delete.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtINDEX_KeyDelete (DSC_Descriptor *pIndexDescriptor, rtINDEX_Key *pKey) {
    DSC_Descriptor placeHolder;
    rtINDEX_DeleteKeyOperation operation (placeHolder, *pIndexDescriptor, pKey);

    operation.operate ();
}


/******************************
 ******************************
 *****  Debugger Methods  *****
 ******************************
 ******************************/

/********************
 *  'Type' Methods  *
 ********************/

IOBJ_DefineNewaryMethod (NewDM)
{
    return RTYPE_QRegister (
	rtINDEX_NewCluster (RTYPE_QRegisterCPD (parameterArray[0]), NilOf (M_CPD*), -1)
    );
}

IOBJ_DefineNewaryMethod (NewUsingContentPrototypeDM)
{
    return RTYPE_QRegister (
	rtINDEX_NewCluster (
	    RTYPE_QRegisterCPD (parameterArray[0]),
	    RTYPE_QRegisterCPD (parameterArray[1]),
	    -1
	)
    );
}

IOBJ_DefineUnaryMethod (DisplayCountsDM)
{
    IO_printf ("\nIndex Routines Usage Counts\n");
    IO_printf (  "--------------------------\n");

    IO_printf ("NewKeyCount		= %d\n", NewKeyCount);
    IO_printf ("NewFutureKeyCount	= %d\n", NewFutureKeyCount);
    IO_printf ("\n");

    IO_printf ("RealizeKeySequenceCount	= %d\n", RealizeKeySequenceCount);
    IO_printf ("RealizeKeySetCount	= %d\n", RealizeKeySetCount);
    IO_printf ("\n");

    IO_printf ("NewCount		= %d\n", NewCount);
    IO_printf ("NewTimeSeriesCount	= %d\n", NewTimeSeriesCount);
    IO_printf ("\n");

    IO_printf ("DefineMKeyMListKeys	= %d\n", DefineMKeyMListCount);
    IO_printf ("DefineSKeyMListKeys	= %d\n", DefineSKeyMListCount);
    IO_printf ("DefineSKeySListKeys	= %d\n", DefineSKeySListCount);
    IO_printf ("Total Defines		= %d\n",
	DefineMKeyMListCount + DefineSKeyMListCount + DefineSKeySListCount
    );
    IO_printf ("\n");

    IO_printf ("LookupMKeyMListKeys	= %d\n", LookupMKeyMListCount);
    IO_printf ("LookupSKeyMListKeys	= %d\n", LookupSKeyMListCount);
    IO_printf ("LookupSKeySListKeys	= %d\n", LookupSKeySListCount);
    IO_printf ("Total Lookups		= %d\n",
	LookupMKeyMListCount + LookupSKeyMListCount + LookupSKeySListCount
    );
    IO_printf ("\n");

    IO_printf ("LookupMListLimit	= %d\n", LookupMListLimitCount);
    IO_printf ("LookupSListLimit	= %d\n", LookupSListLimitCount);
    IO_printf ("Total Limit Hunts	= %d\n",
	LookupMListLimitCount + LookupSListLimitCount);
    IO_printf ("\n");


    IO_printf ("DeleteMListKeys		= %d\n", DeleteMListKeysCount);
    IO_printf ("DeleteSListKey		= %d\n", DeleteSListKeyCount);
    IO_printf ("Total Deletes		= %d\n",
	DeleteMListKeysCount + DeleteSListKeyCount);
    IO_printf ("\n");

    IO_printf ("ExtractCount		= %d\n", ExtractCount);
    IO_printf ("\n");

    return self;
}


IOBJ_DefineUnaryMethod (InitCountsDM)
{
    NewKeyCount			= 0;
    NewFutureKeyCount		= 0;
    RealizeKeySequenceCount	= 0;
    RealizeKeySetCount		= 0;

    NewCount			= 0;
    NewTimeSeriesCount		= 0;

    DefineMKeyMListCount	= 0;
    DefineSKeyMListCount	= 0;
    DefineSKeySListCount	= 0;

    LookupMKeyMListCount	= 0;
    LookupSKeySListCount	= 0;
    LookupSKeyMListCount	= 0;

    LookupMListLimitCount	= 0;
    LookupSListLimitCount	= 0;

    DeleteMListKeysCount	= 0;
    DeleteSListKeyCount		= 0;

    ExtractCount		= 0;

    return self;
}


IOBJ_DefineUnaryMethod (SetDefaultMapTypeToProductDM)
{
    rtINDEX_KeyMapDefaultType = RTYPE_C_Link;
    return self;
}

IOBJ_DefineUnaryMethod (SetDefaultMapTypeToPartitionDM)
{
    rtINDEX_KeyMapDefaultType = RTYPE_C_RefUV;
    return self;
}

IOBJ_DefineNilaryMethod (GetDefaultMapTypeDM)
{
    return RTYPE_TypeIObject (rtINDEX_KeyMapDefaultType);
}


/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (AlignDM) {
    rtINDEX_Align (RTYPE_QRegisterCPD (self));

    return self;
}

IOBJ_DefineUnaryMethod (CompAlignDM) {
    rtINDEX_AlignAll (RTYPE_QRegisterCPD (self), false);

    return self;
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler (rtINDEX_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"			, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"			, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new:"			, NewDM)
	IOBJ_MDE ("newUsingContentPrototype:"	, NewUsingContentPrototypeDM)
	IOBJ_MDE ("displayCounts"		, DisplayCountsDM)
	IOBJ_MDE ("initCounts"			, InitCountsDM)
	IOBJ_MDE ("setDefaultMapTypeToProduct"	, SetDefaultMapTypeToProductDM)
	IOBJ_MDE ("setDefaultMapTypeToPartition", SetDefaultMapTypeToPartitionDM)
	IOBJ_MDE ("defaultMapType"		, GetDefaultMapTypeDM)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	POPVECTOR_StdDMDEPackage
	IOBJ_MDE ("align"		, AlignDM)
	IOBJ_MDE ("compAlign"		, CompAlignDM)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDReusability	();
	    rtd->SetCPDPointerCountTo	(rtINDEX_CPD_StdPtrCount + POPVECTOR_CPD_XtraPtrCount);
	    M_RTD_CPDInitFn		(rtd) = POPVECTOR_InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtINDEX_Handle::Maker;
	    M_RTD_ReclaimFn		(rtd) = POPVECTOR_ReclaimContainer;
	    M_RTD_MarkFn		(rtd) = POPVECTOR_MarkContainers;
	}
        break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD *>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD *>() = instanceMD;
        break;
    case RTYPE_PrintObject:
	POPVECTOR_Print (iArgList.arg<M_CPD*>(), false);
        break;
    case RTYPE_RPrintObject:
	POPVECTOR_Print (iArgList.arg<M_CPD*>(), true);
        break;
    default:
        return -1;
    }
    return 0;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (rtINDEX);
    FAC_FDFCase_FacilityDescription
        ("List Index Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTindex.c 1 replace /users/mjc/system
  870109 10:26:28 mjc Release 'index' representation type shell

 ************************************************************************/
/************************************************************************
  RTindex.c 2 replace /users/mjc/system
  870330 19:07:06 mjc Release for ongoing work by 'jad'

 ************************************************************************/
/************************************************************************
  RTindex.c 3 replace /users/jad/system
  870414 16:01:19 jad implemented basics, key routines, & Define and
Lookup routines.

 ************************************************************************/
/************************************************************************
  RTindex.c 4 replace /users/jck/system
  870415 14:15:42 jck Fixed minor bugs in define and lookup routines -- Non Scalar functions

 ************************************************************************/
/************************************************************************
  RTindex.c 5 replace /users/jad/system
  870421 12:38:28 jad implemented Extract routine

 ************************************************************************/
/************************************************************************
  RTindex.c 6 replace /users/jck/system
  870511 15:29:59 jck Added 'interval lookup' and 'delete' capability

 ************************************************************************/
/************************************************************************
  RTindex.c 7 replace /users/jck/system
  870513 11:38:00 jck Corrected implementation of 'nextDate:'

 ************************************************************************/
/************************************************************************
  RTindex.c 8 replace /users/jck/system
  870522 15:29:55 jck Fixed a couple of bugs

 ************************************************************************/
/************************************************************************
  RTindex.c 9 replace /users/mjc/translation
  870524 09:32:58 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTindex.c 10 replace /users/jck/system
  870605 16:10:28 jck Generalized handling to include vectors as one choice for an indexed list's index

 ************************************************************************/
/************************************************************************
  RTindex.c 11 replace /users/jck/system
  870612 13:30:55 jck Implemented ability to add lists to existing lstores

 ************************************************************************/
/************************************************************************
  RTindex.c 12 replace /users/jck/system
  870703 16:06:18 jck Fixed bug causing flawed execution of 'intervalEnd'

 ************************************************************************/
/************************************************************************
  RTindex.c 13 replace /users/jck/system
  870707 06:52:48 jck Implemented 'rtINDEX_DeleteUnusedKeys' and had
rtINDEX_Delete include a call to it

 ************************************************************************/
/************************************************************************
  RTindex.c 14 replace /users/jck/system
  870707 07:32:04 jck This time for real

 ************************************************************************/
/************************************************************************
  RTindex.c 15 replace /users/jck/system
  870803 16:46:01 jck Fixed bug in the scalar case of rtINDEX_InitializedNew

 ************************************************************************/
/************************************************************************
  RTindex.c 16 replace /users/jck/system
  870812 15:42:03 jck Corrected invalid assumption that keyvalue's ref nil was always zero

 ************************************************************************/
/************************************************************************
  RTindex.c 17 replace /users/jck/system
  871007 13:37:51 jck Added a marking function for the global garbage collector

 ************************************************************************/
/************************************************************************
  RTindex.c 18 replace /users/jck/system
  871214 14:32:08 jck (1) Forced alignment of underlying lstores in the various lookup routines.
(2) Added rtINDEX_Align and rtINDEX_AlignAll

 ************************************************************************/
/************************************************************************
  RTindex.c 19 replace /users/mjc/Software/system
  871214 16:25:50 mjc Added comprehensive alignment routines for all property types

 ************************************************************************/
/************************************************************************
  RTindex.c 20 replace /users/jad/system
  871223 16:09:55 jad Added omitted key materialization logic to lookup case routines

 ************************************************************************/
/************************************************************************
  RTindex.c 21 replace /users/mjc/Software/system
  871223 16:32:41 mjc Corrected cumulative computation bug in 'DeleteEmptyColumns'

 ************************************************************************/
/************************************************************************
  RTindex.c 22 replace /users/jad/system
  880107 16:01:54 jad fixed placement of 'rtINDEX_RealizeSet' calls
and fixed bug which didn't re-distribute 'pIndexDescriptor' in Define, Lookup
 IntervalLimits, and Delete functions.

 ************************************************************************/
/************************************************************************
  RTindex.c 23 replace /users/jad/system
  880711 16:45:09 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  RTindex.c 24 replace /users/jck/system
  881212 10:51:42 jck Introduced memory saving efficiencies into some Distribution functions

 ************************************************************************/
/************************************************************************
  RTindex.c 25 replace /users/jck/system
  890117 13:26:12 jck Implemented Indexed List entire column deletion

 ************************************************************************/
/************************************************************************
  RTindex.c 26 replace /users/jck/system
  890223 07:59:22 jck Changes in use of alignments for the DeleteColumn operatyion

 ************************************************************************/
/************************************************************************
  RTindex.c 27 replace /users/jck/system
  890413 13:38:24 jck Added rtINDEX_DetermineExpansion

 ************************************************************************/
/************************************************************************
  RTindex.c 28 replace /users/jck/system
  890912 16:05:27 jck Removing RTtstore from the system

 ************************************************************************/
