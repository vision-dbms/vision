#ifndef Vxa_VCollectable_Interface
#define Vxa_VCollectable_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VClass.h"

#include "Vxa_VExportableDatum.h"

#include "Vxa_VExportable.h"
#include "Vxa_VImportable.h"
#include "Vxa_VResultBuilder.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VRTTI.h"

#include "Vxa_VCollectableMethod.h"

#include "Vxa_VConstant.h"

#include "Vxa_VCallType1Exporter.h"
#include "Vxa_VCallType2Exporter.h"

/*************************
 *****  Definitions  *****
 *************************/

#define DEFINE_VXA_COLLECTABLE(T)\
    template class Vxa_API Vxa::VExportable<T*>;\
    template class Vxa_API Vxa::VImportable<T*>

#define DEFINE_VXA_TEMPLATED_COLLECTABLE(T)\
    DEFINE_VXA_COLLECTABLE(T);\
    template class Vxa_API T

namespace Vxa {
    template <typename T> class VCollectable
	: public VClass
	, public VExportable<T*>
	, public VImportable<T*>
    {
	DECLARE_FAMILY_MEMBERS (VCollectable<T>,VClass);

    //  Aliases
    public:
	typedef VCollectableMethodBase<T> method_t;
	typedef typename method_t::collection_t collection_t;
	typedef typename method_t::val_t val_t;
	typedef typename method_t::var_t var_t;

	typedef VCollectable<T> collectable_t;
	typedef VExportable<T*> exportable_t;
	typedef VImportable<T*> importable_t;

	typedef typename importable_t::scalar_return_t scalar_return_t;

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
    protected:
	VCollectable () {
	    V::VRTTI iRTTI (typeid(*this)); {
		VString iClassID (iRTTI.name ());
		BaseClass::setIdentificationTo (iClassID);
	    }
	    defineConstant ("rttiName", iRTTI.name ());
	}

    //  Destruction
    protected:
	~VCollectable () {
	}

    //  Export Creation
    private:
	virtual bool createExport (export_return_t &rpResult, val_t const &rpInstance) {
	    (new collection_t (this, 0, rpInstance))->getRole (rpResult);
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

    //  Method Definition
    public:
	template <typename Signature> bool defineMethod (VString const &rName, Signature pMember) {
	    typename VCollectableMethod<Signature>::Reference pMethod (
		new VCollectableMethod<Signature> (rName, pMember)
	    );
	    return defineMethod (pMethod);
	}
	using BaseClass::defineMethod;

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
	    Datum const iDatum (this, rpResult);
	    return pResultBuilder->returnResult (iDatum);
	}
    };
}


#endif
