/*****  V_VRTTI Implementation  *****/

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

#include "V_VRTTI.h"

/************************
 *****  Supporting  *****
 ************************/

#ifdef USING_CXXABI_DEMANGLE
#include <cxxabi.h>
#endif


/*********************
 *********************
 *****           *****
 *****  V_VRTTI  *****
 *****           *****
 *********************
 *********************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VRTTI::VRTTI (std::type_info const &rTypeInfo) : m_rTypeInfo (rTypeInfo)
#ifdef USING_CXXABI_DEMANGLE
, m_pDemangledName (0)
#endif
{
}

V::VRTTI::VRTTI (VRTTI const &rOther) : m_rTypeInfo (rOther.m_rTypeInfo)
#ifdef USING_CXXABI_DEMANGLE
, m_pDemangledName (0)
#endif
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VRTTI::~VRTTI () {
#ifdef USING_CXXABI_DEMANGLE
    if (m_pDemangledName)
	::free (const_cast<char*>(m_pDemangledName));
#endif
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

char const *V::VRTTI::name () const {
#ifdef USING_CXXABI_DEMANGLE
    if (!m_pDemangledName) {
	int status;
	m_pDemangledName = abi::__cxa_demangle(m_rTypeInfo.name (), 0, 0, &status);
    }
    return m_pDemangledName;
// #elif defined (__VMS) && defined (__ia64) && !defined(_DEBUG)
//     return m_rTypeInfo.__demangled_name ();
#else
    return m_rTypeInfo.name ();
#endif
}
