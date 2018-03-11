/*****  Vxa_VImportable Implementation  *****/
#define Vxa_VImportable_Implementation

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

#include "Vxa_VImportable.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"

#include "Vxa_VCallType1Importer.h"
#include "Vxa_VCallType2Importer.h"

#include "Vxa_VResultBuilder.h"
#include "Vxa_VTask.h"


/******************************
 ******************************
 *****                    *****
 *****  Vxa::VImportable  *****
 *****                    *****
 ******************************
 ******************************/

namespace Vxa {
    template <
	typename Val_T, typename Var_T = typename Vca::VTypePattern<Val_T>::var_t
    > class Vxa_API VStockImportable : public VImportable<Val_T> {
    public:
	typedef VStockImportable<Val_T,Var_T> this_t;
	DECLARE_FAMILY_MEMBERS (this_t, VImportable<Val_T>);

    //  Aliases
    public:
	typedef typename BaseClass::scalar_return_t scalar_return_t;

    //  Construction
    public:
	VStockImportable () {
	}

    //  Destruction
    public:
	~VStockImportable () {
	}

    //  Instance Retrieval
    private:
	virtual bool retrieve (scalar_return_t &rResult, VTask *pTask, VCallType1Importer &rImporter) OVERRIDE {
	    return rImporter.getParameter (pTask, this, rResult);
	}
	virtual bool retrieve (scalar_return_t &rResult, VTask *pTask, VCallType2Importer &rImporter) OVERRIDE {
	    return rImporter.getParameter (pTask, this, rResult);
	}
    };
}


/************************************************
 ************************************************
 *****                                      *****
 *****  Vxa::VImportable Run Time Metadata  *****
 *****                                      *****
 ************************************************
 ************************************************/

namespace {
    Vxa::VStockImportable<Vxa::VAny const&>		g_iImportTraits_VAny;
    Vxa::VStockImportable<bool>				g_iImportTraits_bool;
    Vxa::VStockImportable<short>			g_iImportTraits_short;
    Vxa::VStockImportable<unsigned short>		g_iImportTraits_unsigned_short;
    Vxa::VStockImportable<int>				g_iImportTraits_int;
    Vxa::VStockImportable<unsigned int>			g_iImportTraits_unsigned_int;
    Vxa::VStockImportable<float>			g_iImportTraits_float;
    Vxa::VStockImportable<double>			g_iImportTraits_double;

    Vxa::VStockImportable<char const*,VString>		g_iImportTraits_char_const_p;
    Vxa::VStockImportable<VString const&,VString>	g_iImportTraits_VString;
}
