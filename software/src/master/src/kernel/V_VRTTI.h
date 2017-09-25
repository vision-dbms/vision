#ifndef V_VRTTI_Interface
#define V_VRTTI_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

/*---------------------------------------------------------------------*
 *  This class exists to demangle std::type_info::name() on Linux (and
 *  other) platforms where demangled names are not returned by default.
 *---------------------------------------------------------------------*/

#if defined(__linux__) || defined(__APPLE__)
#define USING_CXXABI_DEMANGLE
#endif

namespace V {
    class V_API VRTTI : public VTransient {
	DECLARE_FAMILY_MEMBERS (VRTTI, VTransient);

    //  Construction
    public:
	VRTTI (std::type_info const &rTypeInfo);
	VRTTI (VRTTI const &rOther);

    //  Destruction
    public:
	~VRTTI ();

    //  Access
    public:
	char const *name () const;

    //  State
    private:
	std::type_info const&	m_rTypeInfo;
#ifdef USING_CXXABI_DEMANGLE
	mutable char const*	m_pDemangledName;
#endif
    };
}


#endif
