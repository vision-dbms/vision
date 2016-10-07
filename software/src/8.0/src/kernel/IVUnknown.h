/**
 * @file
 * Defines a number of macros and a base class for Vca interfaces, regardless of what library the interfaces are in.
 */

#ifndef Vca_IVUnknown_Interface
#define Vca_IVUnknown_Interface

#ifndef Vca_IVUnknown
#define Vca_IVUnknown extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VcaOIDL.h"
#include "Vca_VcaOIDR.h"
#include "Vca_VcaSerializer.h"

#include "Vca_VInterfaceMember.h"

#include "Vca_VTypeInfoHolder.h"

namespace Vca {
    template<typename T> class IDataSource;
    template<typename T> class VTrigger;

    class VMessageManager;
    class VSiteInfo;
    class VRolePlayer;
}


/*********************
 *****  Helpers  *****
 *********************/

/********************************************
 *----  Interface Declaration  Helpers  ----*
 ********************************************/

/**
 * Forward-declares an interface and associated typeinfo template instantiations.
 * Automatically inserts the extern keyword as well as DECLSPEC_DLL{EXPORT,IMPORT} for template instantiations.
 * Must be called from outside any namespace.
 */
#define DECLARE_API_VINTERFACE(api,ifName)\
    DECLARE_API_VINTERFACE_AT_LEVEL(api,ifName,1)

#define DECLARE_API_VINTERFACE_AT_LEVEL(api,ifName,level)\
    api##_NAMESPACE_ENTER\
    class ifName;\
    api##_NAMESPACE_EXIT\
    VINTERFACE_TYPEINFOHOLDER_DECLARATIONS(extern,api##_API,api::ifName,level)

/**
 * Internal use only. Do not invoke.
 */
#define DECLARE_API_VINTERFACE_NOSCOPE(api,ifName)\
    DECLARE_API_VINTERFACE_NOSCOPE_AT_LEVEL(api,ifName,1)

#define DECLARE_API_VINTERFACE_NOSCOPE_AT_LEVEL(api,ifName,level)\
    class ifName;\
    VINTERFACE_TYPEINFOHOLDER_DECLARATIONS(extern,api##_API,ifName,level)

#define DECLARE_VINTERFACE(ifName)\
    class ifName


/******************************************
 *----  Interface Definition Helpers  ----*
 ******************************************/

/**
 * Declares an interface within a specific library.
 * Automatically handles tagging of the class with the appropriate keywords for import/export.
 *
 * @param ifName the name of the interface to declare.
 * @param ifBase the name of the interface's base class.
 * @param api the library within which the interface is declared.
 */
#define VINTERFACE_IN_API(ifName,ifBase,api)\
template <class ActorClass, class MDI> class ifName##_Role;\
class ABSTRACT api##_API VINTERFACE_DEFINITION(ifName,ifBase)

#define VINTERFACE_NEST_IN_API(ifName,ifBase,api)\
class ABSTRACT api##_API VINTERFACE_DEFINITION_NEST(ifName,ifBase)

/**
 * @warning Should not be used directly; should only be used for defining
 * interfaces that are not declared within a library.
 * @private
 */
#define VINTERFACE(ifName,ifBase) class ABSTRACT VINTERFACE_DEFINITION(ifName,ifBase)

/**
 * @warning Should not be used directly; should only be used for defining
 * interfaces that are not declared within a library.
 *
 * This macro expands to code that does NOT define 'template<typename ActorClass, typename MDI> class Role'.
 *
 * @private
 */
#define VINTERFACE_NEST(ifName,ifBase) class ABSTRACT VINTERFACE_DEFINITION_NEST(ifName,ifBase)

#define VINTERFACE_DEFINITION(ifName,ifBase) ifName : public ifBase {\
    DECLARE_ABSTRACT_RTTLITE(ifName,ifBase);\
    /** @cond VINTERFACEMEMBERS */\
    /** Descriptor class containing ifName role information. */\
    template <class Actor,class MDI> struct Role {\
        /** The role template for the ifName interface. */\
        typedef ifName##_Role<Actor,MDI> Implementation;\
    };\
    /** @endcond */\
    VINTERFACE_DEFINITION_MEMBERS (ifName)

#define VINTERFACE_DEFINITION_NEST(ifName,ifBase) ifName : public ifBase {\
    DECLARE_ABSTRACT_RTTLITE(ifName,ifBase);\
    VINTERFACE_DEFINITION_MEMBERS (ifName)

/**
 * Defines VTypeInfo and associated members for an interface.
 */
#define VINTERFACE_DEFINITION_MEMBERS(ifName)\
    /** @cond VINTERFACE_MEMBERS */\
public:\
    /** Vca::VSubscription template for this interface. */\
    typedef Vca::VSubscription<ThisClass> Subscription;\
    virtual Subscription* subscription () const {\
	return 0;\
    }\
    virtual ThisClass* target () {\
	return this;\
    }\
    /** Vca::VProxy_ template for this interface. */\
    typedef Vca::VProxy_<ThisClass> Proxy;\
protected:\
    ifName () {\
    }\
protected:\
    friend class Vca::VTypeInfoHolderForInterface<ThisClass>;\
    /** Holder for the type information for this interface. */\
    static Vca::VTypeInfoHolderForInterface<ThisClass> g_iTypeInfoHolder;\
    static VTypeInfo *typeInfoInstantiation ();\
public:\
    /** Returns the most derived type information for this interface. Virtual function forces instantiation of template type info holders. */\
    virtual Vca::VTypeInfoHolderInstance &typeInfoHolder_() const {\
        return g_iTypeInfoHolder;\
    }\
    /** Returns the type information for this interface. Non-virtual function provides type information for the interface as it is seen by the caller. */\
    static Vca::VTypeInfoHolderInstance &typeInfoHolder () {\
        return g_iTypeInfoHolder;\
    }\
    /** @endcond */\
    /** Returns the shared VTypeInfo (type descriptor) for this interface. */\
    static VTypeInfo *typeInfo () {\
        return g_iTypeInfoHolder.typeInfo_();\
    }

#define VINTERFACE_MEMBERIMPL(mname) MemberImpl_##mname
#define VINTERFACE_MEMBERINFO(mname) MemberInfo_##mname

/**
 * The type name for the metadata object for a particular interface member.
 */
#define VINTERFACE_MEMBERTYPE(mname) MemberType_##mname

/**
 * Instantiates <code>static MemberInfo_*</code> types for a member within an interface definition.
 * It is assumed that the caller will include a semicolon after using this macro (as though it were a function).
 *
 * Sample expansion of:
 *
 * @code
 * VINTERFACE_MEMBERINFO_DEFINITION(ISample, doSomething, 0);
 * @endcode
 *
 * Would be:
 *
 * @code
 * ISample::MemberType_doSomething ISample::MemberInfo_doSomething(doSomething, (0), &ISample::MemberImpl_doSomething);
 * @endcode
 *
 * @param ifName the name of the interface.
 * @param mname the name of the member.
 * @param mindex a small integer used to uniquely identify the member within the interface.
 */
#define VINTERFACE_MEMBERINFO_DEFINITION(ifName,mname,mindex)\
    /** Internal descriptor for ifName##::##mname. */\
    ifName::VINTERFACE_MEMBERTYPE(mname)\
    /* Initialize our memberinfo object using a direct constructor call. */\
    ifName::VINTERFACE_MEMBERINFO(mname)(#mname, (mindex), &ifName::VINTERFACE_MEMBERIMPL(mname))

/**
 * Defines an interface method taking no parameters.
 * Three pieces are defined here:
 *   -# A function that consumers can call to invoke the member.
 *   -# A static data member holding the metadata describing the member.
 *   -# An internal virtual whose override allows the member to be realized.
 *
 * @param mname the name of the method to be defined.
 */
#define VINTERFACE_METHOD_0(mname)\
public:\
    void mname (MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this, pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_0<\
        ThisClass\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)() {}\
    /** @endcond */

/**
 * Defines an interface method taking one parameter.
 * @copydetails VINTERFACE_METHOD_0
 * @param Pn the parameter type of the nth parameter for the method being declared.
 */
#define VINTERFACE_METHOD_1(mname,P1)\
public:\
    void mname (P1 p1,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_1<\
        ThisClass,P1\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1) {}\
    /** @endcond */

/**
 * Defines an interface method taking two parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_2(mname,P1,P2)\
public:\
    void mname (P1 p1,P2 p2,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_2<\
        ThisClass,P1,P2\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2) {}\
    /** @endcond */

/**
 * Defines an interface method taking three parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_3(mname,P1,P2,P3)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_3<\
        ThisClass,P1,P2,P3\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3) {}\
    /** @endcond */

/**
 * Defines an interface method taking four parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_4(mname,P1,P2,P3,P4)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_4<\
        ThisClass,P1,P2,P3,P4\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4) {}\
    /** @endcond */

/**
 * Defines an interface method taking five parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_5(mname,P1,P2,P3,P4,P5)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_5<\
        ThisClass,P1,P2,P3,P4,P5\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5) {}\
    /** @endcond */

/**
 * Defines an interface method taking six parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_6(mname,P1,P2,P3,P4,P5,P6)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,p6,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_6<\
        ThisClass,P1,P2,P3,P4,P5,P6\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5,P6) {}\
    /** @endcond */

/**
 * Defines an interface method taking seven parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_7(mname,P1,P2,P3,P4,P5,P6,P7)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,p6,p7,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_7<\
        ThisClass,P1,P2,P3,P4,P5,P6,P7\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5,P6,P7) {}\
    /** @endcond */

/**
 * Defines an interface method taking eight parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_8(mname,P1,P2,P3,P4,P5,P6,P7,P8)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,p6,p7,p8,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_8<\
        ThisClass,P1,P2,P3,P4,P5,P6,P7,P8\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5,P6,P7,P8) {}\
    /** @endcond */

/**
 * Defines an interface method taking nine parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_9(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,p6,p7,p8,p9,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_9<\
        ThisClass,P1,P2,P3,P4,P5,P6,P7,P8,P9\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5,P6,P7,P8,P9) {}\
    /** @endcond */

/**
 * Defines an interface method taking ten parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_10(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_10<\
        ThisClass,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10) {}\
    /** @endcond */

/**
 * Defines an interface method taking eleven parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_11(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_11<\
        ThisClass,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11) {}\
    /** @endcond */

/**
 * Defines an interface method taking twelve parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_12(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_12<\
        ThisClass,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12) {}\
    /** @endcond */

/**
 * Defines an interface method taking thirteen parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_13(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12,P13 p13, MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,pMgr);\
    } \
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_13<\
        ThisClass,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13) {}\
    /** @endcond */

/**
 * Defines an interface method taking fourteen parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_14(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12,P13 p13,P14 p14,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_14<\
        ThisClass,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14) {}\
    /** @endcond */

/**
 * Defines an interface method taking fifteen parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_15(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12,P13 p13,P14 p14,P15 p15,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_15<\
        ThisClass,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15) {}\
    /** @endcond */

/**
 * Defines an interface method taking sixteen parameters.
 * @copydetails VINTERFACE_METHOD_1
 */
#define VINTERFACE_METHOD_16(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16)\
public:\
    void mname (P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12,P13 p13,P14 p14,P15 p15,P16 p16,MGR *pMgr=0) {\
        VINTERFACE_MEMBERINFO(mname)(this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,pMgr);\
    }\
    /** @cond VINTERFACEMEMBERS */\
    /** Convenience definition for the VInterfaceMember templated for this class and appropriate parameters for mname##(). */\
    typedef Vca::VInterfaceMember_16<\
        ThisClass,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16\
    > VINTERFACE_MEMBERTYPE(mname);\
    static VINTERFACE_MEMBERTYPE(mname) VINTERFACE_MEMBERINFO(mname);\
private:\
    /** Delegation method for mname##(), overriden by role subclasses; translates message invocation into method call on implementor. The stock implementation of this method does nothing; the real implementation of this method is actually generated in role subclasses (Role::Implementation). */\
    virtual void VINTERFACE_MEMBERIMPL(mname)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16) {}\
    /** @endcond */

/**
 * Used to indicate termination of an interface declaration.
 * Intended to be used in conjuction with VINTERFACE_IN_API or one of the macros that use it.
 */
#define VINTERFACE_END };


/*************************************
 *----  Role Definition Helpers  ----*
 *************************************/

/**
 * Defines a Vca Interface Role for use with Vca::VRolePlayer. All interfaces require use of this.
 */
#define VINTERFACE_ROLE(ifName,ifBase)\
/** @cond VINTERFACE_MEMBERS */\
class ifName; template <class ActorClass, class MDI>\
/** Role for ifName interface; instances couple said interface with implementors. For details on how interfaces, roles, implementors and consumers are related, see @ref InterfacesExplained "Interfaces Explained". */\
class ifName##_Role : public ifBase::Role<ActorClass,MDI>::Implementation {\
public:\
    /** Convenience definition for this role's base class, the role for ifName. */\
    typedef typename ifBase::Role<ActorClass,MDI>::Implementation BaseClass;\
    USING (BaseClass::actor)\
protected:\
    /** Empty protected constructor simply prevents us from calling <code>new</code>. */\
    ifName##_Role () {\
    }\
private:

/**
 * Defines a Vca Interface Role, nested within the interface itself, for use with a Vca::VRolePlayer. Interfaces that take template parameters require use of this instead of VINTERFACE_ROLE. Unlike the latter, this is intended to be expanded from within an interface class definition and is not compatible with VINTERFACE expansions (which don't support templates anyway).
 * Should be followed immediately by VINTERFACE_ROLE_0 (or similar) expansions for each interface member and ultimately by a VINTERFACE_ROLE_END expansion.
 */
#define VINTERFACE_ROLE_NEST public:\
    /** @cond VINTERFACE_MEMBERS */\
    template <typename ActorClass, typename MDI> class Role {\
    public:\
	typedef typename BaseClass::Role<ActorClass,MDI>::Implementation ImplementationBase;\
	class Implementation : public ImplementationBase {\
	public:\
	    USING (ImplementationBase::actor)\
	protected:\
	    Implementation () {\
	    }\
	private:

#define VINTERFACE_ROLE_NEST_END }; /* class Implementation */\
    /** @endcond */\
    }; /* template <typename ActorClass, typename MDI> class Role*/

/**
 * Defines a role member taking zero arguments.
 * Intended to be used within a VRole subclass definition corresponding to a particular interface.
 *
 * @param mname the name of the member.
 */
#define VINTERFACE_ROLE_0(mname)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)() {\
    actor ()->mname (this);\
}

/**
 * Defines a role member taking one argument.
 * @copydetails VINTERFACE_ROLE_0
 * @param Pn the parameter type of the nth parameter for the method being declared.
 */
#define VINTERFACE_ROLE_1(mname,P1)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1) {\
    actor ()->mname (this,p1);\
}

/**
 * Defines a role member taking two arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_2(mname,P1,P2)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2) {\
    actor ()->mname (this,p1,p2);\
}

/**
 * Defines a role member taking three arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_3(mname,P1,P2,P3)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3) {\
    actor ()->mname (this,p1,p2,p3);\
}

/**
 * Defines a role member taking four arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_4(mname,P1,P2,P3,P4)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4) {\
    actor ()->mname (this,p1,p2,p3,p4);\
}

/**
 * Defines a role member taking five arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_5(mname,P1,P2,P3,P4,P5)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5) {\
    actor ()->mname (this,p1,p2,p3,p4,p5);\
}

/**
 * Defines a role member taking six arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_6(mname,P1,P2,P3,P4,P5,P6)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6) {\
    actor ()->mname (this,p1,p2,p3,p4,p5,p6);\
}

/**
 * Defines a role member taking seven arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_7(mname,P1,P2,P3,P4,P5,P6,P7)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7) {\
    actor ()->mname (this,p1,p2,p3,p4,p5,p6,p7);\
}

/**
 * Defines a role member taking eight arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_8(mname,P1,P2,P3,P4,P5,P6,P7,P8)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8) {\
    actor ()->mname (this,p1,p2,p3,p4,p5,p6,p7,p8);\
}

/**
 * Defines a role member taking nine arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_9(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9) {\
    actor ()->mname (this,p1,p2,p3,p4,p5,p6,p7,p8,p9);\
}

/**
 * Defines a role member taking ten arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_10(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10) {\
    actor ()->mname (this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10);\
}

/**
 * Defines a role member taking eleven arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_11(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11) {\
    actor ()->mname (this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11);\
}

/**
 * Defines a role member taking twelve arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_12(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12) {\
    actor ()->mname (this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12);\
}

/**
 * Defines a role member taking thirteen arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_13(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12,P13 p13) {\
    actor ()->mname (this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13);\
}

/**
 * Defines a role member taking fourteen arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_14(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12,P13 p13,P14 p14) {\
    actor ()->mname (this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14);\
}

/**
 * Defines a role member taking fifteen arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_15(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12,P13 p13,P14 p14,P15 p15) {\
    actor ()->mname (this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15);\
}

/**
 * Defines a role member taking sixteen arguments.
 * @copydetails VINTERFACE_ROLE_1
 */
#define VINTERFACE_ROLE_16(mname,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14,P15,P16)\
/** Delegation method for mname interface message; translates message invocation into method call on ActorClass. */\
private: void VINTERFACE_MEMBERIMPL(mname)(P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7,P8 p8,P9 p9,P10 p10,P11 p11,P12 p12,P13 p13,P14 p14,P15 p15,P16 p16) {\
    actor ()->mname (this,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16);\
}

#define VINTERFACE_ROLE_END /** @endcond */ };


/*****************************************
 *----  TypeInfo Definition Helpers  ----*
 *****************************************/

#define VINTERFACE_SYMBOL(ifName) G_iTypeInfoUUIDHolder_##ifName

#define VINTERFACE_SYMBOL_DECLARATION(ifName)\
VTypeInfo::UUIDHolder const VINTERFACE_SYMBOL(ifName)

#define VINTERFACE_SYMBOL_DEFINITION(ifName,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11)\
VTypeInfo::UUIDHolder const VINTERFACE_SYMBOL(ifName)(\
    i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11\
)

#define VINTERFACE_TYPEINFOHOLDER_DEFINITION(ifClass)\
Vca::VTypeInfoHolderForInterface<ifClass > ifClass::g_iTypeInfoHolder

/**
 * Initializes the singleton instance of an interface's metadata object as well as other type-specific information.
 * Must be called within the definition of every interface (typically from a .cpp).
 *
 * @param ifName the name of the interface.
 * @param i1 First four bytes of the UUID for the interface.
 * @param i2 Next two bytes of the UUID for the interface.
 * @param i3 Next two bytes of the UUID for the interface.
 * @param i4 Next byte of the UUID for the interface.
 * @param i5 Next byte of the UUID for the interface.
 * @param i6 Next byte of the UUID for the interface.
 * @param i7 Next byte of the UUID for the interface.
 * @param i8 Next byte of the UUID for the interface.
 * @param i9 Next byte of the UUID for the interface.
 * @param i10 Next byte of the UUID for the interface.
 * @param i11 Final byte of the UUID for the interface.
 */
#define VINTERFACE_TYPEINFO_DEFINITION(ifName,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11)\
    VINTERFACE_SYMBOL_DEFINITION(\
        ifName,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11\
    );\
    VINTERFACE_TYPEINFO (ifName)\
    VINTERFACE_TYPEINFO_PARAMETERS (ifName, 0)\
    VINTERFACE_TYPEINFO_END\
    VINTERFACE_TYPEINFOHOLDER_DEFINITION (ifName)

#define VINTERFACE_TYPEINFO(ifName) VTypeInfo *ifName

#define VINTERFACE_TYPEINFO_PARAMETERS(ifName,cParameters) ::typeInfoInstantiation () {\
    return VTypeInfo::groundInstance (\
        #ifName, VINTERFACE_SYMBOL(ifName),\
        Vca::VTypeInfoHolder<BaseClass*>::typeInfoNoException (),\
        (cParameters)

#define VINTERFACE_TYPEINFO_PARAMETER(p) ,Vca::VTypeInfoHolder<p >::typeInfo ()

#define VINTERFACE_TYPEINFO_END );}


/*************************************
 *----  Template Instantiations  ----*
 *************************************/

/**
 * Generates template instantiations for commonly used templates pertaining to interface declarations. Automatically qualifies the temlate instantiations with 'extern' and 'DECLSPEC_DLL_EXPORT' as appropriate based on related macros (api_ifName and api_API respectively).
 * This macro must be invoked in each interface header file and outside of any namespace.
 * Will not service interfaces nested within multiple namespaces; in such cases use VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API instead.
 */
#define VINTERFACE_TEMPLATE_INSTANTIATIONS(api,ifName)\
    VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API(api,api,ifName)

#define VINTERFACE_TEMPLATE_INSTANTIATIONS_AT_LEVEL(api,ifName,level)\
    VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API_AT_LEVEL(api,api,ifName,level)

/**
 * Generates template instantiations for commonly used templates pertaining to interface declarations. Automatically qualifies the temlate instantiations with 'extern' and 'DECLSPEC_DLL_EXPORT' as appropriate based on related macros (api_ifName and api_API respectively).
 * This macro must be invoked in each interface header file and outside of any namespace.
 * Will not service interfaces nested within a namespace; in such cases use VINTERFACE_TEMPLATE_INSTANTIATIONS instead.
 */
#define VINTERFACE_TEMPLATE_INSTANTIATIONS_NOSCOPE(api,ifName)\
    VINTERFACE_TEMPLATE_INSTANTIATIONS_NOSCOPE_AT_LEVEL(api,ifName,1)

#define VINTERFACE_TEMPLATE_INSTANTIATIONS_NOSCOPE_AT_LEVEL(api,ifName,level)\
    class ifName;\
    VINTERFACE_TEMPLATE_DECLARATIONS(api##_##ifName,api##_API,ifName,level)

/**
 * Generates template instantiations for commonly used templates pertaining to interface declarations. Qualifies the temlate instantiations with 'extern' and 'DECLSPEC_DLL_EXPORT' as appropriate.
 * This macro must be invoked in each interface header file and outside of any namespace.
 * Should only be used for interfaces with nested namespaces. In other cases, VINTERFACE_TEMPLATE_INSTANTIATIONS is provided for convenience.
 */
#define VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API(api,scope,ifName)\
    VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API_AT_LEVEL(api,scope,ifName,1)

#define VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API_AT_LEVEL(api,scope,ifName,level)\
    api##_NAMESPACE_ENTER\
    class ifName;\
    api##_NAMESPACE_EXIT\
    VINTERFACE_TEMPLATE_DECLARATIONS(api##_##ifName,api##_API,scope::ifName,level)

/**
 * Generates exported template instantiations for an interface.
 * This macro must be invoked in each interface implementation file and outside of any namespace.
 */
#define VINTERFACE_TEMPLATE_EXPORTS(ifName)\
    VINTERFACE_TEMPLATE_EXPORTS_AT_LEVEL(ifName,1)

#define VINTERFACE_TEMPLATE_EXPORTS_AT_LEVEL(ifName,level)\
    VINTERFACE_TEMPLATE_DEFINITIONS(ifName,level)

/**
 * Internal use only. Do not invoke.
 */
#define VINTERFACE_TEMPLATE_DECLDEFS(storageClass,declspec,ifNameFull,level)\
    VINTERFACE_TYPEINFOHOLDER_DECLDEFS(storageClass,declspec,ifNameFull,level)\
    VINTERFACE_INTERFACE_DECLDEFS(storageClass,declspec,ifNameFull,level)

/**
 * Internal use only. Do not invoke.
 */
#define VINTERFACE_TYPEINFOHOLDER_DECLDEFS(storageClass,declspec,ifNameFull,level)\
    VINTERFACE_TYPEINFOHOLDER_DECLDEFS_##level(storageClass,declspec,ifNameFull)

#define VINTERFACE_TYPEINFOHOLDER_DECLDEFS_1(storageClass,declspec,ifNameFull)\
    storageClass template class declspec Vca::VTypeInfoHolder<ifNameFull*>;\
    storageClass template class declspec Vca::VTypeInfoHolder<Vca::IDataSource<ifNameFull*>*>;\
    storageClass template class declspec Vca::VTypeInfoHolder<IVReceiver<ifNameFull*>*>;

#define VINTERFACE_TYPEINFOHOLDER_DECLDEFS_2(storageClass,declspec,ifNameFull)\
    VINTERFACE_TYPEINFOHOLDER_DECLDEFS_1(storageClass,declspec,ifNameFull)\
    storageClass template class declspec Vca::VTypeInfoHolder<Vca::IDataSource<Vca::IDataSource<ifNameFull*>*>*>;\
    storageClass template class declspec Vca::VTypeInfoHolder<IVReceiver<Vca::IDataSource<ifNameFull*>*>*>;

/**
 * Internal use only. Do not invoke.
 */
#define VINTERFACE_INTERFACE_DECLDEFS(storageClass,declspec,ifNameFull,level)\
    VINTERFACE_INTERFACE_DECLDEFS_##level(storageClass,declspec,ifNameFull)

#define VINTERFACE_INTERFACE_DECLDEFS_1(storageClass,declspec,ifNameFull)\
    storageClass template class declspec Vca::IDataSource<ifNameFull*>;\
    storageClass template class declspec IVReceiver<ifNameFull*>;

#define VINTERFACE_INTERFACE_DECLDEFS_2(storageClass,declspec,ifNameFull)\
    storageClass template class declspec Vca::IDataSource<ifNameFull*>;\
    storageClass template class declspec IVReceiver<ifNameFull*>;\
    storageClass template class declspec Vca::IDataSource<Vca::IDataSource<ifNameFull*>*>;\
    storageClass template class declspec IVReceiver<Vca::IDataSource<ifNameFull*>*>;


/***************************************************************************
 ***************************************************************************
 *----                                                                 ----*
 *----  PLATFORM DEPENDENT INSTANTIATION DECLARATION/DEFINITION RULES  ----*
 *----                                                                 ----*
 ***************************************************************************
 ***************************************************************************/

/** We don't want extern template instantiations on VMS or Solaris as they're not supported there. */
#if defined(__VMS) || defined(sun)

/**
 * Internal use only. Do not invoke.
 */
#define VINTERFACE_TEMPLATE_DECLARATIONS(storageClass,declspec,ifNameFull,level)

/**
 * Internal use only. Do not invoke.
 */
#define VINTERFACE_INTERFACE_DECLARATIONS(storageClass,declspec,ifNameFull,level)

/**
 * Internal use only. Do not invoke.
 */
#define VINTERFACE_TYPEINFOHOLDER_DECLARATIONS(storageClass,declspec,ifNameFull,level)

/**
 * Internal use only. Do not invoke.
 */
#define VINTERFACE_TEMPLATE_DEFINITIONS(ifName,level)\
    VINTERFACE_TEMPLATE_DECLDEFS(namespace Vca {},DECLSPEC_DLLEXPORT,ifName,level)

#else

/**
 * Internal use only. Do not invoke.
 */
#define VINTERFACE_TEMPLATE_DECLARATIONS(storageClass,declspec,ifNameFull,level)\
    VINTERFACE_TEMPLATE_DECLDEFS(storageClass,declspec,ifNameFull,level)

/**
 * Internal use only. Do not invoke.
 */
#define VINTERFACE_INTERFACE_DECLARATIONS(storageClass,declspec,ifNameFull,level)\
    VINTERFACE_INTERFACE_DECLDEFS(storageClass,declspec,ifNameFull,level)

/**
 * Internal use only. Do not invoke.
 */
#define VINTERFACE_TYPEINFOHOLDER_DECLARATIONS(storageClass,declspec,ifNameFull,level)\
    VINTERFACE_TYPEINFOHOLDER_DECLDEFS(storageClass,declspec,ifNameFull,level)

/**
 * Internal use only. Do not invoke.
 */
#define VINTERFACE_TEMPLATE_DEFINITIONS(ifName,level)

#endif // defined (__VMS)


/*************************
 *****  Definitions  *****
 *************************/

/********************************************
 *----  Vca::VcaSerializerForInterface  ----*
 ********************************************/

namespace Vca {
    class Vca_API VcaSerializerForInterface : public VcaSerializer {
        DECLARE_VIRTUAL_RTTLITE (VcaSerializerForInterface, VcaSerializer);

    //  Sequencer
    public:
        typedef VTrigger<ThisClass> Sequencer;

    //  ObjectSite
    public:
        enum ObjectSite {
            ObjectSite_Unknown,

            //  {ObjectSite_Nil} ()
            ObjectSite_Nil,

            //  {ObjectSite_Receiver, SSID objectSSID} ()
            ObjectSite_Receiver,

            //  {ObjectSite_Sender , SSID objectSSID} (VTypeInfo *pObjectType)
            ObjectSite_Sender,

            //  {ObjectSite_Other, SSID objectSSID} (
            //      VcaSite *pObjectSite, VTypeInfo *pObjectType
            //  )
            ObjectSite_Other
        };

    //  Construction
    protected:
        VcaSerializerForInterface (VcaSerializer *pCaller, bool bWeak);

    //  Destruction
    protected:
        ~VcaSerializerForInterface ();

    //  Access
    protected:
        virtual IVUnknown *interface_() const = 0;

    //  Query
    public:
        bool objectKindIsKnown () const {
            return m_xObjectSite != ObjectSite_Unknown;
        }

    //  Sequencer Actions
    protected:
        void doData (Sequencer *pSequencer);
        void doSite (Sequencer *pSequencer);
        void doType (Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
        void localizeData ();

        void getData ();
        void putData ();

        void wrapupIncomingSerialization ();

    //  Wrapup Utilities
    private:
        virtual void setInterfaceTo_(IVUnknown *pIVUnknown) = 0;

    //  State
    protected:
        bool const              m_bWeak;
        U08                     m_xObjectSite;
        SSID                    m_iObjectSSID;
        VReference<VcaSite>     m_pObjectSite;
        VTypeInfo::Reference    m_pObjectType;
    };
}


/*********************************************
 *----  Vca::VcaSerializerForInterface_  ----*
 *********************************************/

namespace Vca {
    template <class I>
    class VcaSerializerForInterface_ : public VcaSerializerForInterface {
        DECLARE_CONCRETE_RTTLITE (VcaSerializerForInterface_<I>, VcaSerializerForInterface);

    //  Reference Type
    public:
        typedef typename VTypeTraits<I*>::ReferenceType ReferenceType;

    //  Construction
    public:
        VcaSerializerForInterface_(
            VcaSerializer *pCaller, typename I::Reference& rpI, bool bWeak = false
        ) : VcaSerializerForInterface (pCaller, bWeak), m_rpI (rpI._rpReferent_()) {
        }
        VcaSerializerForInterface_(
            VcaSerializer *pCaller, ReferenceType rpI, bool bWeak = false
        ) : VcaSerializerForInterface (pCaller, bWeak), m_rpI (rpI) {
        }

    //  Destruction
    protected:
        ~VcaSerializerForInterface_() {
        }

    //  Access
    protected:
        IVUnknown *interface_() const {
            return m_rpI;
        }

    //  Update
    private:
        inline void setInterfaceTo_(IVUnknown *pIVUnknown);

    //  State
    protected:
        ReferenceType m_rpI;
    };
}


/**********************************************
 *----  Vca::VTypeInfoHolderForInterface  ----*
 **********************************************/

namespace Vca {
    template <class I> class VTypeInfoHolderForInterface : public VTypeInfoHolder<I*> {
        DECLARE_FAMILY_MEMBERS (VTypeInfoHolderForInterface<I>, VTypeInfoHolder<I*>);

    //  Reference Type
    public:
        typedef typename BaseClass::ReferenceType ReferenceType;

    //  Construction
    public:
        VTypeInfoHolderForInterface () {
        }

    //  Type Info Instantiation
    protected:
        VTypeInfo *typeInfoInstantiation_() {
            return I::typeInfoInstantiation ();
        }

    //  Proxy Creation
    public:
        IVUnknown *proxy (VcaOIDX *pOIDX) const {
            return new typename I::Proxy (pOIDX);
        }

    //  Instance Management
    public:
        void construct_(ReferenceType rpI) {
            rpI = 0;
        }
        void preserve_(ReferenceType rpI) {
            if (rpI)
                rpI->retain ();
        }
        void destroy_(ReferenceType rpI) {
            if (rpI)
                rpI->release ();
        }
	bool validates_(I* pI) const {
	    return !pI || pI->isUsable ();
	}

    //  Serializer Creation
    public:
        VcaSerializer *serializer_(VcaSerializer *pCaller, ReferenceType rpI) const {
            return new VcaSerializerForInterface_<I>(pCaller, rpI);
        }
    };
}


/**************************
 *----  Vca::VProxy_  ----*
 **************************/

namespace Vca {
    template <class MDI> class VProxy_ : public MDI {
        DECLARE_CONCRETE_RTTLITE (VProxy_<MDI>, MDI);

//  Construction
    public:
        VProxy_(VcaOIDX *pOIDX) : m_pOIDX (pOIDX) {
        }

//  Destruction
    protected:
        ~VProxy_() {
            m_pOIDX.detachInterface (this);
        }

//  Message Handling
    private:
        virtual bool defersTo (VMessageScheduler &rScheduler) {
            return m_pOIDX->defersTo (rScheduler);
        }

//  Connectivity
private:
    virtual /*override*/ bool isConnected_() const {
	return m_pOIDX.isConnected ();
    }
    virtual /*override*/ bool isUsable_() const {
	return m_pOIDX.isUsable ();
    }

//  OID
    private:
        VcaOIDR *oidr_() const {
            return oidr ();
        }
        VcaOIDX *oidx_() const {
            return oidx ();
        }
    protected:
        VcaOIDR *oidr () const {
            return m_pOIDX;
        }
        VcaOIDX *oidx () const {
            return m_pOIDX;
        }

//  State
    protected:
        VcaOIDX::Vise const m_pOIDX;
    };
}


/***********************
 *----  Role Part  ----*
 ***********************/

/**
 * Base class for all interface roles.
 *
 * @tparam ActorClass the implementor of this role's interface.
 * @tparam MDI The most derived interface.
 */
template <class ActorClass, class MDI = IVUnknown> class IVUnknown_Role : public MDI {
public:
    typedef IVUnknown_Role<ActorClass,MDI> this_t;
//    DECLARE_ABSTRACT_RTTLITE (this_t,MDI);

//  Actor Type
public:
    typedef ActorClass Actor;

//  Interface
public:
    typedef MDI Interface;

//  Construction
protected:
    IVUnknown_Role () {
    }

//  Destruction
protected:
    ~IVUnknown_Role () {
    }

//  Access
public:
    virtual Actor *actor_() const = 0;
    /**
     * Returns the actual implementor of this role's interface.
     */
    Actor *actor () const {
        return actor_();
    }

//  Interface
    VINTERFACE_ROLE_2 (QueryInterface, VTypeInfo*, IVReceiver<IVUnknown*>*);
};


/***********************
 *----  Interface  ----*
 ***********************/

/**
 * Interface from which all other interfaces are derived.
 */
VINTERFACE_TYPEINFOHOLDER_DECLARATIONS(Vca_IVUnknown,Vca_API,IVUnknown,1)
VcaINTERFACE (IVUnknown,VReferenceable);

//  Aliases
public:
    /** @cond VINTERFACE_MEMBERS */
    typedef Vca::ITrigger               ITrigger;

    typedef Vca::VcaOID                 VcaOID;
    typedef Vca::VcaOIDL                VcaOIDL;
    typedef Vca::VcaOIDR                VcaOIDR;
    typedef Vca::VcaOIDX                VcaOIDX;
    typedef Vca::VcaPeer                VcaPeer;
    typedef Vca::VcaSelf                VcaSelf;
    typedef Vca::VcaSite                VcaSite;
    typedef Vca::VcaSSID                VcaSSID;
    typedef Vca::VMessage               VMessage;
    typedef Vca::VMessageManager        VMessageManager;
    typedef Vca::VMessageScheduler      VMessageScheduler;
    typedef Vca::VRolePlayer            VRolePlayer;
    typedef Vca::VSiteInfo		VSiteInfo;
    typedef Vca::VCallbackTicket        VCallbackTicket;
    typedef Vca::VTriggerTicket         VTriggerTicket;
    typedef Vca::count_t                count_t;

    typedef VMessage::Reference RPI;
    typedef VMessageManager MGR;
    /** @endcond */

//  Friends
    friend class Vca::VMessage;
    friend class Vca::VcaOIDL;
    friend class Vca::VcaPeer;
    friend class Vca::VcaSelf;
    friend class Vca::VcaSerializerForInterface;

/*-----------------------------*
 *----  Interface Members  ----*
 *-----------------------------*/

public:
    /**
     * Used to ask one interface for another one -- queries for an interface from the remote end by type.
     * To retrieve a suitable VTypeInfo, you can use the typeInfo() static method on the interface type that you're looking for.
     *
     * @param p1 the VTypeInfo of the interface that you're querying for.
     * @param p2 the receiver by which the interface should be returned if/when found.
     */
    VINTERFACE_METHOD_2 (QueryInterface, VTypeInfo*, IVReceiver<IVUnknown*>*);

/*-----------------------*
 *----  Destruction  ----*
 *-----------------------*/

protected:
    /** @cond VINTERFACE_MEMBERS */
    ~IVUnknown ();

/*----------------------------------*
 *----  Infrastructure Support  ----*
 *----------------------------------*/

//  Temporary Hacks
public:
    /**
     * @warn Do not call this method!!!
     */
    void closePeer ();

//  Export
public:
    count_t exportCount () const {
	return m_iOIDLVise.exportCount ();
    }
    count_t weakExportCount () const {
	return m_iOIDLVise.weakExportCount ();
    }
    bool isExported () const {
        return m_iOIDLVise.isExported ();
    }
    bool isExportedTo (VcaPeer const *pPeer) const {
        return m_iOIDLVise.isExportedTo (pPeer);
    }
    bool isntExported () const {
        return m_iOIDLVise.isntExported ();
    }
    bool isntExportedTo (VcaPeer const *pPeer) const {
        return m_iOIDLVise.isntExportedTo (pPeer);
    }

private:
    /**
     * Mark this local interface as exported to a remote peer.
     *
     * @param pPeer the peer to which this object is being exported.
     * @param bWeak a boolean which, when true, indicates that this export does NOT keep this process alive.
     *        Primarily used for interfaces exchanged internally by Vca (e.g., IPeer) that do the business
     *        of Vca, but also potentially useful for breaking self-reference loops for any object.
     */
    void createExportTo (VcaPeer *pPeer, bool bWeak);

    /**
     * Mark this local interface as loopback exported to self.  Used exclusively as part of the machinery
     * that ensures Vca's IPeer exchange works correctly on byte stream connections that loopback to the
     * same process.
     *
     * @param pPeer the peer to which this object is being exported.
     * @param bWeak a boolean which, when true, indicates that this export does NOT keep this process alive.
     */
    void createExportTo (VcaSelf *pSelf, bool bWeak);

//  Import
public:
    bool weakenImport () const;

//  Message Handling
public:
    /**
     * To be implemented by subclasses in order to appropriately decide if a message should be handled directly/locally or should be deferred via a VMessageScheduler.
     *
     * There are two implementations of this method, one for remote interface implementations (VProxy_::defersTo()) and one for local (VRoleHolder::defersTo()).
     *
     * @param rScheduler the scheduler that may be used for deferral.
     * @return true if messages from this thread to this interface need to be queued via a VMessageScheduler, false if they can be executed directly.
     */
    virtual bool defersTo (VMessageScheduler &rScheduler) = 0;
private:
    void evaluateIncomingFrom (VMessage *pMessage, VcaSite *pSite) const {
        m_iOIDLVise.evaluateIncomingFrom (pMessage, pSite);
    }

//  OID
public:
    VcaOID *oid () const;

//  OIDR/X
private:
    virtual VcaOIDR *oidr_() const {
        return 0;
    }
    virtual VcaOIDX *oidx_() const {
        return 0;
    }
public:
    VcaOIDR *oidr () const {
        return oidr_();
    }
    VcaOIDX *oidx () const {
        return oidx_();
    }

//  OIDL
private:
    void detach (VcaOIDL *pOIDL) {
        m_iOIDLVise.detach (pOIDL);
    }
    void getSSID (VcaSSID &rSSID) {
        m_iOIDLVise.getSSID (rSSID);
    }

//  QI
public:
    void QI (VTypeInfo *pType, IVReceiver<IVUnknown*> *pReceiver);
    /** @endcond */

/// @name Connectivity
//@{
private:
    /**
     * Used to determine if this interface is currently connected to its
     * implementor. The default implementation of this virtual returns true;
     */
    virtual bool isConnected_() const;
    /**
     * Used to determine if messages to this interface can potentially
     * reach their implementor. The default implementation of this virtual
     * returns true.
     */
    virtual bool isUsable_() const;
public:
    /**
     * Used to determine if this interface is currently connected to its
     * implementor. Interfaces backed by a local VRolePlayer are always
     * viewed as connected.  Implemented by delegation to isConnected_().
     *
     * @return true if local or currently connected to a remote object, false otherwise.
     */
    bool isConnected () const {
	return isConnected_();
    }

    /**
     * Negation of isConnected()
     * @see isConnected()
     */
    bool isntConnected () const {
        return !isConnected ();
    }

    /**
     * Used to determine if messages to this interface can potentially
     * reach their implementor. Unlike 'connected' interfaces which
     * require that a path to the implementor be in existence at the
     * time of the 'isConnected' call, interfaces are considered 'usable'
     * if Vca plausibly believes it can establish a connection.  Like
     * 'connected' interfaces, interfaces backed by a local VRolePlayer
     * are always usable.  Implemented by delegation to isUsable_().
     *
     * @return true if local or potentially connectable to a remote object, false otherwise.
     */
    bool isUsable () const {
	return isUsable_();
    }

    /**
     * Negation of isUsable()
     * @see isUsable()
     */
    bool isntUsable () const {
        return !isUsable ();
    }

    /**
     * Provides a mechanism for being notified when this interface becomes
     * unreachable. Utility classes exist for working with this functionality.
     *
     * @see Vca::VInterfaceEKG
     */
    bool requestNoRouteToPeerCallback (
        VTriggerTicket::Reference &rpTicket, ITrigger *pEventSink, bool bSuspended = false
    ) const;


    /**
     * Provides a mechanism for being notified when this interface is exported.
     *
     */
    bool requestExportCountCallbacks (
        VCallbackTicket::Reference &rpTicket, IVReceiver<count_t>* pSink
    ) const {
	return m_iOIDLVise.requestExportCountCallbacks (rpTicket, pSink, oidr ());
    }

    /**
     * Provides a mechanism for being notified when this interface is exported.
     *
     */
    template <class sink_t> bool requestExportCountCallbacks (
        VCallbackTicket::Reference &rpTicket, sink_t *pSink, void (sink_t::*pSinkMember)(count_t)
    ) const {
	return m_iOIDLVise.requestExportCountCallbacks (rpTicket, pSink, pSinkMember, oidr ());
    }
//@}

//  RolePlayer
private:
    /**
     * Returns this interface's VRolePlayer.
     * Will only return a VRolePlayer instance when a local VRolePlayer
     * instance is available to be returned. In many cases the VRolePlayer for
     * this interface is remote and thus a null pointer is returned.
     *
     * @return a pointer to this interface's VRolePlayer if local, null
     * otherwise.
     */
    virtual VRolePlayer *rolePlayer_() const {
        return 0;
    }

public:
    /** @cond VINTERFACE_MEMBERS */
    VRolePlayer *rolePlayer() const {
        return rolePlayer_();
    }

//  SiteInfo
private:
    VcaSite* site () const;
public:
    VSiteInfo* siteInfo () const;

//  TypeInfo
public:
    VTypeInfo *typeInfo_() const {
        return typeInfoHolder_().typeInfo_();
    }

//  Display
public:
    using BaseClass::displayInfo;

    void displayInfo () const;

    void displayExportTable () const;

//  State
private:
    VcaOIDL::Vise m_iOIDLVise;
    /** @endcond */
};


/*****************************************
 *****  Deferred Inline Definitions  *****
 *****************************************/

namespace Vca {
    template<class I> void VcaSerializerForInterface_<I>::setInterfaceTo_(IVUnknown *pIVUnknown) {
        if (pIVUnknown)
            pIVUnknown->retain ();
        if (m_rpI)
            m_rpI->release ();
        m_rpI = static_cast<I*>(pIVUnknown);
    }
}


/********************************************
 *****  IClient/IVReceiver Definitions  *****
 ********************************************
 *----------------------------------------------------------------------------*
 *  The goal here is an include of "IVReceiver.h", but to do that, we need to
 *  ensure that Vca::IClient, the superclass of IVReceiver, is defined before
 *  IVReceiver.  While we could naively include "IVReceiver.h" here, doing so
 *  causes simple includes of "Vca_IClient.h" to fail.  To prevent that, we
 *  include 'Vca_IClient.h' here and rely on "Vca_IClient.h" to include
 *  "IVReceiver.h" after its own definitions.
 *----------------------------------------------------------------------------*/

#include "Vca_IClient_NRK.h"
#include "Vca_IDataSource_NRK.h"

VINTERFACE_INTERFACE_DECLARATIONS(Vca_IVUnknown,Vca_API,IVUnknown,1)

#ifndef _WIN32
VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IClient)
#endif

#endif
