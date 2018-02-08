/*****  Vxa_VExportable Implementation  *****/
#define Vxa_VExportable_Implementation

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

#include "Vxa_VExportable.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"

#include "Vxa_VCallType1Exporter.h"
#include "Vxa_VCallType2Exporter.h"

#include "Vxa_VConstant.h"

#include "Vxa_VExportableDatum.h"

#include "Vxa_VInfiniteSetOf.h"
#include "Vxa_VResultBuilder.h"


/******************************
 ******************************
 *****                    *****
 *****  Vxa::VExportable  *****
 *****                    *****
 ******************************
 ******************************/

namespace Vxa {
    template <
	typename Val_T, typename Var_T = typename Vca::VTypePattern<Val_T>::var_t
    > class Vxa_API VStockExportable : public VExportable<Val_T> {
	typedef VStockExportable<Val_T,Var_T> this_t;
	DECLARE_FAMILY_MEMBERS (this_t, VExportable<Val_T>);

    //  Aliases
    public:
	typedef ThisClass exportable_t;
	typedef VInfiniteSetOf<ThisClass> universe_t;
	typedef VMonotypeMapMakerFor<Val_T,Var_T> map_maker_t;

    //  Datum
    public:
	class Datum : public VExportableDatum_<exportable_t> {
	    DECLARE_FAMILY_MEMBERS (Datum,VExportableDatum_<exportable_t>);

	//  Construction
	public:
	    Datum (exportable_t *pType, Val_T iValue) : BaseClass (pType), m_iValue (iValue) {
	    }

	//  Destruction
	public:
	    ~Datum () {
	    }

	//  Access
	public:
	    using BaseClass::type;
	    VSet *universe () const {
		return type()->universe ();
	    }

	//  Use
	public:
	    virtual bool returnResultUsing (VCallType1Exporter *pExporter) const OVERRIDE {
		return pExporter->returnResult (type (), m_iValue);
	    }
	    virtual bool returnResultUsing (VCallType2Exporter *pExporter) const OVERRIDE {
		typename map_maker_t::Reference pMapMaker;
		return pExporter->returnResult (pMapMaker, type (), universe (), static_cast<Val_T>(m_iValue));
	    }

	//  State
	private:
	    Var_T const m_iValue;
	};

    //  Construction
    public:
	VStockExportable () {
	}

    //  Destruction
    public:
	~VStockExportable () {
	}

    //  Access
    public:
	universe_t *universe () const {
	/**************************************************************************************************
	 *  Every stock exportable has a unique universe set that is a 'const' property of the exportable.
	 *  Normally, the stock exportable's constructor would be the natural place to create that set.
	 *  That cannot be done.  Stock exportables are anonymous-namespace scoped globals.  That means
	 *  they are constructed when the Vxa library is loaded.  Consequently, their constructors cannot
	 *  rely directly or indirectly on the prior construction of any other global data objects in the
	 *  library.  Sets, on the other hand, are role players.  They require a cohort and cohort
	 *  construction requires that all globals in a toolkit be properly constructed.  Satisfy that
	 *  requirement by creating the universe set on first access.
	 **************************************************************************************************/
	    if (m_pUniverse.isNil ())
		m_pUniverse.setTo (new universe_t (const_cast<ThisClass*>(this)));
	    return m_pUniverse;
	}

    //  Result Generation
    private:
	virtual bool createMethod (method_return_t &rResult, Val_T const &rValue) OVERRIDE {
	    rResult.setTo (new VConstant<Val_T,Var_T> (rValue));
	    return true;
	}
	virtual bool createExport (export_return_t &rResult, Val_T const &rValue) OVERRIDE {
	    return false;
	}
	virtual bool returnResult (VResultBuilder *pResultBuilder, Val_T const &rValue) OVERRIDE {
	    Datum const iDatum (this, rValue);
	    return pResultBuilder->returnResult (iDatum);
	}

    //  State
    private:
	typename universe_t::Reference mutable m_pUniverse;
    };
}


/************************************************
 ************************************************
 *****                                      *****
 *****  Vxa::VExportable Run Time Metadata  *****
 *****                                      *****
 ************************************************
 ************************************************/

namespace {
    Vxa::VStockExportable<bool>				g_iExportable_bool;
    Vxa::VStockExportable<short>			g_iExportable_short;
    Vxa::VStockExportable<unsigned short>		g_iExportable_unsigned_short;
    Vxa::VStockExportable<int>				g_iExportable_int;
    Vxa::VStockExportable<unsigned int>			g_iExportable_unsigned_int;
    Vxa::VStockExportable<float>			g_iExportable_float;
    Vxa::VStockExportable<double>			g_iExportable_double;

    Vxa::VStockExportable<char const*,VString>		g_iExportable_char_const_p;
    Vxa::VStockExportable<VString>			g_iExportable_VString;
}
