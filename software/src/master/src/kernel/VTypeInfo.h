#ifndef VTypeInfo_Interface
#define VTypeInfo_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca.h"

#include "VReferenceable.h"

#include "VComponentReference.h"
#include "V_VString.h"

#include "VkDynamicArrayOf.h"
#include "VkSetOf.h"
#include "VkUUID.h"

/**************************
 *****  Declarations  *****
 **************************/

class IVUnknown;

namespace Vca {
    class VcaOIDX;

    class VInterfaceMember;

    class VTypeInfoHolderInstance;
}


/*************************
 *****  Definitions  *****
 *************************/

/***********************
 *----  VTypeInfo  ----*
 ***********************/

/**
 * A serializable type identifier.
 * Only one instance of this class will ever be created for each type (a rule enforced by the machinery for creating them). This allows consumers to compare two VTypeInfo pointers for equality, ensuring that the result tells about the types being compared, not just the pointers.
 * In essence, VTypeInfo provides run-time functionality analogous to compile-time C++ type comparison, amongst other things.
 */
class Vca_API VTypeInfo : public VReferenceable {
    DECLARE_CONCRETE_RTT (VTypeInfo, VReferenceable);

//  Aliases
public:
    /**
     * Convenience definition for use with proxy handling.
     */
    typedef Vca::VcaOIDX VcaOIDX;
    typedef Vca::VInterfaceMember VInterfaceMember;
    typedef Vca::VTypeInfoHolderInstance VTypeInfoHolderInstance;
    typedef VkUUIDHolder UUIDHolder;
    typedef V::uuid_t uuid_t;
    typedef V::VString VString;

//  Friends
    friend class Vca::VTypeInfoHolderInstance;

/*------------------------------*
 *----  class NameProvider  ----*
 *------------------------------*/
public:
    class Vca_API NameProvider : public VReferenceable {
	DECLARE_ABSTRACT_RTTLITE (NameProvider, VReferenceable);

    //  Construction
    protected:
	NameProvider () {
	}

    //  Destruction
    protected:
	~NameProvider () {
	}

    //  Access
    public:
	virtual VString const &name () const = 0;
    };
    class StringBasedNameProvider;
    class TypeInfoBasedNameProvider;

/*-------------------------------*
 *----  class DeepReference  ----*
 *-------------------------------*/
protected:
    //  'Reference's with instance unification predicates
    class Vca_API DeepReference : public Reference {
	DECLARE_FAMILY_MEMBERS (DeepReference, Reference);

    //  Construction
    public:
	DeepReference (VTypeInfo *pOther) : Reference (pOther) {
	}
	DeepReference () {
	}

    //  Unification predicates
    public:
	bool operator <  (VTypeInfo const *pOther) const {
	    return **this < pOther;
	}
	bool operator <= (VTypeInfo const *pOther) const {
	    return **this <= pOther;
	}
	bool operator == (VTypeInfo const *pOther) const {
	    return **this == pOther;
	}
	bool operator != (VTypeInfo const *pOther) const {
	    return **this != pOther;
	}
	bool operator >= (VTypeInfo const *pOther) const {
	    return **this >= pOther;
	}
	bool operator >  (VTypeInfo const *pOther) const {
	    return **this > pOther;
	}

    //  Update (needed by 'VkSetOf')
    public:
	ThisClass &operator= (ThisClass const &rOther) {
	    setTo (rOther);
	    return *this;
	}
	ThisClass &operator= (VTypeInfo *pValue) {
	    setTo (pValue);
	    return *this;
	}
    };

/*-----------------------------*
 *----  class NestedTypes  ----*
 *-----------------------------*/
protected:
    typedef VkSetOf<DeepReference,VTypeInfo*,VTypeInfo const*> NestedTypes;

/*------------------------------------*
 *----  class ParameterSignature  ----*
 *------------------------------------*/
//  (has unification predicates for arrays of unified types)
public:
    typedef VkDynamicArrayOf<Reference> ParameterSignatureBaseClass;
    class Vca_API ParameterSignature : public ParameterSignatureBaseClass {
	DECLARE_FAMILY_MEMBERS (ParameterSignature, ParameterSignatureBaseClass);

    //  Construction
    public:
	ParameterSignature (ParameterSignature const &rOther);
	ParameterSignature (unsigned int cElements) : VkDynamicArrayOf<Reference> (cElements) {
	}
	ParameterSignature () {
	}

    //  Comparison
    public:
	bool operator<  (ParameterSignature const &rOther) const;
	bool operator<= (ParameterSignature const &rOther) const;
	bool operator== (ParameterSignature const &rOther) const;
	bool operator!= (ParameterSignature const &rOther) const;
	bool operator>= (ParameterSignature const &rOther) const;
	bool operator>  (ParameterSignature const &rOther) const;

    //  Description
    public:
	void displayDescription (size_t sIndent) const;
	void getNameList (VString &rNames) const;
    };

/*-------------------------------*
 *----  VTypeInfo (finally)  ----*
 *-------------------------------*/

//  Ground
public:
    /**
     * Returns the VkUUID of the ground VTypeInfo.
     */
    static VkUUID const &groundUUID ();

    /**
     * Returns the ground VTypeInfo.
     */
    static VTypeInfo *ground (); 

    static VTypeInfo *ground (
	VString const		&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper
    );
    static VTypeInfo *ground (
	VString const		&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper,
	ParameterSignature const&rParameterSignature
    );

//  Construction
private:
    VTypeInfo (
	VTypeInfo		*pScope,
	VString const		&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper,
	ParameterSignature const&rParameterSignature
    );
    VTypeInfo (
	VTypeInfo		*pScope,
	VString const		&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper,
	va_list			 pParameters,
	unsigned int		 cParameters
    );
    VTypeInfo (
	VTypeInfo		*pScope,
	VString const		&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper
    );
    VTypeInfo (
	VTypeInfo		*pScope,
	std::type_info const	&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper
    );

    void setTypeInfoHolderInstanceTo (VTypeInfoHolderInstance *pHolderInstance) {
	m_pProxyFactory = m_pHolderInstance = pHolderInstance;
    }

//  Destruction
protected:
    ~VTypeInfo ();

//  Access
public:
    VInterfaceMember const *interfaceMember (unsigned int xMember) const;
    VInterfaceMember const *interfaceMember (VString const &rName, bool bRecursive = false) const;

    /**
     * Returns a human-readable representation of the type. Implies no notion of uniqueness.
     */
    VString const &name () const {
	return m_pNameProvider->name ();
    }
    /**
     * Returns the scope in which this type is defined.
     *
     * In the case that this VTypeInfo describes a nested class, this method will return the VTypeInfo for the class in which it is nested. In all other cases, this method will return the ground() VTypeInfo.
     *
     * @return the VTypeInfo for the scope in which this type is defined.
     */
    VTypeInfo *scope () const {
	return m_pScope;
    }
    /**
     * Returns the superclass of this type.
     *
     * @return the VTypeInfo for the superclass of this type.
     */
    VTypeInfo *super () const {
	return m_pSuper;
    }
    /**
     * Returns the UUID of this type.
     *
     * @return the VkUUID for this type.
     */
    VkUUID const &uuid () const {
	return m_iUUID;
    }
    /**
     * Returns the number of template parameters that this type accepts.
     *
     * For a basic type that is not templated, the valence is always zero.
     *
     * @return the number of parameters upon which this type is templated.
     */
    unsigned int valence () const {
	return m_iParameters.cardinality ();
    }

    /**
     * Returns the type signature of the template parameters for this type.
     *
     * For a basic type that is not templated, an empty set is returned.
     *
     * @return a ParameterSignature instance populated for this type.
     */
    ParameterSignature const &parameters () const {
	return m_iParameters;
    }
    /**
     * Returns the VTypeInfo describing a given template parameter.
     *
     * For a basic type that is not templated, don't use this.
     *
     * @param xParameter the zero-based position of the parameter being queried for.
     * @return the VTypeInfo representing the requested parameter.
     */
    Reference const &parameter (unsigned int xParameter) const {
	return m_iParameters[xParameter];
    }

//  Query
public:
    /**
     * Returns true if this VTypeInfo is the ground VTypeInfo.
     */
    bool isGround () const {
	return operator== (ground ());
    }

    /**
     * Returns true if this VTypeInfo's direct parent scope is the ground VTypeInfo.
     */
    bool isGroundScoped () {
	return m_pScope->isGround ();
    }

    /**
     * Returns true if the valence of this VTypeInfo is zero and it is ground-scoped.
     */
    bool isSimple () const {
	return valence () == 0 && m_pScope.isntNil () && m_pScope->isGround ();
    }

    /**
     * Used to determine if this type inherits from, or is, the given type.
     *
     * Semantically, this method determines whether or not this type is substitutable for pOther.
     *
     * @param pOther the type against which this type is compared.
     * @return true if this type is a subclass of, or is, the type described by pOther.
     */
    bool specializes (VTypeInfo const *pOther) const;

    /**
     * Used to determine if a given type inherits from, or is, this type.
     *
     * Semantically, this method determines whether or not the given type is substitutable for tyis type.
     *
     * @param pOther the type against which this type is compared.
     * @return true if the given type is a subclass of, or is, this type.
     */
    bool generalizes (VTypeInfo const *pOther) const {
	return pOther->specializes (this);
    }

    /**
     * Used to determine if this type inherits from the given type.
     * @copydetail specializes()
     */
    bool strictlySpecializes (VTypeInfo const *pOther) const {
	return this != pOther && specializes (pOther);
    }

    /**
     * Used to determine if a given type inherits from this type.
     * @copydetail generalizes()
     */
    bool strictlyGeneralizes (VTypeInfo const *pOther) const {
	return this != pOther && generalizes (pOther);
    }

//  Unification
public:
    /**
     * Provides strict ordering/comparison functions, not intended for human-readable ordering.
     */
    bool operator<  (VTypeInfo const *pOther) const;
    /**
     * @copybrief operator<()
     */
    bool operator<= (VTypeInfo const *pOther) const;
    /**
     * @copybrief operator<()
     */
    bool operator== (VTypeInfo const *pOther) const;
    /**
     * @copybrief operator<()
     */
    bool operator!= (VTypeInfo const *pOther) const;
    /**
     * @copybrief operator<()
     */
    bool operator>= (VTypeInfo const *pOther) const;
    /**
     * @copybrief operator<()
     */
    bool operator>  (VTypeInfo const *pOther) const;

protected:
    VTypeInfo *instance (VTypeInfo *pKey);

//  Instantiation
public:
    /**
     * Requests the unique VTypeInfo instance for a given type whose scope is this type.
     *
     * Returns the existing instance if one already exists, otherwise constructs a new one.
     *
     * @param rName the (mostly inconsequential) descriptive name of the requested type.
     * @param rUUID the unique identifier for the requested type.
     * @param pSuper the VTypeInfo instance for the superclass of the requested type.
     *
     * @return the VTypeInfo describing the requested type.
     */
    VTypeInfo *instance (
	std::type_info const	&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper = 0
    );

    /**
     * @copydoc instance(VString &,uuid_t &,VTypeInfo *)
     * @param rParameterSignature the (template) parameter signature of the requested type.
     */
    VTypeInfo *instance (
	VString const		&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper,
	ParameterSignature const&rParameterSignature
    );

    /**
     * @copydoc instance(VString &,uuid_t &,VTypeInfo *)
     * @param cParameters the number of items in the pParameters list.
     */
    VTypeInfo *__cdecl instance (
	VString const		&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper,
	unsigned int		 cParameters,
	...
    );

    /**
     * @copydoc instance(VString &,uuid_t &,VTypeInfo *,unsigned int,...)
     * @param pParameters the template parameter list of VTypeInfo instances.
     */
    VTypeInfo *instance (
	VString const		&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper,
	va_list			 pParameters,
	unsigned int		 cParameters
    );

    /**
     * @copydoc instance(VString &,uuid_t &,VTypeInfo *)
     */
    VTypeInfo *instance (
	VString const		&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper = 0
    );

    static VTypeInfo *groundInstance (
	VString const		&rNameProvider,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper,
	ParameterSignature const&rParameterSignature
    ) {
	return ground ()->instance (
	    rNameProvider, rUUID, pSuper, rParameterSignature
	);
    }
    static VTypeInfo *groundInstance (
	VString const		&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper,
	va_list			 pParameters,
	unsigned int		 cParameters
    ) {
	return ground ()->instance (
	    rName, rUUID, pSuper, pParameters, cParameters
	);
    }
    static VTypeInfo *__cdecl groundInstance (
	VString const		&rName,
	uuid_t const		&rUUID,
	VTypeInfo		*pSuper,
	unsigned int		 cParameters,
	...
    );
    template <typename name_provider_t> static VTypeInfo *groundInstance (
	name_provider_t const &rNameProvider, uuid_t const &rUUID, VTypeInfo *pSuper = 0
    ) {
	return ground ()->instance (rNameProvider, rUUID, pSuper);
    }

protected:
    void uninstantiate (VTypeInfo *pInstantiation);

//  Proxy Creation
public:
    IVUnknown *proxy (VcaOIDX *pOIDX);

//  Description
public:
    /**
     * Generates a human-readable description of this type.
     *
     * @param sIndent indent amount. Used for recursive calls of this method and should not be provided by consumers.
     */
    void displayDescription (size_t sIndent = 0) const;

    /**
     * Returns the human-readable name of this type.
     *
     * @param sIndent indent amount. Used for recursive calls of this method and should not be provided by consumers.
     */
    void getName (VString &rResult) const;

//  State
private:
    /**
     * The singleton VTypeInfoHolder instance for this VTypeInfo.
     *
     * Will always be set for compile-instantiated types. Will not be set for Vca-instantiated types.
     */
    VTypeInfoHolderInstance*		m_pHolderInstance;

    /**
     * The singleton VTypeInfoHolder instance for the most derived compiler-instantiated supertype that exists in this process for the described type.
     *
     * Will always be set for all VTypeInfo instances.
     */
    VTypeInfoHolderInstance*		m_pProxyFactory;

    /**
     * The type within which this type is nested or the ground scope if an non-nested class.
     */
    Reference			const	m_pScope;
    NameProvider::Reference	const	m_pNameProvider;
    VkUUIDHolder		const	m_iUUID;

    /**
     * The superclass of this type. Null if there is none.
     */
    Reference			const	m_pSuper;

    /**
     * The template parameter signature of this type.
     */
    ParameterSignature		const	m_iParameters;

    /**
     * All VTypeInfo instances whose scope is this type (nested types).
     */
    NestedTypes				m_iNestedTypes;
};


#endif
