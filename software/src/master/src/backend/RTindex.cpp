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

/******************************
 ******************************
 *****  Canonicalization  *****
 ******************************
 ******************************/

bool rtINDEX_Handle::getCanonicalization_(rtVSTORE_Handle::Reference &rpStore, DSC_Pointer const &rPointer) {
    rpStore.setTo (
	static_cast<rtVSTORE_Handle*>(
	    (isATimeSeries () ? TheTimeSeriesClass () : TheIndexedListClass ()).ObjectHandle ()
	)
    );
    return true;
}


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
 *	pPPT		- the positional p-token of 'source'.  Redundant but
 *			  available.
 *	source		- a standard descriptor for the elements to be held
 *                        in this key.
 *
 *  Returns:
 *	The address of the created key.
 *
 *****/
rtINDEX_Key::rtINDEX_Key (rtPTOKEN_Handle *pPPT, DSC_Descriptor *source) : BaseClass (1), m_pPPT (pPPT) {
    NewKeyCount++;

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
    rtPTOKEN_Handle::Reference pPPT;
    if (!reordering)
	m_pPPT.setTo (subset->PPT ());
    else {
	if (reordering->RType () != RTYPE_C_RefUV) raiseException (
	    EC__InternalInconsistency,
	    "rtINDEX_NewFutureKey: 'reordering' must be a reference uvector"
	);

	if (rtPTOKEN_BaseElementCount (reordering, UV_CPx_RefPToken) !=
	    rtPTOKEN_BaseElementCount (reordering, UV_CPx_PToken)
	) raiseException (
	    EC__InternalInconsistency,
	    "rtINDEX_NewFutureKey: Reordering P-Tokens Differ In Size"
	);

	m_pPPT.setTo (static_cast<rtUVECTOR_Handle*>(reordering->containerHandle ())->pptHandle ());
    }

/*****  ... and initialize a new key, ...  *****/
/*****  ... as a new scalar key if 'parent' is scalar, ...  *****/
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
	m_iSet.constructScalarComposition (
	    m_iSequence.store (), 0, DSC_Descriptor_Value (m_iSequence)
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

bool rtINDEX_Key::ConformsToIndex (rtINDEX_Handle *pIndex) {
    if (!pIndex || !pIndex->isATimeSeries ())
	return false;

    rtINDEX_Key *pKey = this;
    DSC_Descriptor *pKeyDescriptor = NilOf (DSC_Descriptor*);
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

    Vdd::Store::Reference pIndexKeyStore;
    pIndex->getKeyStore (pIndexKeyStore);
    return pIndexKeyStore->Names (pKeyDescriptor->store ());
}

bool rtINDEX_Key::ConformsToIndex (DSC_Descriptor const &rInstances) {
    return ConformsToIndex (dynamic_cast<rtINDEX_Handle*>(rInstances.store ()));
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
 *	pInstancePToken		- the address of a handle for the list p-token of the
 *				  index.
 *	xKeyMapRType		- the index of the new index's key map r-type.  Must
 *				  be either RTYPE_C_Link or RTYPE_C_RefUV.
 *	xKeySetRType		- the index of the new index's key set r-type.  Must
 *				  be one of the value u-vector types or RTYPE_C_Vector.
 *	pKeySetRPT		- a handle for the reference p-token of the key set.
 *				  Required if 'xKeySetRType' is one of the u-vector types,
 *				  ignored otherwise.
 *	pKeyStore		- the key store for this index.
 *				  Required if 'xKeySetRType' is one of the u-vector
 *				  types, ignored otherwise.
 *	pContentPrototype	- a CPD for the content prototype of the contained
 *				  l-store (Nil to request a Vector).
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *
 *****/
PrivateFnDef rtINDEX_Handle *rtINDEX_NewCluster (
    rtPTOKEN_Handle*		pInstancePToken,
    RTYPE_Type			xKeyMapRType,
    RTYPE_Type			xKeySetRType,
    rtPTOKEN_Handle*		pKeySetRPT,
    Vdd::Store*			pKeyStore,
    Vdd::Store*			pContentPrototype
)
{
/*****  Superficially validate the key set creation arguments, ...  *****/
    if (RTYPE_C_Vector != xKeySetRType && IsNil (pKeyStore)) ERR_SignalFault (
	EC__InternalInconsistency, UTIL_FormatMessage (
	    "rtINDEX_NewCluster: Key Store Required For Key Type %s",
	    RTYPE_TypeIdAsString (xKeySetRType)
	)
    );

/*****  ... create the key set, ...  *****/
    M_ASD *pContainerSpace = pInstancePToken->Space ();
    rtPTOKEN_Handle::Reference pKeySetPPT (new rtPTOKEN_Handle (pContainerSpace, 0));
    M_CPD *pKeySet;
    switch (xKeySetRType) {
    case RTYPE_C_Vector:
	pKeySet = (new rtVECTOR_Handle (pKeySetPPT, true))->GetCPD ();
	pKeyStore = 0;
	break;
    case RTYPE_C_CharUV:
	pKeySet = rtCHARUV_New (pKeySetPPT, pKeySetRPT);
	UV_CPD_IsASetUV (pKeySet) = true;
	break;
    case RTYPE_C_DoubleUV:
	pKeySet = rtDOUBLEUV_New (pKeySetPPT, pKeySetRPT);
	UV_CPD_IsASetUV (pKeySet) = true;
	break;
    case RTYPE_C_FloatUV:
	pKeySet = rtFLOATUV_New (pKeySetPPT, pKeySetRPT);
	UV_CPD_IsASetUV (pKeySet) = true;
	break;
    case RTYPE_C_IntUV:
	pKeySet = rtINTUV_New (pKeySetPPT, pKeySetRPT);
	UV_CPD_IsASetUV (pKeySet) = true;
	break;
    case RTYPE_C_Unsigned64UV:
	pKeySet = rtU64UV_New (pKeySetPPT, pKeySetRPT);
	UV_CPD_IsASetUV (pKeySet) = true;
	break;
    case RTYPE_C_Unsigned96UV:
	pKeySet = rtU96UV_New (pKeySetPPT, pKeySetRPT);
	UV_CPD_IsASetUV (pKeySet) = true;
	break;
    case RTYPE_C_Unsigned128UV:
	pKeySet = rtU128UV_New (pKeySetPPT, pKeySetRPT);
	UV_CPD_IsASetUV (pKeySet) = true;
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
    rtLSTORE_Handle::Reference pListStore (
	new rtLSTORE_Handle (pInstancePToken, pContentPrototype)
    );

/*****  create the key map, ... *****/
    rtPTOKEN_Handle::Reference pElementPToken;
    pListStore->getContentPToken (pElementPToken);
    M_CPD *pKeyMap;
    switch (xKeyMapRType) {
    case RTYPE_C_Link: {
	    rtPTOKEN_Handle::Reference pKeyMapRPT (
		new rtPTOKEN_Handle (pContainerSpace, pInstancePToken, pKeySetPPT)
	    );
	    pKeyMap = rtLINK_NewEmptyLink (pElementPToken, pKeyMapRPT);
	}
	break;
    case RTYPE_C_RefUV:
	pKeyMap = rtREFUV_New (pElementPToken, pKeySetPPT);
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
    rtINDEX_Handle *pIndex = new rtINDEX_Handle (
	pContainerSpace, pListStore, pKeyMap, pKeySet, pKeyStore
    );

/*****  ... cleanup, ...  *****/
    pKeyMap->release ();
    pKeySet->release ();

/*****  ... and return:  *****/
    NewCount++;

    return pIndex;
}

rtINDEX_Handle::rtINDEX_Handle (
    M_ASD *pSpace, rtLSTORE_Handle *pListStore, M_CPD *pKeyMap, M_CPD *pKeySet, Vdd::Store *pKeyStore
) : BaseClass (pSpace, RTYPE_C_Index, rtINDEX_CPD_StdPtrCount) {
    StoreReference (rtINDEX_CPx_Lists	 , static_cast<VContainerHandle*>(pListStore));
    StoreReference (rtINDEX_CPx_Map	 , pKeyMap);
    StoreReference (rtINDEX_CPx_KeyValues, pKeySet);
    if (pKeyStore)
	StoreReference (rtINDEX_CPx_KeyStore, pKeyStore);
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new index and its associated L-Store.
 *
 *  Arguments:
 *	pInstancePToken		- the address of a handle for the list p-token of the
 *				  index.
 *	xKeySetRType		- the index of the new index's key set r-type.  Must
 *				  be one of the value u-vector types or RTYPE_C_Vector.
 *	pKeySetRPT		- a handle for the reference p-token of the key set.
 *				  Required if 'xKeySetRType' is one of the u-vector
 *				  types, ignored otherwise.
 *	pKeyStoreCPD		- a CPD for the key store for this index.  Required
 *				  if 'xKeySetRType' is one of the u-vector types,
 *				  ignored otherwise.
 *	pContentPrototype	- a CPD for the content prototype of the contained
 *				  l-store (Nil to request a Vector).
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *
 *****/
PublicFnDef rtINDEX_Handle *rtINDEX_NewCluster (
    rtPTOKEN_Handle*		pInstancePToken,
    RTYPE_Type			xKeySetRType,
    rtPTOKEN_Handle*		pKeySetRPT,
    Vdd::Store*			pKeyStore,
    Vdd::Store*			pContentPrototype
) {
    return rtINDEX_NewCluster (
	pInstancePToken, rtINDEX_KeyMapDefaultType, xKeySetRType, pKeySetRPT, pKeyStore, pContentPrototype
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new indexed list cluster.
 *
 *  Arguments:
 *	pInstancePToken		- a handle for the positional p-token
 *				  of the index.
 *	pContentPrototype	- a CPD for the content prototype of the contained
 *				  l-store (Nil to request a Vector).
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *****/
PublicFnDef rtINDEX_Handle *rtINDEX_NewCluster (
    rtPTOKEN_Handle *pInstancePToken, Vdd::Store *pContentPrototype
) {
    return rtINDEX_NewCluster (
	pInstancePToken,
	rtINDEX_KeyMapDefaultType,
	RTYPE_C_Vector,
	NilOf (rtPTOKEN_Handle*),
	NilOf (Vdd::Store*),
	pContentPrototype
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new indexed list cluster.
 *
 *  Arguments:
 *	pInstancePToken		- the address of a handle for the positional p-token
 *				  of the index.
 *	xKeyMapRType		- the index of the new index's key map r-type.  Must
 *				  be either RTYPE_C_Link or RTYPE_C_RefUV.
 *	pKeySetPrototype	- the address of a descriptor which provides the key's
 * 				  type information.
 *	pContentPrototype	- a CPD for the content prototype of the contained
 *				  l-store (Nil to request a Vector).
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *****/
PrivateFnDef rtINDEX_Handle *rtINDEX_NewCluster (
    rtPTOKEN_Handle*		pInstancePToken,
    RTYPE_Type			xKeyMapRType,
    DSC_Descriptor*		pKeySetPrototype,
    Vdd::Store*			pContentPrototype
) {
    RTYPE_Type xKeySetRType = pKeySetPrototype->pointerRType ();

    rtPTOKEN_Handle::Reference pKeySetRPT (pKeySetPrototype->RPT ());
    return rtINDEX_NewCluster (
	pInstancePToken, xKeyMapRType, xKeySetRType, pKeySetRPT, pKeySetPrototype->store (), pContentPrototype
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new indexed list cluster.
 *
 *  Arguments:
 *	pInstancePToken		- the address of a handle for the positional p-token
 *				  of the index.
 *	pKeySetPrototype	- the address of a descriptor which provides the key's
 * 				  type information.
 *	pContentPrototype	- a CPD for the content prototype of the contained
 *				  l-store (Nil to request a Vector).
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *****/
PublicFnDef rtINDEX_Handle *rtINDEX_NewCluster (
    rtPTOKEN_Handle*		pInstancePToken,
    DSC_Descriptor*		pKeySetPrototype,
    Vdd::Store*			pContentPrototype
) {
    return rtINDEX_NewCluster (
	pInstancePToken, rtINDEX_KeyMapDefaultType, pKeySetPrototype, pContentPrototype
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new indexed list cluster.
 *
 *  Arguments:
 *	rpResult		- a reference to the rtINDEX_Handle::Reference
 *				  that will be set to a handle for the clone.
 *	pClonePPT		- the address of a handle for the positional p-token
 *				  of the index.
 *
 *****/
bool rtINDEX_Handle::isACloneOfIndex (rtINDEX_Handle const *pOther) const {
    return ReferenceNames (rtINDEX_CPx_KeyStore, pOther, pOther->element (rtINDEX_CPx_KeyStore));
}

void rtINDEX_Handle::clone (Reference &rpResult, rtPTOKEN_Handle *pClonePPT) const {
    M_CPD *pKeySet;
    getKeySet (pKeySet);

    Vdd::Store::Reference pKeyStore;  rtPTOKEN_Handle::Reference pKeySetRPT;
    if (ReferenceIsntNil (rtINDEX_CPx_KeyStore)) {
	getKeyStore (pKeyStore);
	pKeySetRPT.setTo (static_cast<rtUVECTOR_Handle*>(pKeySet->containerHandle ())->rptHandle ());
    }

    M_CPD *pKeyMap;
    getKeyMap (pKeyMap);

    Vdd::Store::Reference pContentPrototype; {
	rtLSTORE_Handle::Reference pListStore;
	getListStore (pListStore);
	pListStore->getContent (pContentPrototype);
    }

    rpResult.setTo (
	rtINDEX_NewCluster (
	    pClonePPT,
	    rtINDEX_KeyMapAutoConversionEnabled ? rtINDEX_KeyMapDefaultType : pKeyMap->RType (),
	    pKeySet->RType (),
	    pKeySetRPT,
	    pKeyStore,
	    pContentPrototype
	)
    );
    pKeyMap->release ();
    pKeySet->release ();
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
PrivateFnDef rtINDEX_Handle *rtINDEX_NewCluster (
    rtLINK_CType *pContentPartition, Vdd::Store *pContent, RTYPE_Type xKeyMapRType, rtINDEX_Key *pKey
) {

/*****  Make the LStore ... *****/
    rtLSTORE_Handle::Reference pListStore (new rtLSTORE_Handle (pContentPartition, pContent, true));
    M_ASD *pContainerSpace = pListStore->objectSpace ();

/*****  ... obtain the key set, ... *****/
    pKey->RealizeSet ();

    M_CPD *pKeySet;
    if (pKey->isScalar ()) {
	DSC_Pointer setPointer;
	M_CPD *pKeyEnumeratorCPD;

	pKey->RealizeSequence ();
	setPointer.constructSet (
	    pContainerSpace,
	    pKeyEnumeratorCPD,
	    DSC_Descriptor_Pointer (rtINDEX_Key_Sequence (pKey))
	);
	pKeySet = DSC_Pointer_Value (setPointer);
	pKeyEnumeratorCPD->release ();
    }
    else {
	pKeySet = DSC_Descriptor_Value (rtINDEX_Key_Set (pKey));
	pKeySet->retain ();
    }

/*****  ... create the key map, ...  *****/
    M_CPD *pKeyMap;
    switch (xKeyMapRType) {
    case RTYPE_C_Link: {
	    rtPTOKEN_Handle *pRowPToken = pContentPartition->RPT ();
	    rtPTOKEN_Handle::Reference pColPToken (
		static_cast<rtUVECTOR_Handle*>(pKeySet->containerHandle ())->pptHandle ()
	    );
	    rtLINK_CType *pKeyMapLC = rtLINK_RefConstructor (
		new rtPTOKEN_Handle (pContainerSpace, pRowPToken, pColPToken)
	    );
	    unsigned int const numberOfRows = pRowPToken->cardinality ();
	    unsigned int const numberOfCols = pColPToken->cardinality ();
	    unsigned int const *pBreakpoint = pListStore->breakpointArray () + 1;

	    unsigned int elementNumber = 0;
	    if (pKey->isScalar ()) {
		for (unsigned int rowNumber = 0; rowNumber < numberOfRows; rowNumber++, pBreakpoint++) {
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
		for (unsigned int rowNumber = 0; rowNumber < numberOfRows; rowNumber++, pBreakpoint++) {
		    while (elementNumber < *pBreakpoint) {
			elementNumber++;
			rtLINK_AppendRange (pKeyMapLC, rowNumber * numberOfCols + *enumeration++, 1);
		    }
		}
	    }
	    pKeyMapLC->Close (pContentPartition->PPT ());
	    pKeyMap = pKeyMapLC->ToLink ();
	}
	break;
    case RTYPE_C_RefUV:
	if (pKey->isScalar ()) {
	    pKeyMap = rtREFUV_New (pContentPartition->PPT (), pKeySet, (int)UV_CPx_PToken);
	    memset (
		rtREFUV_CPD_Array (pKeyMap),
		0,
		UV_CPD_ElementCount (pKeyMap) * UV_CPD_Granularity (pKeyMap)
	    );
	}
	else {
	    pKeyMap = rtINDEX_Key_Enumerator (pKey);
	    pKeyMap->retain ();
	}
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
    rtINDEX_Handle *pIndex = new rtINDEX_Handle (
	pContainerSpace, pListStore, pKeyMap, pKeySet, rtINDEX_Key_Set (pKey).store ()
    );

/*****  ...  clean up  ...  *****/
    pKeyMap->release ();
    pKeySet->release ();

/*****  ... and return the result.  *****/
    return pIndex;
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
PublicFnDef rtINDEX_Handle *rtINDEX_NewCluster (
    rtLINK_CType *pContentPartition, Vdd::Store *pContent, rtINDEX_Key *pKey
) {
    return rtINDEX_NewCluster (
	pContentPartition, pContent, rtINDEX_KeyMapDefaultType, pKey
    );
}


/*---------------------------------------------------------------------------
 *****  Routine to create a new time-series indexed list cluster.
 *
 *  Arguments:
 *	pInstancePToken		- a handle for the positional p-token
 *				  of the index.
 *
 *  Returns:
 *	The address of a CPD for the 'Index' created.
 *
 *****/
PublicFnDef rtINDEX_Handle *rtINDEX_NewTimeSeries (rtPTOKEN_Handle *pInstancePToken) {
    NewTimeSeriesCount++;

    M_KOTE const &rDate = pInstancePToken->TheDateClass ();

    return rtINDEX_NewCluster (
	pInstancePToken,
	rtINDEX_KeyMapDefaultType,
	RTYPE_C_IntUV,
	rDate.PTokenHandle (),
	rDate.store (),
	0
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
 *	bDeletingKeyMapNils	- a flag that, when non-zero, requests the deletion
 *				  of nils from partitioned key maps.
 *
 *  Returns:
 *	'pIndexCPD'
 *
 *****/
bool rtINDEX_Handle::align (bool bDeletingKeyMapNils) {
/*****
 *  Align the l-store first so that the key map alignment does not cause
 *  the list and element p-tokens of the l-store to be out of alignment
 *  simultaneously, ...
 *****/
    rtLSTORE_Handle::Reference pListStore;
    getListStore (pListStore);
    pListStore->align ();

/*****
 *  Align the key map, and, if element deletions were possible, align the
 *  l-store again to reflect those deletions, ...
 *****/
    M_CPD *pKeyMap;
    getKeyMap (pKeyMap);

    switch (pKeyMap->RType ()) {
    case RTYPE_C_Link:
	pKeyMap->align ();
	pListStore->align ();
	break;

    case RTYPE_C_RefUV:
	if (!bDeletingKeyMapNils)
	    pKeyMap->align ();
	else {
	    rtREFUV_DeleteNils (pKeyMap);
	    pListStore->align ();
	}
	break;

    default:
	ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"rtINDEX_Handle::align: Invalid Index Map Type: %s",
		pKeyMap->RTypeName ()
	    )
	);
	break;
    }
    pKeyMap->release ();

    return true;
}


/*********************************
 *****  Recursive Alignment  *****
 *********************************/

/*---------------------------------------------------------------------------
 *****  Routine to comprehensively align an Index.
 *
 *  Arguments:
 *	bDeletingEmptyUSegments	- a boolean which, when true, requests that
 *				  unreferenced u-segments be deleted from
 *				  vectors.
 *
 *  Returns:
 *	true if an alignment was performed, false otherwise.
 *
 *****/
bool rtINDEX_Handle::alignAll (bool bDeletingEmptyUSegments) {
    rtLSTORE_Handle::Reference pListStore;
    getListStore (pListStore);
    bool result = pListStore->alignAll (bDeletingEmptyUSegments); {
	M_CPD *pKeySet;
	getKeySet (pKeySet);
	result = pKeySet->alignAll (bDeletingEmptyUSegments) || result;
	pKeySet->release ();
    } {
	M_CPD *pKeyMap;
	getKeyMap (pKeyMap);
	switch (pKeyMap->RType ()) {
	case RTYPE_C_Link:
	    if (pKeyMap->isntTerminal (rtLINK_CPx_PosPToken) || pKeyMap->isntTerminal (rtLINK_CPx_RefPToken)) {
		pKeyMap->align ();
		result = true;
	    }
	    break;

	case RTYPE_C_RefUV:
	    if (pKeyMap->isntTerminal (UV_CPx_PToken) || pKeyMap->isntTerminal (UV_CPx_RefPToken)) {
		result = true;
	    }
	    result = rtREFUV_DeleteNils (pKeyMap) || result;
	    break;

	default:
	    pKeyMap->release ();
	    ERR_SignalFault (
		EC__InternalInconsistency, UTIL_FormatMessage (
		    "rtINDEX_Handle::alignAll: Invalid Index Map Type: %s",
		    pKeyMap->RTypeName ()
		)
	    );
	    break;
	}
	pKeyMap->release ();
    }
    if (pListStore->alignAll (bDeletingEmptyUSegments))
	result = true;

    return result;
}


/*************************************************
 *****  Product Key Map Construction Helper  *****
 *************************************************/

PrivateFnDef M_CPD *rtINDEX_PartitionedMapAsProductMap (M_CPD *pKeyMap, M_CPD *pLStoreCPD) {
    rtLINK_CType *pKeyMapLC; {
	rtPTOKEN_Handle::Reference pKeyMapPPT (
	    static_cast<rtPTOKEN_Handle*>(pKeyMap->GetContainerHandle (UV_CPx_RefPToken, RTYPE_C_PToken))
	);
	rtPTOKEN_Handle::Reference pLStorePPT (
	    static_cast<rtPTOKEN_Handle*>(pLStoreCPD->GetContainerHandle (rtLSTORE_CPx_RowPToken, RTYPE_C_PToken))
	);
	pKeyMapLC = rtLINK_RefConstructor (new rtPTOKEN_Handle (pLStoreCPD->Space (), pLStorePPT, pKeyMapPPT));
    }

    unsigned int const numberOfRows = rtPTOKEN_BaseElementCount (
	pLStoreCPD, rtLSTORE_CPx_RowPToken
    );
    unsigned int const numberOfCols = rtPTOKEN_BaseElementCount (
	pKeyMap, UV_CPx_RefPToken
    );
    unsigned int const *pBreakpoint = rtLSTORE_CPD_BreakpointArray (
	pLStoreCPD
    ) + 1;

    unsigned int elementNumber = 0;
    unsigned int const *enumeration = (unsigned int *)rtREFUV_CPD_Array (pKeyMap);
    for (
	unsigned int rowNumber = 0; rowNumber < numberOfRows; rowNumber++, pBreakpoint++
    ) {
	while (elementNumber < *pBreakpoint) {
	    elementNumber++;
	    rtLINK_AppendRange (
		pKeyMapLC, rowNumber * numberOfCols + *enumeration++, 1
	    );
	}
    }
    pKeyMapLC->Close (pKeyMap, UV_CPx_PToken);

    return pKeyMapLC->ToLink ();
}


/********************************
 *****  Key Map Conversion  *****
 ********************************/

void rtINDEX_Handle::getKeyMapType (RTYPE_Type &rxKeyMapType) const {
    VContainerHandle::Reference pKeyMap;
    getKeyMap (pKeyMap);
    rxKeyMapType = pKeyMap->RType ();
}

void rtINDEX_Handle::setKeyMapType (RTYPE_Type xKeyMapType) {
    align (true);

    M_CPD *pKeyMap;
    getKeyMap (pKeyMap);

    M_CPD *pNewMapCPD = NilOf (M_CPD*);
    switch (pKeyMap->RType ()) {
    case RTYPE_C_Link:
	if (RTYPE_C_RefUV == xKeyMapType) {
	    rtLINK_CType *pKeyMapLC = rtLINK_ToConstructor (pKeyMap);
	    pNewMapCPD = rtLINK_ColumnProjection (pKeyMapLC);
	    pKeyMapLC->release ();
	}
	break;
    case RTYPE_C_RefUV:
	if (RTYPE_C_Link == xKeyMapType) {
	    M_CPD *pLStoreCPD;
	    getListStore (pLStoreCPD);
	    pNewMapCPD = rtINDEX_PartitionedMapAsProductMap (pKeyMap, pLStoreCPD);
	    pLStoreCPD->release ();
	}
	break;
    default:
	break;
    }
    if (pNewMapCPD) {
	EnableModifications ();
	StoreReference (rtINDEX_CPx_Map, pNewMapCPD);
	pNewMapCPD->release ();
    }
    pKeyMap->release ();
}


/********************************
 *****  Key Set Correction  *****
 ********************************/

/*---------------------------------------------------------------------------
 ***** Private Routine to ensure that an index's key meets the definition of
 ***** a vector/uvector set.
 *****/
void rtINDEX_Handle::EnsureIndexKeyMeetsSetDefinition () {

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
    M_CPD *pKeySet;
    getKeySet (pKeySet);

    rtLINK_CType *refNilsLC = 0;
    switch (pKeySet->RType ()) {
    case RTYPE_C_Vector:
/*****  If the key set is valid, do a simple alignment...  *****/
	if (static_cast<rtVECTOR_Handle*>(pKeySet->containerHandle ())->isASet (&refNilsLC))
	    align ();

/*****  If the key set can be fixed by deleting its nil elements, do so...  *****/
	else if (IsntNil (refNilsLC)) {
	    rtPTOKEN_CType *pKeySetDomainAdjustment = refNilsLC->RPT ()->makeAdjustments ();
	    unsigned int sKeySetDomainAdjustment = 0;

	    rtLINK_TraverseRRDCList (refNilsLC, no_op, handleRepeat, handleRange);
	    refNilsLC->release ();

	    pKeySetDomainAdjustment->ToPToken ()->discard ();

	    if (static_cast<rtVECTOR_Handle*>(pKeySet->containerHandle ())->restoreSetAttribute ())
		align (true);
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
	if (UV_CPD_IsASetUV (pKeySet))
	    align ();

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
		pKeySet->RTypeName ()
	    )
	);
	break;
    }
    pKeySet->release ();

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
 *  Returns:
 *	NOTHING
 *
 *****/
void rtINDEX_Handle::deleteUnusedKeys () {

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
    EnsureIndexKeyMeetsSetDefinition ();

/*****  ... get the key projection of the presence map, ...  *****/
    M_CPD *pKeyMap;
    getKeyMap (pKeyMap);
    rtLINK_CType *pKeySelectorLC = 0;
    switch (pKeyMap->RType ()) {
    case RTYPE_C_Link: {
	    rtLINK_CType *pKeyMapLC = rtLINK_ToConstructor (pKeyMap);

	    M_CPD *pKeyEnumerationCPD = rtLINK_ColumnProjection (pKeyMapLC);
	    pKeyMapLC->release ();

	    pKeySelectorLC = rtLINK_RefUVToConstructor (pKeyEnumerationCPD);
	    pKeyEnumerationCPD->release ();
	}
	break;

    case RTYPE_C_RefUV:
	pKeySelectorLC = rtLINK_RefUVToConstructor (pKeyMap);
	break;

    default:
	ERR_SignalFault (
	    EC__InternalInconsistency, UTIL_FormatMessage (
		"rtINDEX_Handle::deleteUnusedKeys: Invalid Index Map Type: %s",
		pKeyMap->RTypeName ()
	    )
	);
	break;
    }

/*****  Prepare the key ptoken for editing  *****/
    rtPTOKEN_Handle *pKeyPToken = pKeySelectorLC->RPT ();
    rtPTOKEN_CType *pKeyPTokenConstructor = pKeyPToken->makeAdjustments ();

/*****  Traverse the key selector, editing the column ptoken  *****/
    int totalAdjustment = 0;
    int adjustment = 0;
    int lastColumn = 0;
    rtLINK_TraverseRRDCList (pKeySelectorLC, handleNil, handleRepeat, handleRange);
    handleNil (0, 0, (int)pKeyPToken->cardinality ());

/*****  Commit the changes  *****/
    pKeyPTokenConstructor->ToPToken ()->discard ();
    pKeySelectorLC->release ();

    pKeyMap->release ();

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
 *	pAdditionPPT		- a standard CPD for a P-Token which
 *				  specifies the number of rows to be added.
 *
 *  Returns:
 *	An link constructor referencing the rows added to the index.  The
 *	positional P-Token of this link constructor will be 'newListsPToken'.
 *
 *****/
rtLINK_CType *rtINDEX_Handle::addInstances_(rtPTOKEN_Handle *pAdditionPPT) {
    EnsureIndexKeyMeetsSetDefinition ();

    Vdd::Store::Reference pListStore;
    getListStore (pListStore);
    if (isATimeSeries ()) {
	rtPTOKEN_Handle::Reference pPPT (pListStore->getPToken ());
	if (!pPPT->isIndependent ())
	    return 0;
    }
    return pListStore->addInstances (pAdditionPPT);
}


/*************************************
 *****  List Cardinality Access  *****
 *************************************/

bool rtINDEX_Handle::getCardinality (M_CPD *&rpResult, rtLINK_CType *pSubscript) {
    rtLSTORE_Handle::Reference pListStore;
    getListStore (pListStore);
    return pListStore->getCardinality (rpResult, pSubscript);
}

bool rtINDEX_Handle::getCardinality (unsigned int &rpResult, DSC_Scalar &rSubscript) {
    rtLSTORE_Handle::Reference pListStore;
    getListStore (pListStore);
    return pListStore->getCardinality (rpResult, rSubscript);
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

bool rtINDEX_Handle::getListElements (
    DSC_Descriptor &rResult, rtLINK_CType *pInstances, M_CPD *pSubscript, int iModifier, rtLINK_CType **ppGuard
) {
//  Check integrity ...
    EnsureIndexKeyMeetsSetDefinition ();

//  ... and do the operation:
    rtLSTORE_Handle::Reference pListStore;
    getListStore (pListStore);
    return pListStore->getListElements (rResult, pInstances, pSubscript, iModifier, ppGuard);
}

bool rtINDEX_Handle::getListElements (
    DSC_Descriptor &rResult, DSC_Scalar &rInstance, int xSubscript, int iModifier
) {
//  Check integrity ...
    EnsureIndexKeyMeetsSetDefinition ();

//  ... and do the operation:
    rtLSTORE_Handle::Reference pListStore;
    getListStore (pListStore);
    return pListStore->getListElements (rResult, rInstance, xSubscript, iModifier);
}

bool rtINDEX_Handle::getListElements (
    DSC_Descriptor &rResult, DSC_Pointer &rInstances, DSC_Descriptor &rSubscript, int iModifier, rtLINK_CType **ppGuard
) {
//  Check integrity ...
    EnsureIndexKeyMeetsSetDefinition ();

//  ... and do the operation:
    rtLSTORE_Handle::Reference pListStore;
    getListStore (pListStore);
    return pListStore->getListElements (rResult, rInstances, rSubscript, iModifier, ppGuard);
}


/****************************
 *****  Element Access  *****
 ****************************/

bool rtINDEX_Handle::getListElements (
    DSC_Pointer		&rInstances,
    Vdd::Store::Reference&rpElementStore,
    rtLINK_CType	*&rpElementPointer,
    rtLINK_CType	*&rpExpansion,
    M_CPD		*&rpDistribution,
    rtINDEX_Key		*&rpKey
) {
    ExtractCount++;

/*****  Check its integrity ... *****/
    EnsureIndexKeyMeetsSetDefinition ();

/*****  Extract elements from the l-store ... *****/
    rtLSTORE_Handle::Reference pListStore;
    getListStore (pListStore);
    bool bDone = pListStore->getListElements (
	rInstances, rpElementStore, rpElementPointer, rpExpansion, rpDistribution, rpKey
    );

/*****
 *  If the index isn't a time-series, then its keys are stored in a vector.
 *  As of now (6/3/87) the handling of this type of index has not been
 *  defined and no rtINDEX_Key can be created. Thus, cleanup and return.
 *****/
    if (!bDone || !isATimeSeries ()) {
	rpKey = NilOf (rtINDEX_Key *);
	return bDone;
    }

/*****  Get a selector for the keys ... *****/
    M_CPD *pKeySelectorCPD; {
	M_CPD *pKeyMap;
	getKeyMap (pKeyMap);
	switch (pKeyMap->RType ()) {
	case RTYPE_C_Link: {
		rtLINK_CType *pMapSelectorLC = rtLINK_LCExtract (
		    pKeyMap, rpElementPointer
		);

	    /*****  Get the column projection ... *****/
		pKeySelectorCPD = rtLINK_ColumnProjection (pMapSelectorLC);
		pMapSelectorLC->release ();
	    }
	    break;

	case RTYPE_C_RefUV:
	    pKeySelectorCPD = rtREFUV_LCExtract (pKeyMap, rpElementPointer);
	    break;

	default:
	    ERR_SignalFault (
		EC__InternalInconsistency, UTIL_FormatMessage (
		    "rtINDEX_Extract: Invalid Index Map Type: %s",
		    pKeyMap->RTypeName ()
		)
	    );
	    break;
	}
	pKeyMap->release ();
    }

/*****  Make the new key ... *****/
    DSC_Descriptor newKeyDsc; {
	Vdd::Store::Reference pKeyStore;
	getKeyStore (pKeyStore);

	M_CPD *pKeySet;
	getKeySet (pKeySet);

	/*** Make a descriptor from the index's key store and set... ***/
	DSC_Descriptor keyDsc;
	keyDsc.constructMonotype (pKeyStore, pKeySet);
	pKeySet->release ();

	/***  Extract using 'pKeySelectorCPD' ... ***/
	newKeyDsc.constructComposition (pKeySelectorCPD, keyDsc);
	pKeySelectorCPD->release ();

	keyDsc.clear ();
    }

    /***  Distribute if neccessary ... ***/
    if (rpDistribution)
	newKeyDsc.distribute (rpDistribution);

    /*** And make the new key ... ***/
    rtPTOKEN_Handle::Reference ptoken (newKeyDsc.PPT ());
    rpKey = new rtINDEX_Key (ptoken, &newKeyDsc);

    newKeyDsc.clear ();

    return true;
}


/**************************************
 *****  Time Zero Access Routine  *****
 **************************************/

/*---------------------------------------------------------------------------
 *****  Routine to lookup point(s) in an index.
 *
 *  Arguments:
 *	rResult			- the address of a descriptor which will be set to
 *				  contain the point(s) found.
 *	pResultPPT		- the address of the positional p-token of the result.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
bool rtINDEX_Handle::getTimeZero (
    DSC_Descriptor &rResult, rtPTOKEN_Handle *pResultPPT
) {
/*****  ... validate that it is a time-series, ...  *****/
    if (!isATimeSeries ()) ERR_SignalFault (
	EC__InternalInconsistency, "rtINDEX_TimeZero: Not A Time-Series"
    );

/*****  ... get the key set, ...  *****/
    M_CPD *pKeySet;
    getKeySet (pKeySet);

/*****  ... get the key store, ...  *****/
    Vdd::Store::Reference pKeyStore;
    getKeyStore (pKeyStore);

/*****  ... construct the result, ...  *****/
    rResult.constructZero (
	pKeyStore, pKeySet->RType (), pResultPPT, static_cast<rtUVECTOR_Handle*>(
	    pKeySet->containerHandle ()
	)->rptHandle ()
    );

/*****  ... and cleanup and return.  *****/
    pKeySet->release ();

    return true;
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
    rtINDEX_Operation (rtINDEX_Handle *pIndex);

/*****  Destruction  *****/
    ~rtINDEX_Operation ();

/*****  Component Access Helpers  *****/
protected:
    Vdd::Store *IndexElements () const {
	return m_pIndexElements;
    }

/*****  Fault Generation Helpers  *****/
protected:
    void SignalKeySetTypeFault (char const* pWhereDescription) const;

    void SignalKeyMapTypeFault (char const* pWhereDescription) const;

/*****  State  *****/
protected:
    rtINDEX_Handle::Reference const	m_pIndex;
    rtLSTORE_Handle::Reference		m_pIndexLists;
    Vdd::Store::Reference		m_pIndexElements;
    M_CPD *m_pKeySet;
    M_CPD *m_pKeyMapCPD;
};


/**************************
 *****  Construction  *****
 **************************/

rtINDEX_Operation::rtINDEX_Operation (rtINDEX_Handle *pIndex) : m_pIndex (pIndex) {
/*****  Check the integrity of the index, ... *****/
    pIndex->EnsureIndexKeyMeetsSetDefinition ();

/*****  ... and unpack and cache its components:  *****/
    pIndex->getListStore (m_pIndexLists);
    m_pIndexLists->getContent (m_pIndexElements);

    pIndex->getKeySet (m_pKeySet);
    pIndex->getKeyMap (m_pKeyMapCPD);
}

/*************************
 *****  Destruction  *****
 *************************/

rtINDEX_Operation::~rtINDEX_Operation () {
    m_pKeySet->release ();
    m_pKeyMapCPD->release ();
}


/******************************
 *****  Fault Generation  *****
 ******************************/

void rtINDEX_Operation::SignalKeySetTypeFault (char const *pWhereDescription) const {
    ERR_SignalFault (
	EC__InternalInconsistency,
	UTIL_FormatMessage (
	    "rtINDEX::%s:%s: Invalid Key Set Type: %s",
	    WhatIAmDoingDescription (),
	    pWhereDescription,
	    m_pKeySet->RTypeName ()
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
	    m_pKeyMapCPD->RTypeName ()
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
    rtPTOKEN_Handle *OperationPPT () const {
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
) : rtINDEX_Operation	(static_cast<rtINDEX_Handle*>(iIndexDescriptor.store ()))
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
) {
    switch (m_pKeySet->RType ()) {
    case RTYPE_C_Vector: {
	    DSC_Pointer locationsPointer;
	    DSC_Pointer foundPointer;
	    static_cast<rtVECTOR_Handle*>(m_pKeySet->containerHandle ())->locate (
		OperationKeySet (), xLookupCase, locationsPointer, foundPointer
	    );
	    pKeySetDomainReferenceGuard = foundPointer.isEmpty ()
		? NilOf (rtLINK_CType*) : (rtLINK_CType*)DSC_Pointer_Link (foundPointer);
	    pKeySetDomainReference = DSC_Pointer_Link (locationsPointer);
	}
        break;
    case RTYPE_C_CharUV:
	rtCHARUV_Lookup (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_DoubleUV:
	rtDOUBLEUV_Lookup (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_FloatUV:
	rtFLOATUV_Lookup (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_IntUV:
	rtINTUV_Lookup (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_Unsigned64UV:
	rtU64UV_Lookup (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_Unsigned96UV:
	rtU96UV_Lookup (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    xLookupCase,
	    &pKeySetDomainReference,
	    &pKeySetDomainReferenceGuard
	);
        break;
    case RTYPE_C_Unsigned128UV:
	rtU128UV_Lookup (
	    m_pKeySet,
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
    if (RTYPE_C_Vector == m_pKeySet->RType ()) {
	DSC_Pointer locationsPointer;
	DSC_Pointer locatedPointer;
	static_cast<rtVECTOR_Handle*>(m_pKeySet->containerHandle ())->locate (
	    OperationKeySet (), xLookupCase, locationsPointer, locatedPointer
	);
	fKeySetDomainReferenceValid = locationsPointer.isntEmpty ();
	iKeySetDomainReference = DSC_Pointer_Scalar (locationsPointer);
	locatedPointer.clear ();
    }
    else {
	int xKeySetDomainReference;
	switch (m_pKeySet->RType ()) {
	case RTYPE_C_CharUV:
	    fKeySetDomainReferenceValid = rtCHARUV_ScalarLookup (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Char (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_DoubleUV:
	    fKeySetDomainReferenceValid = rtDOUBLEUV_ScalarLookup (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Double (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_FloatUV:
	    fKeySetDomainReferenceValid = rtFLOATUV_ScalarLookup (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Float (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_IntUV:
	    fKeySetDomainReferenceValid = rtINTUV_ScalarLookup (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Int (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned64UV:
	    fKeySetDomainReferenceValid = rtU64UV_ScalarLookup (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Unsigned64 (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned96UV:
	    fKeySetDomainReferenceValid = rtU96UV_ScalarLookup (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Unsigned96 (OperationKeySet ()),
		xLookupCase,
		&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned128UV:
	    fKeySetDomainReferenceValid = rtU128UV_ScalarLookup (
		m_pKeySet,
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

	if (fKeySetDomainReferenceValid) {
	    iKeySetDomainReference.constructReference (
		static_cast<rtUVECTOR_Handle*>(m_pKeySet->containerHandle ())->pptHandle (),
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
) {
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

    iKeySetDomainReference.destroy ();

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

    iKeySetDomainReference.destroy ();

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

    iKeySetDomainReference.destroy ();

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

    iKeySetDomainReference.destroy ();

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
	    pKeySetDomainReference->RPT ()->cardinality (),
	    &pKeyMapDomainReference,
	    &pKeyMapDomainReferenceGuard
	);
	pKeyMapComainReferenceLC->release ();
    }
    else {
	rtREFUV_PartitionedLocate (
	    m_pKeyMapCPD,
	    m_pIndexLists->breakpointArray (),
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

    if (fKeyMapComainIsAProduct) {
	rtLINK_LookupUsingLCKey (
	    m_pKeyMapCPD,
	    pKeyMapComainReferenceLC,
	    rtLINK_LookupCase_GT == xLookupCase ? rtLINK_LookupCase_GE : xLookupCase,
	    iKeySetDomainReference.RPTCardinality (),
	    &pKeyMapDomainReference,
	    &pKeyMapDomainReferenceGuard
	);
	pKeyMapComainReferenceLC->release ();
    }
    else {
	rtREFUV_PartitionedLocate (
	    m_pKeyMapCPD,
	    m_pIndexLists->breakpointArray (),
	    OperationIndexReferenceLC (),
	    iKeyMapComainReference,
	    rtLINK_LookupCase_GT == xLookupCase ? rtLINK_LookupCase_GE : xLookupCase,
	    pKeyMapDomainReference,
	    pKeyMapDomainReferenceGuard
	);
	iKeyMapComainReference.destroy ();
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
	iKeySetDomainReference.RPTCardinality (),
	&iKeyMapDomainReference
    ) : rtREFUV_PartitionedLocate (
	m_pKeyMapCPD,
	m_pIndexLists->breakpointArray (),
	OperationIndexReference (),
	iKeyMapComainReference,
	rtLINK_LookupCase_GT == xLookupCase ? rtLINK_LookupCase_GE : xLookupCase,
	iKeyMapDomainReference
    );

    iKeyMapComainReference.destroy ();
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
    
	switch (m_pKeyMapCPD->RType ()) {
	case RTYPE_C_Link:
	    fKeyMapComainIsAProduct = true;
	    m_pKeyMapCPD->align (); {
		rtPTOKEN_Handle::Reference pKeyMapComainPToken (
		    static_cast<rtLINK_Handle*>(m_pKeyMapCPD->containerHandle ())->rptHandle ()
		);
		pKeyMapComainReferenceCPD = rtLINK_LinearizeLCrUVic (
		    pKeyMapComainPToken,
		    OperationIndexReferenceLC (),
		    pPartialKeySetReferences,
		    OperationKeySetReference ()
		);
		pKeyMapComainReferenceLC = rtLINK_RefUVToConstructor (
		    pKeyMapComainReferenceCPD, &pKeyMapComainDistribution, true
		);
		pKeyMapComainReferenceCPD->release ();

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
    else switch (m_pKeyMapCPD->RType ()) {
    case RTYPE_C_Link:
	fKeyMapComainIsAProduct = true;
	m_pKeyMapCPD->align (); {
	    rtPTOKEN_Handle::Reference pKeyMapComainPToken (
		static_cast<rtLINK_Handle*>(m_pKeyMapCPD->containerHandle ())->rptHandle ()
	    );

	    pKeyMapComainReferenceCPD = rtLINK_LinearizeLCrLCic (
		pKeyMapComainPToken,
		OperationIndexReferenceLC (),
		pKeySetDomainReference,
		OperationKeySetReference ()
	    );

	    pKeyMapComainReferenceLC = rtLINK_RefUVToConstructor (
		pKeyMapComainReferenceCPD, &pKeyMapComainDistribution, true
	    );

	    pKeyMapComainReferenceCPD->release ();

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
    switch (m_pKeyMapCPD->RType ())
    {
    case RTYPE_C_Link:
	fKeyMapComainIsAProduct = true;
	m_pKeyMapCPD->align (); {
	    rtPTOKEN_Handle::Reference pKeyMapComainPToken (
		static_cast<rtLINK_Handle*>(m_pKeyMapCPD->containerHandle ())->rptHandle ()
	    );
	    pKeyMapComainReferenceLC = rtLINK_LinearizeLCrRc (
		pKeyMapComainPToken, OperationIndexReferenceLC (), &iKeySetDomainReference
	    );
	}
	break;

    case RTYPE_C_RefUV:
	fKeyMapComainIsAProduct = false;
	pKeyMapComainReferenceLC = NilOf (rtLINK_CType *);
	iKeyMapComainReference.constructFrom (iKeySetDomainReference);
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
    switch (m_pKeyMapCPD->RType ())
    {
    case RTYPE_C_Link:
	fKeyMapComainIsAProduct = true;
	m_pKeyMapCPD->align (); {
	    rtPTOKEN_Handle::Reference pKeyMapComainPToken (
		static_cast<rtLINK_Handle*>(m_pKeyMapCPD->containerHandle ())->rptHandle ()
	    );
	    rtREFUV_LinearizeRrRc (
		&iKeyMapComainReference,
		pKeyMapComainPToken,
		&OperationIndexReference (),
		&iKeySetDomainReference
	    );
	}
	break;

    case RTYPE_C_RefUV:
	fKeyMapComainIsAProduct = false;
	iKeyMapComainReference.constructFrom (iKeySetDomainReference);
	break;

    default:
	SignalKeyMapTypeFault ("ConvertKeySetReferenceToKeyMapComainReference");
	break;
    }
}


/****************************************************
 *****  Nil Element Reference Result Generator  *****
 ****************************************************/

void rtINDEX_KeyedOperation::SetResultToNilElementReference () {
    rtPTOKEN_Handle::Reference pPPT (OperationPPT ());
    rtPTOKEN_Handle::Reference pRPT (m_pIndexElements->getPToken ());

    m_iResultDescriptor.constructReferenceConstant (
	pPPT, IndexElements (), pRPT, pRPT->cardinality ()
    );
}


/**********************************************
 *****  Interval Limit Result Generators  *****
 **********************************************/

void rtINDEX_KeyedOperation::SetResultToIntervalLimit (
    rtLINK_CType	*pKeyMapDomainReference,
    rtLINK_CType	*pKeyMapDomainReferenceGuard,
    M_CPD		*pKeyMapDistribution
) {
    LookupMListLimitCount++;

    M_CPD *pKeySetDomainReference;
    switch (m_pKeyMapCPD->RType ()) {
    case RTYPE_C_Link: {
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
	Vdd::Store::Reference pKeyStore;
	m_pIndex->getKeyStore (pKeyStore);

	DSC_Descriptor iKeySetDescriptor;
	iKeySetDescriptor.constructMonotype (pKeyStore, m_pKeySet);
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
    switch (m_pKeyMapCPD->RType ()) {
    case RTYPE_C_Link: {
	    rtREFUV_Type_Reference iKeyMapComainReference;
	    rtLINK_RefExtract (
		&iKeyMapComainReference, m_pKeyMapCPD, &iKeyMapDomainReference
	    );

	    rtPTOKEN_Handle::Reference pKeySetDomainPToken (
		iKeyMapComainReference.RPT ()->colPTokenHandle ()
	    );

	    iKeySetDomainReference.constructReference (
		pKeySetDomainPToken,
		DSC_Scalar_Int (iKeyMapComainReference) % pKeySetDomainPToken->cardinality ()
	    );
	    iKeyMapComainReference.destroy ();
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
	Vdd::Store::Reference pKeyStore;
	m_pIndex->getKeyStore (pKeyStore);

	DSC_Descriptor iKeySetDescriptor;
	iKeySetDescriptor.constructMonotype (pKeyStore, m_pKeySet);
	m_iResultDescriptor.constructComposition (iKeySetDomainReference, iKeySetDescriptor);
	iKeySetDescriptor.clear ();
    }

    iKeySetDomainReference.destroy ();
}


/****************************************
 *****  Time Zero Result Generator  *****
 ****************************************/

void rtINDEX_KeyedOperation::SetResultToTimeZero () {
    rtPTOKEN_Handle::Reference pPosPToken (OperationPPT ());
    m_pIndex->getTimeZero (m_iResultDescriptor, pPosPToken);
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
    switch (m_pKeySet->RType ()) {
    case RTYPE_C_Vector: {
	    DSC_Pointer locationsPointer;
	    static_cast<rtVECTOR_Handle*>(m_pKeySet->containerHandle ())->locateOrAdd (
		OperationKeySet (), locationsPointer
	    );
	    pKeySetDomainReference = DSC_Pointer_Link (locationsPointer);
	}
	break;
    case RTYPE_C_CharUV:
	rtCHARUV_LocateOrAdd (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_DoubleUV:
	rtDOUBLEUV_LocateOrAdd (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_FloatUV:
	rtFLOATUV_LocateOrAdd (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_IntUV:
	rtINTUV_LocateOrAdd (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_Unsigned64UV:
	rtU64UV_LocateOrAdd (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_Unsigned96UV:
	rtU96UV_LocateOrAdd (
	    m_pKeySet,
	    DSC_Descriptor_Value (OperationKeySet ()),
	    &pKeySetDomainReference
	);
	break;
    case RTYPE_C_Unsigned128UV:
	rtU128UV_LocateOrAdd (
	    m_pKeySet,
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
		m_pIndexLists->breakpointArray (),
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
		m_pIndexLists->kot ()->TheIntegerClass.PTokenHandle (),
		pKeyMapDomainReferenceDistribution
	    );
	    m_pIndexLists->alignUsingSelectedLists (OperationIndexReferenceLC (), pElementAddCounts);
	    pElementAddCounts->release ();
	    pKeyMapDomainReferenceAdditions->release ();
	}
    }
    pKeySetDomainReference->release ();

/*****  ... and initialize the result descriptor.  *****/
    m_iResultDescriptor.constructReference (
	IndexElements (), pKeyMapDomainReferenceDistribution, pKeyMapDomainReference
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
	switch (m_pKeyMapCPD->RType ()) {
	case RTYPE_C_Link:
	    m_pKeyMapCPD->align (); {
		rtPTOKEN_Handle::Reference pKeyMapComainPToken (
		    static_cast<rtLINK_Handle*>(m_pKeyMapCPD->containerHandle ())->rptHandle ()
		);
		rtLINK_CType *pKeyMapComainReferenceLC = rtLINK_LinearizeLCrRc (
		    pKeyMapComainPToken, OperationIndexReferenceLC (), &keySelectorRef
		);

		pKeyMapDomainReferences = rtLINK_LocateOrAddFromLC (
		    m_pKeyMapCPD, pKeyMapComainReferenceLC, &pKeyMapDomainAdditions
		);

		pKeyMapComainReferenceLC->release ();
	    }
	    break;

	case RTYPE_C_RefUV:
	    rtREFUV_PartitionedLocateOrAdd (
		m_pKeyMapCPD,
		m_pIndexLists->breakpointArray (),
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
		m_pIndexLists->kot ()->TheIntegerClass.PTokenHandle (),
		NilOf (M_CPD*)
	    );
	    m_pIndexLists->alignUsingSelectedLists (OperationIndexReferenceLC (), pElementAddCounts);
	    pElementAddCounts->release ();
	    pKeyMapDomainAdditions->release ();
	}
    }
    keySelectorRef.destroy ();

/*****  ... and initialize the result descriptor.  *****/
    m_iResultDescriptor.constructLink (IndexElements (), pKeyMapDomainReferences);
}


/*************************************
 *****  SKSL Delegation Handler  *****
 *************************************/

void rtINDEX_LocateOrAddOperation::ProcessSKSLCase () {
    DefineSKeySListCount++;

/*****  ... update the key set, ... *****/
    rtREFUV_Type_Reference keySelectorRef;
    DefineScalarKey (keySelectorRef);

/*****  ... update the key map, ... *****/
    rtREFUV_Type_Reference elementSelectorRef; {
	bool added;
	switch (m_pKeyMapCPD->RType ()) {
	case RTYPE_C_Link:
	    m_pKeyMapCPD->align (); {
		rtPTOKEN_Handle::Reference pKeyMapComainPToken (
		    static_cast<rtLINK_Handle*>(m_pKeyMapCPD->containerHandle ())->rptHandle ()
		);

		rtREFUV_Type_Reference cartesianKeyRef;
		rtREFUV_LinearizeRrRc (
		    &cartesianKeyRef, pKeyMapComainPToken, &OperationIndexReference (), &keySelectorRef
		);

		added = rtLINK_LocateOrAddFromRef (
		    m_pKeyMapCPD, &cartesianKeyRef, &elementSelectorRef
		);
		cartesianKeyRef.destroy ();
	    }
	    break;

	case RTYPE_C_RefUV:
	    rtREFUV_PartitionedLocateOrAdd (
		m_pKeyMapCPD,
		m_pIndexLists->breakpointArray (),
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
	if (added) 
	    m_pIndexLists->alignUsingSelectedLists (OperationIndexReference (), 1);
    }
    keySelectorRef.destroy ();

/*****  ... and initialize the result descriptor. *****/
    m_iResultDescriptor.constructScalar (IndexElements (), elementSelectorRef);
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
) {
    if (RTYPE_C_Vector == m_pKeySet->RType ()) {
	DSC_Pointer locationPointer;
	static_cast<rtVECTOR_Handle*>(m_pKeySet->containerHandle ())->locateOrAdd (
	    OperationKeySet (), locationPointer
	);
	rKeySetDomainReference = DSC_Pointer_Scalar (locationPointer);
    }
    else {
	int xKeySetDomainReference;
	switch (m_pKeySet->RType ()) {
	case RTYPE_C_CharUV:
	    rtCHARUV_ScalarLocateOrAdd (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Char (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_DoubleUV:
	    rtDOUBLEUV_ScalarLocateOrAdd (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Double (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_FloatUV:
	    rtFLOATUV_ScalarLocateOrAdd (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Float (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_IntUV:
	    rtINTUV_ScalarLocateOrAdd (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Int (OperationKeySet ()),
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned64UV:
	    rtU64UV_ScalarLocateOrAdd (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Unsigned64 (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned96UV:
	    rtU96UV_ScalarLocateOrAdd (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Unsigned96 (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	case RTYPE_C_Unsigned128UV:
	    rtU128UV_ScalarLocateOrAdd (
		m_pKeySet,
		&DSC_Descriptor_Scalar_Unsigned128 (OperationKeySet ()), 
		(int *)&xKeySetDomainReference
	    );
	    break;
	default:
	    SignalKeySetTypeFault ("DefineScalarKey");
	    break;
	}
	rKeySetDomainReference.constructReference (
	    rtPTOKEN_BasePToken (m_pKeySet, UV_CPx_PToken), xKeySetDomainReference
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

	m_iResultDescriptor.constructMonotype (IndexElements (), pKeyMapReferenceUV);
	m_iResultDescriptor.distribute (pKeyMapDistribution);

	pKeyMapReferenceUV->release ();
	pKeyMapDistribution->release ();
    }
    else m_iResultDescriptor.constructReference (
	IndexElements (), pKeyMapDistribution, pKeyMapDomainReference
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

	m_iResultDescriptor.constructMonotype (IndexElements (), pKeyMapReferenceUV);
	pKeyMapReferenceUV->release ();
    }
    else {
	m_iResultDescriptor.constructLink (IndexElements (), pKeyMapDomainReference);
    }
}


/*************************************
 *****  SKSL Delegation Handler  *****
 *************************************/

void rtINDEX_LocateOperation::ProcessSKSLCase () {
    LookupSKeySListCount++;

    rtREFUV_Type_Reference iKeyMapDomainReference;
    if (LocateKeyAsKeyMapDomainReference (m_xLookupCase, iKeyMapDomainReference))
	m_iResultDescriptor.constructScalar (IndexElements (), iKeyMapDomainReference);
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
    if (LocateKeyAsKeyMapDomainReference (m_xLookupCase, iKeyMapDomainReference)) {
	SetResultToIntervalLimit (iKeyMapDomainReference);
	iKeyMapDomainReference.destroy ();
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
    void DeleteUnusedKeys () {
	m_pIndex->deleteUnusedKeys ();
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
	    m_pIndexLists->breakpointArray (),
	    OperationIndexReferenceLC (),
	    pKeyMapComainReferenceCPD
	);
	pKeyMapComainDistribution = NilOf (M_CPD*);

	pKeyMapComainReferenceCPD->release ();
    }

    if (pKeyMapComainDistribution)
	pKeyMapComainDistribution->release ();

    m_pIndexLists->align ();
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

	m_pIndexLists->align ();
    }
    else {
	rtREFUV_PartitionedDelete (
	    m_pKeyMapCPD,
	    m_pIndexLists->breakpointArray (),
	    OperationIndexReferenceLC (),
	    iKeyMapComainReference
	);
	iKeyMapComainReference.destroy ();

	m_pIndexLists->align ();
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
	iKeyMapComainReference.destroy ();

	m_pIndexLists->align ();
    }
    else {
	rtREFUV_PartitionedDelete (
	    m_pKeyMapCPD,
	    m_pIndexLists->breakpointArray (),
	    OperationIndexReference (),
	    iKeyMapComainReference
	);
	iKeyMapComainReference.destroy ();

	m_pIndexLists->align ();
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
	m_pKeySet, RTYPE_C_Vector == m_pKeySet->RType () ? rtVECTOR_CPx_PToken : UV_CPx_PToken
    );
}

void rtINDEX_DeleteKeyOperation::commit () {
    m_pKeySetDomainAdjustment->ToPToken ()->discard ();
    m_pKeySetDomainAdjustment = NilOf (rtPTOKEN_CType *);

    m_pIndex->align(true);
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

	    iKeySetDomainReference.destroy ();
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

IOBJ_DefineNewaryMethod (NewDM) {
    return RTYPE_QRegister (rtINDEX_NewCluster (RTYPE_QRegisterPToken (parameterArray[0])));
}

IOBJ_DefineNewaryMethod (NewUsingContentPrototypeDM) {
    Vdd::Store::Reference pContentPrototype (RTYPE_QRegisterHandle (parameterArray[1])->getStore ());
    return RTYPE_QRegister (
	rtINDEX_NewCluster (RTYPE_QRegisterPToken (parameterArray[0]), pContentPrototype)
    );
}

IOBJ_DefineUnaryMethod (DisplayCountsDM) {
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


IOBJ_DefineUnaryMethod (InitCountsDM) {
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


IOBJ_DefineUnaryMethod (SetDefaultMapTypeToProductDM) {
    rtINDEX_KeyMapDefaultType = RTYPE_C_Link;
    return self;
}

IOBJ_DefineUnaryMethod (SetDefaultMapTypeToPartitionDM) {
    rtINDEX_KeyMapDefaultType = RTYPE_C_RefUV;
    return self;
}

IOBJ_DefineNilaryMethod (GetDefaultMapTypeDM) {
    return RTYPE_TypeIObject (rtINDEX_KeyMapDefaultType);
}


/************************
 *  'Instance' Methods  *
 ************************/

IOBJ_DefineUnaryMethod (AlignDM) {
    RTYPE_QRegisterCPD (self)->align ();

    return self;
}

IOBJ_DefineUnaryMethod (CompAlignDM) {
    RTYPE_QRegisterCPD (self)->alignAll (false);

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

FAC_DefineFacility {
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
