#ifndef Vca_VTypePattern_Interface
#define Vca_VTypePattern_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/**************************
 *****  Declarations  *****
 **************************/

class VString;

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {

/*******************************************************
 *----  template <typename T> struct VTypePattern  ----*
 *******************************************************/

    template <typename T> struct VTypePattern {
	typedef T val_t;
	typedef T var_t;
    };

/*****************************************************************
 *----  template <typename T> struct VTypePattern<T const&>  ----*
 *****************************************************************/

    template <typename T> struct VTypePattern<T const&> {
	typedef T const& val_t;
	typedef T var_t;
    };

/***********************************************************
 *----  template <typename T> struct VTypePattern<T*>  ----*
 ***********************************************************/

    template <typename T> struct VTypePattern<T*> {
	typedef T val_t;
	typedef typename T::Reference var_t;
    };

/**********************************************************
 *----  template <> struct VTypePattern<char const*>  ----*
 **********************************************************/

    template <> struct VTypePattern<char const*> {
	typedef char const* val_t;
	typedef VString var_t;
    };

}


#endif
