#ifndef Vxa_VCollectable_Interface
#define Vxa_VCollectable_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VClass.h"

#include "Vxa_VExportable.h"
#include "Vxa_VImportable.h"
#include "Vxa_VResultBuilder.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VRTTI.h"

#include "Vxa_VConstant.h"

#include "Vxa_VCallType1Exporter.h"
#include "Vxa_VCallType2Exporter.h"

/*************************
 *****  Definitions  *****
 *************************/

#define DEFINE_VXA_COLLECTABLE(T)\
    template class Vxa::VExportable<T*>;\
    template class Vxa::VImportable<T*>

#define DEFINE_VXA_TEMPLATED_COLLECTABLE(T)\
    DEFINE_VXA_COLLECTABLE(T);\
    template class T

namespace Vxa {
    template <typename T> class VCollectable
	: public VClass
	, public VExportable<T*>
	, public VImportable<T*>
    {
	DECLARE_FAMILY_MEMBERS (VCollectable<T>,VClass);

    //  Aliases
    public:
        typedef typename VCollectableTraits<T>::cluster_t collection_t;
	typedef typename VCollectableTraits<T>::val_t val_t;
	typedef typename VCollectableTraits<T>::var_t var_t;

	typedef VCollectable<T> collectable_t;
	typedef VExportable<T*> exportable_t;
	typedef VImportable<T*> importable_t;

	typedef typename importable_t::scalar_return_t scalar_return_t;

    //  Class Builder
    private:
	class CBuilder : public T::ClassBuilder {
	public:
	    typedef typename T::ClassBuilder base_t;
	public:
	    CBuilder (VClass *pClass) : base_t (pClass) {
		base_t::defineConstant ("rttiName", collectable_t::RTTIName ());
		base_t::defineHelp (collectable_t::RTTIName ());
	    }
	};

    //  Datum
    public:
	class Datum : public VExportableDatum_<VClass> {
	    DECLARE_FAMILY_MEMBERS (Datum,VExportableDatum_<VClass>);

	//  Construction
	public:
	    Datum (VClass *pClass, T *pObject) : BaseClass (pClass), m_pObject (pObject) {
	    }

	//  Destruction
	public:
	    ~Datum () {
	    }

	//  Use
	public:
	    virtual bool returnResultUsing (VCallType1Exporter *pExporter) const {
		return pExporter->returnObject (type (), m_pObject);
	    }
	    virtual bool returnResultUsing (VCallType2Exporter *pExporter) const {
		return pExporter->returnObject (type (), m_pObject);
	    }

	//  State
	private:
	    typename T::Reference const m_pObject;
	};

    //  Construction
    public:
	VCollectable () {
	}

    //  Destruction
    public:
	~VCollectable () {
	}

    //  RTTI Info
    public:
	static V::VRTTI RTTI () {
	    static V::VRTTI iRTTI (typeid (T));
	    return iRTTI;
	}
	static VString RTTIName () {
	    static VString iRTTIName (RTTI ().name ());
	    return iRTTIName;
	}

    //  Class Materialization
    public:
	VClass *thisAsClass () {
	    static CBuilder iBuilder (this);
	    return this;
	}

    //  Export Creation
    private:
	virtual bool createExport (export_return_t &rpResult, val_t const &rpInstance) {
	    (new collection_t (thisAsClass (), 0, rpInstance))->getRole (rpResult);
	    return rpResult.isntNil ();
	}

    //  Method Creation
    private:
	virtual bool createMethod (
	    method_return_t &rpResult, VString const &rName, val_t const &rpInstance
	) {
	    rpResult.setTo (new VConstant<val_t,var_t> (rName, rpInstance));
	    return rpResult.isntNil ();
	}

    //  Parameter Acquistion
    private:
	template <class ImporterType> bool retrieveImpl (scalar_return_t &rResult, VTask *pTask, ImporterType &rImporter) {
	    return rImporter.raiseUnimplementedOperationException (pTask, typeid(*this), "retrieveImpl");
	}
	virtual bool retrieve (scalar_return_t &rResult, VTask *pTask, VCallType1Importer &rImporter) {
	    return retrieveImpl (rResult, pTask, rImporter);
	}
	virtual bool retrieve (scalar_return_t &rResult, VTask *pTask, VCallType2Importer &rImporter) {
	    return retrieveImpl (rResult, pTask, rImporter);
	}

    //  Result Generation
    private:
	virtual bool returnResult (VResultBuilder *pResultBuilder, val_t const &rpResult) {
	    Datum const iDatum (thisAsClass (), rpResult);
	    return pResultBuilder->returnResult (iDatum);
	}
    };
}


#endif
