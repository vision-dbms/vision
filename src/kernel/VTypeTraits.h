#ifndef VTypeTraits_Interface
#define VTypeTraits_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

class VString;

/*************************
 *****  Definitions  *****
 *************************/

/*****************************************************
 *----  template <typename T> class VTypeTraits  ----*
 *****************************************************/

template <typename T> class VTypeTraits {
//  Reference Type
public:
    typedef T &ReferenceType;

//  Storage Type
public:
    typedef T StorageType;
};

/***************************************************************
 *----  template <typename T> class VTypeTraits<T const&>  ----*
 ***************************************************************/

template <typename T> class VTypeTraits<T const&> : public VTypeTraits<T> {
//  Reference Type
public:
    typedef T const &ReferenceType;
};

/*********************************************************
 *----  template <> class VTypeTraists<char const*>  ----*
 *********************************************************/

template <> class VTypeTraits<char const*> : public VTypeTraits<VString> {
//  Reference Type
public:
    typedef char const *&ReferenceType;
};


#endif
