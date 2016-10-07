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

#if defined(__linux__)
#include <cxxabi.h>
#endif

/******************
 *****  Self  *****
 ******************/

#include "V_VRTTI.h"

/************************
 *****  Supporting  *****
 ************************/


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
#ifdef __linux
, m_pDemangledName (0)
#endif
{
}

V::VRTTI::VRTTI (VRTTI const &rOther) : m_rTypeInfo (rOther.m_rTypeInfo)
#ifdef __linux
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
#if defined (__linux__)
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
#if defined (__linux__)
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
