#ifndef V_VArgList_Interface
#define V_VArgList_Interface

/************************
 *****  Components  *****
 ************************/

#include <stdarg.h>

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VFamilyValues.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VArgList {
	DECLARE_FAMILY_MEMBERS (VArgList, void);

    //  Construction
    public:
	VArgList (va_list iArgList);
	VArgList (ThisClass const &rOther);
	VArgList ();

    //  Destruction
    public:
	~VArgList ();

    //  Access
    private:
	template <typename T> class BadType {
	};
	template <typename T> T goodArgType () {
	    return va_arg (m_pArgList, T);
	}
	template <typename T> T badArgType () {
	    return BadType<T>::UnSupported;
	}
    public:
	template <typename T> T arg () {
	    return goodArgType<T> ();
	}

	va_list& list () {
	    return m_pArgList;
	}
	operator va_list& () {
	    return list ();
	}

    //  State
    private:
	va_list m_pArgList;
    };
}

/*******************************
 *****  Definition Helper  *****
 *******************************/

#define V_VARGLIST(varname,parameter) V::VArgList varname; va_start (varname.list(),parameter)


#endif
