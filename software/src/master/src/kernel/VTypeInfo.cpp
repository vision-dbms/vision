/*****  VTypeInfo Implementation  *****/

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

#include "VTypeInfo.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VArgList.h"

#include "Vca_CompilerHappyPill.h"

#include "Vca_VInterfaceMember.h"

#include "Vca_VTypeInfoHolder.h"


/*******************************************
 *******************************************
 *****                                 *****
 *****  VTypeInfo::ParameterSignature  *****
 *****                                 *****
 *******************************************
 *******************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VTypeInfo::ParameterSignature::ParameterSignature (
    ParameterSignature const &rOther
) : ParameterSignatureBaseClass (rOther) {
}

/************************
 ************************
 *****  Comparison  *****
 ************************
 ************************/

bool VTypeInfo::ParameterSignature::operator< (ParameterSignature const &rOther) const {
    if (elementCount () < rOther.elementCount ())
	return true;
    if (elementCount () > rOther.elementCount ())
	return false;

    for (unsigned int xElement = 0; xElement < elementCount (); xElement++) {
	if (operator[] (xElement).referent () < rOther[xElement].referent ())
	    return true;
	if (operator[] (xElement).referent () > rOther[xElement].referent ())
	    return false;
    }

    return false;
}

bool VTypeInfo::ParameterSignature::operator<= (ParameterSignature const &rOther) const {
    if (elementCount () < rOther.elementCount ())
	return true;
    if (elementCount () > rOther.elementCount ())
	return false;

    for (unsigned int xElement = 0; xElement < elementCount (); xElement++) {
	if (operator[] (xElement).referent () < rOther[xElement].referent ())
	    return true;
	if (operator[] (xElement).referent () > rOther[xElement].referent ())
	    return false;
    }

    return true;
}

bool VTypeInfo::ParameterSignature::operator== (ParameterSignature const &rOther) const {
    if (elementCount () != rOther.elementCount ())
	return false;

    for (unsigned int xElement = 0; xElement < elementCount (); xElement++)
	if (operator[] (xElement).referent () != rOther[xElement].referent ())
	    return false;

    return true;
}

bool VTypeInfo::ParameterSignature::operator!= (ParameterSignature const &rOther) const {
    if (elementCount () != rOther.elementCount ())
	return true;

    for (unsigned int xElement = 0; xElement < elementCount (); xElement++)
	if (operator[] (xElement).referent () != rOther[xElement].referent ())
	    return true;

    return false;
}

bool VTypeInfo::ParameterSignature::operator>= (ParameterSignature const &rOther) const {
    if (elementCount () > rOther.elementCount ())
	return true;
    if (elementCount () < rOther.elementCount ())
	return false;

    for (unsigned int xElement = 0; xElement < elementCount (); xElement++) {
	if (operator[] (xElement).referent () > rOther[xElement].referent ())
	    return true;
	if (operator[] (xElement).referent () < rOther[xElement].referent ())
	    return false;
    }

    return true;
}

bool VTypeInfo::ParameterSignature::operator> (ParameterSignature const &rOther) const {
    if (elementCount () > rOther.elementCount ())
	return true;
    if (elementCount () < rOther.elementCount ())
	return false;

    for (unsigned int xElement = 0; xElement < elementCount (); xElement++) {
	if (operator[] (xElement).referent () > rOther[xElement].referent ())
	    return true;
	if (operator[] (xElement).referent () < rOther[xElement].referent ())
	    return false;
    }

    return false;
}


/*********************
 *********************
 *****  Display  *****
 *********************
 *********************/

void VTypeInfo::ParameterSignature::displayDescription (size_t sIndent) const {
    for (unsigned int xElement = 0; xElement < elementCount (); xElement++) {
	display ("%s\n%*s", xElement > 0 ? "," : "", sIndent, "");
	operator[] (xElement)->displayDescription (sIndent);
    }
}

void VTypeInfo::ParameterSignature::getNameList (VString &rResult) const {
    char const *pPrefix = "<";
    unsigned int xElement = 0;
    while (xElement < elementCount ()) {
	rResult << pPrefix;
	pPrefix = ",";
	operator[] (xElement++)->getName (rResult);
    }
    if (xElement > 0)
	rResult << ">";
}


/************************************************
 ************************************************
 *****                                      *****
 *****  VTypeInfo::StringBasedNameProvider  *****
 *****                                      *****
 ************************************************
 ************************************************/

class VTypeInfo::StringBasedNameProvider : public NameProvider {
    DECLARE_CONCRETE_RTTLITE (StringBasedNameProvider, NameProvider);

//  Construction
public:
    StringBasedNameProvider (VString const &rName) : m_iName (rName) {
    }

//  Destruction
private:
    ~StringBasedNameProvider () {
    }

//  Access
public:
    virtual VString const &name () const OVERRIDE {
	return m_iName;
    }

//  State
private:
    VString const m_iName;
};


/**************************************************
 **************************************************
 *****                                        *****
 *****  VTypeInfo::TypeInfoBasedNameProvider  *****
 *****                                        *****
 **************************************************
 **************************************************/

class VTypeInfo::TypeInfoBasedNameProvider : public NameProvider {
    DECLARE_CONCRETE_RTTLITE (TypeInfoBasedNameProvider, NameProvider);

//  Construction
public:
    TypeInfoBasedNameProvider (std::type_info const &rTypeInfo) : m_iRTTI (rTypeInfo) {
    }

//  Destruction
private:
    ~TypeInfoBasedNameProvider () {
    }

//  Access
public:
    virtual VString const &name () const OVERRIDE {
	if (m_iName.isEmpty ())
	    m_iName.setTo (m_iRTTI.name ());
	return m_iName;
    }

//  State
private:
    V::VRTTI	const	m_iRTTI;
    VString	mutable	m_iName;
};


/***********************
 ***********************
 *****             *****
 *****  VTypeInfo  *****
 *****             *****
 ***********************
 ***********************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VTypeInfo);

/********************
 ********************
 *****  Ground  *****
 ********************
 ********************/

VkUUID const &VTypeInfo::groundUUID () {
// {F5B4BCE9-FA0D-469f-903E-5EFEAA362FE3}
    static VkUUIDHolder const s_iGroundUUID (
	0xf5b4bce9, 0xfa0d, 0x469f, 0x90, 0x3e, 0x5e, 0xfe, 0xaa, 0x36, 0x2f, 0xe3
    );
    return s_iGroundUUID;
}

VTypeInfo *VTypeInfo::ground () {
    static Reference s_pGround;

    if (s_pGround.isNil ()) {
	VString const iDash ("-", false);
	Reference const pGround (new VTypeInfo (0, iDash, groundUUID (), 0));
	s_pGround.interlockedSetIfNil (pGround);
    }

    return s_pGround;
}

VTypeInfo *VTypeInfo::ground (
    VString const &rName, uuid_t const &rUUID, VTypeInfo *pSuper
) {
    return ground ();
}

VTypeInfo *VTypeInfo::ground (
    VString const &rName, uuid_t const &rUUID, VTypeInfo *pSuper, ParameterSignature const &rParameterSignature
) {
    return ground ();
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VTypeInfo::VTypeInfo (
    VTypeInfo			*pScope,
    VString const		&rName,
    uuid_t const		&rUUID,
    VTypeInfo			*pSuper,
    ParameterSignature const	&rParameterSignature
)
: m_pScope		(pScope)
, m_pNameProvider	(new StringBasedNameProvider (rName))
, m_iUUID		(rUUID)
, m_pSuper		(pSuper)
, m_iParameters		(rParameterSignature)
, m_pHolderInstance	(0)
, m_pProxyFactory	(0)
{
}

VTypeInfo::VTypeInfo (
    VTypeInfo		*pScope,
    VString const	&rName,
    uuid_t const	&rUUID,
    VTypeInfo		*pSuper,
    va_list		 pParameters,
    unsigned int	 cParameters
)
: m_pScope		(pScope)
, m_pNameProvider	(new StringBasedNameProvider (rName))
, m_iUUID		(rUUID)
, m_pSuper		(pSuper)
, m_iParameters		(cParameters)
, m_pHolderInstance	(0)
, m_pProxyFactory	(0)
{
//  'm_iParameters' is a 'const' data member that can't be constructed in this
//  constructor's ctor-list.  Cast off the 'const' to complete it's construction.
    V::VArgList iParameterList (pParameters);
    ParameterSignature &rParameters = const_cast<ParameterSignature&>(m_iParameters);
    for (unsigned int xParameter = 0; xParameter < cParameters; xParameter++)
	rParameters[xParameter].setTo (iParameterList.arg<VTypeInfo*>());
}

VTypeInfo::VTypeInfo (
    VTypeInfo		*pScope,
    VString const	&rName,
    uuid_t const	&rUUID,
    VTypeInfo		*pSuper
)
: m_pScope		(pScope)
, m_pNameProvider	(new StringBasedNameProvider (rName))
, m_iUUID		(rUUID)
, m_pSuper		(pSuper)
, m_pHolderInstance	(0)
, m_pProxyFactory	(0)
{
}

VTypeInfo::VTypeInfo (
    VTypeInfo		*pScope,
    std::type_info const&rTypeInfo,
    uuid_t const	&rUUID,
    VTypeInfo		*pSuper
)
: m_pScope		(pScope)
, m_pNameProvider	(new TypeInfoBasedNameProvider (rTypeInfo))
, m_iUUID		(rUUID)
, m_pSuper		(pSuper)
, m_pHolderInstance	(0)
, m_pProxyFactory	(0)
{
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VTypeInfo::~VTypeInfo () {
    m_pScope->uninstantiate (this);
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

Vca::VInterfaceMember const *VTypeInfo::interfaceMember (unsigned int xMember) const {
    return m_pHolderInstance ? m_pHolderInstance->interfaceMember (xMember) : 0;
}

Vca::VInterfaceMember const *VTypeInfo::interfaceMember (VString const &rMemberName, bool bRecursive) const {
    VInterfaceMember const *pMember = m_pHolderInstance ? m_pHolderInstance->interfaceMember (rMemberName) : 0;
    if (bRecursive) {
	VTypeInfo *pSuper = m_pSuper;
	while (!pMember && pSuper) {
	    pMember = pSuper->interfaceMember (rMemberName);
	    pSuper = pSuper->super ();
	}
    }
    return pMember;
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool VTypeInfo::specializes (VTypeInfo const *pOther) const {
    for (VTypeInfo const *pThis = this; pThis; pThis = pThis->super ())
	if (pThis == pOther)
	    return true;
    return false;
}


/************************************
 ************************************
 *****  Unification Predicates  *****
 ************************************
 ************************************/

bool VTypeInfo::operator< (VTypeInfo const *pOther) const {
    return this != pOther && (
	scope () < pOther->scope () || (
	    scope () == scope () && (
		uuid () < pOther->uuid () || (
		    uuid () == pOther->uuid () && m_iParameters < pOther->m_iParameters
		)
	    )
	)
    );
}

bool VTypeInfo::operator<= (VTypeInfo const *pOther) const {
    return this == pOther || scope () < pOther->scope () || (
	scope () == scope () && (
	    uuid () < pOther->uuid () || (
		uuid () == pOther->uuid () && m_iParameters <= pOther->m_iParameters
	    )
	)
    );
}

bool VTypeInfo::operator== (VTypeInfo const *pOther) const {
    return this == pOther || (
	scope () == pOther->scope ()
	&& uuid () == pOther->uuid ()
	&& m_iParameters == pOther->m_iParameters
    );
}

bool VTypeInfo::operator!= (VTypeInfo const *pOther) const {
    return this != pOther && (
	scope () != pOther->scope ()
	|| uuid () != pOther->uuid ()
	|| m_iParameters != pOther->m_iParameters
    );
}

bool VTypeInfo::operator>= (VTypeInfo const *pOther) const {
    return this == pOther || scope () > pOther->scope () || (
	scope () == pOther->scope () && (
	    uuid () > pOther->uuid () || (
		uuid () == pOther->uuid () && m_iParameters >= pOther->m_iParameters
	    )
	)
    );
}

bool VTypeInfo::operator> (VTypeInfo const *pOther) const {
    return this != pOther && (
	scope () > pOther->scope () || (
	    scope () == pOther->scope () && (
		uuid () > pOther->uuid () || (
		    uuid () == pOther->uuid () && m_iParameters > pOther->m_iParameters
		)
	    )
	)
    );
}


/*************************
 *************************
 *****  Unification  *****
 *************************
 *************************/

VTypeInfo *VTypeInfo::instance (VTypeInfo *pKey) {
    unsigned int xInstantiation;
    m_iNestedTypes.Insert (pKey, xInstantiation);
    return m_iNestedTypes[xInstantiation];
}

/********************************/

VTypeInfo *VTypeInfo::instance (
    VString const		&rName,
    uuid_t const		&rUUID,
    VTypeInfo			*pSuper,
    ParameterSignature const	&rParameterSignature
) {
    Reference pKey (
	new VTypeInfo (this, rName, rUUID, pSuper, rParameterSignature)
    );
    return instance (pKey);
}

VTypeInfo *VTypeInfo::instance (
    VString const	&rName,
    uuid_t const	&rUUID,
    VTypeInfo		*pSuper,
    va_list		 pParameters,
    unsigned int	 cParameters
) {
    Reference pKey (
	new VTypeInfo (this, rName, rUUID, pSuper, pParameters, cParameters)
    );
    return instance (pKey);
}

VTypeInfo *__cdecl VTypeInfo::instance (
    VString const	&rName,
    uuid_t const	&rUUID,
    VTypeInfo		*pSuper,
    unsigned int	 cParameters,
    ...
) {
    V_VARGLIST (pParameters, cParameters);
    return instance (
	rName, rUUID, pSuper, pParameters, cParameters
    );
}

VTypeInfo *VTypeInfo::instance (
    std::type_info const &rTypeInfo, uuid_t const &rUUID, VTypeInfo *pSuper
) {
    Reference pKey (new VTypeInfo (this, rTypeInfo, rUUID, pSuper));
    return instance (pKey);
}

VTypeInfo *VTypeInfo::instance (
    VString const &rName, uuid_t const &rUUID, VTypeInfo *pSuper
) {
    Reference pKey (new VTypeInfo (this, rName, rUUID, pSuper));
    return instance (pKey);
}

VTypeInfo *__cdecl VTypeInfo::groundInstance (
    VString const	&rName,
    uuid_t const	&rUUID,
    VTypeInfo		*pSuper,
    unsigned int	 cParameters,
    ...
) {
    V_VARGLIST (pParameters, cParameters);
    return ground ()->instance (
	rName, rUUID, pSuper, pParameters, cParameters
    );
}

void VTypeInfo::uninstantiate (VTypeInfo *pInstantiation) {
    unsigned int xInstantiation;
    if (pInstantiation->scope () == this && m_iNestedTypes.Locate (
	    pInstantiation, xInstantiation
	) && m_iNestedTypes[xInstantiation].referent () == pInstantiation
    ) m_iNestedTypes.Delete (pInstantiation);
}


/****************************
 ****************************
 *****  Proxy Creation  *****
 ****************************
 ****************************/

IVUnknown *VTypeInfo::proxy (VcaOIDX *pOIDX) {
//  If a proxy factory has already been identified, use it, ...
    if (m_pProxyFactory)
	return m_pProxyFactory->proxy (pOIDX);

//  Otherwise, find a superclass that can function the proxy factory:
    if (!m_pSuper)
	return NilOf (IVUnknown*);

    IVUnknown *pProxy = m_pSuper->proxy (pOIDX);
    m_pProxyFactory = m_pSuper->m_pProxyFactory;

    return pProxy;
}


/*********************
 *********************
 *****  Display  *****
 *********************
 *********************/

void VTypeInfo::displayDescription (size_t sIndent) const {
    VString iUUIDString;
    uuid ().GetString (iUUIDString);
    display ("%s(\"%s\")<", (char const*)iUUIDString, (char const*)name ());
    m_iParameters.displayDescription (sIndent + 2);
    display ("\n%*s>", sIndent, "");
    if (m_pSuper) {
        display (" : public ");
        m_pSuper->displayDescription (sIndent);
    }
}

void VTypeInfo::getName (VString &rResult) const {
    rResult << name ();
    m_iParameters.getNameList (rResult);
}
