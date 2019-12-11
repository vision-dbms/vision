#ifndef Vxa_VExportable_Interface
#define Vxa_VExportable_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VType.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VCollection.h"
#include "Vxa_VMethod.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCardinalityHints;
    class VMapMaker;
    class VResultBuilder;

    typedef ICollection::Reference export_return_t;

/***********************************
 *----  class VExportableType  ----*
 ***********************************/

    class Vxa_API VExportableType : virtual public VType {
	DECLARE_FAMILY_MEMBERS (VExportableType, VType);

    //  Construction
    protected:
	VExportableType (){
	};

    //  Destruction
    protected:
	~VExportableType (){
	};
    };

/*****************************************************
 *----  template <typename T> class VExportable  ----*
 *****************************************************/

    template <typename T> class VExportable : virtual public VExportableType {
	DECLARE_FAMILY_MEMBERS (VExportable<T>, VExportableType);

    //  Construction
    protected:
	VExportable () {
	    if (g_pTraits == 0)
		g_pTraits = this;
	}

    //  Destruction
    protected:
	~VExportable () {
	    if (g_pTraits == this)
		g_pTraits = 0;
	}

    //  Result Generation
    public:
	static bool CreateMethod (method_return_t &rResult, T const &rInstance) {
	    return g_pTraits && g_pTraits->createMethod (rResult, rInstance);
	}
	static bool CreateExport (export_return_t &rResult, T const &rInstance) {
	    return g_pTraits && g_pTraits->createExport (rResult, rInstance);
	}

/*******************************************************************************
 *>>>>  ResultReturn: Step 2
 *
 *  This stop in the result return trail is deceptively simple but very potent.
 *  The VExportable virtualized trait class converts the ReturnResult static
 *  trait call into a call to the virtual member of the singleton instance that
 *  implements result canonicalization.  In the initial implementation of Vxa,
 *  there are two standard implementations of this trait class - one for POD
 *  and POD-like types like VString (see Vxa_VExportable.cpp (or .cxx)) and one
 *  for objects (see Vxa_VCollectable.h).
 *
 *  In a illustration of the power of singleton virtualized trait classes like
 *  Vxa::VExportable, another implementation of this class can be found in the
 *  'voa_toolkit' VMS toolkit.  That implementation canonicalizes instances of
 *  the Vendor String class and is particularly interesting in that it 
 *  represents one approach for canonicalizing a data type NOT understood by 
 *  Vision.
 *
 *  Last Stop: Vxa_VResultBuilder.h
 *  Next Stop: Vxa_VResultBuilder.h
 *
 *******************************************************************************/
	static bool ReturnResult (VResultBuilder *pResultBuilder, T const &rInstance) {
	    return g_pTraits && g_pTraits->returnResult (pResultBuilder, rInstance);
	}
    private:
	virtual bool createMethod (method_return_t &rResult, T const &rInstance) = 0;
	virtual bool createExport (export_return_t &rResult, T const &rInstance) = 0;
	virtual bool returnResult (VResultBuilder *pResultBuilder, T const &rInstance) = 0;

    //  State
    private:
	static ThisClass *g_pTraits;
    };

    template <typename T> VExportable<T>* VExportable<T>::g_pTraits = 0;

/*********************************************************************
 *----  template <typename T> class VExportable<VReference<T> >  ----*
 *********************************************************************/

    template <typename T> class VExportable<VReference<T> > : public VExportable <T*> {
	DECLARE_FAMILY_MEMBERS (VExportable<VReference<T> >, VExportable<T*>);

    //  Construction
    protected:
	VExportable () {
	}

    //  Destruction
    protected:
	~VExportable () {
	}
    };

/***************************************************************
 *----  template <typename T> class VExportable<T const&>  ----*
 ***************************************************************/

    template <typename T> class VExportable<T const&> : public VExportable <T> {
	DECLARE_FAMILY_MEMBERS (VExportable<T const&>, VExportable<T>);

    //  Construction
    protected:
	VExportable () {
	}

    //  Destruction
    protected:
	~VExportable () {
	}
    };

/*****************************************************************
 *----  template <typename T> export_return_t Export (T iT)  ----*
 *****************************************************************/

    template <typename T> inline export_return_t Export (T iT) {
	export_return_t pExport;
	VExportable<T>::CreateExport (pExport, iT);
	return pExport;
    }
}


/*************************************
 *****  Template Instantiations  *****
 *************************************/

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(Vxa_VExportable_Implementation)

#ifndef Vxa_VExportable_Implementation
#define Vxa_VExportable_Implementation extern
#endif

Vxa_VExportable_Implementation template class Vxa_API Vxa::VExportable<bool>;
Vxa_VExportable_Implementation template class Vxa_API Vxa::VExportable<short>;
Vxa_VExportable_Implementation template class Vxa_API Vxa::VExportable<unsigned short>;
Vxa_VExportable_Implementation template class Vxa_API Vxa::VExportable<int>;
Vxa_VExportable_Implementation template class Vxa_API Vxa::VExportable<unsigned int>;
Vxa_VExportable_Implementation template class Vxa_API Vxa::VExportable<float>;
Vxa_VExportable_Implementation template class Vxa_API Vxa::VExportable<double>;

Vxa_VExportable_Implementation template class Vxa_API Vxa::VExportable<char const*>;
Vxa_VExportable_Implementation template class Vxa_API Vxa::VExportable<V::VString>;

#endif

#endif
