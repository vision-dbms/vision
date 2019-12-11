/*****  Vxa_VResultBuilder Implementation  *****/

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

#include "Vxa_VResultBuilder.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCallType1Exporter.h"
#include "Vxa_VCallType2Exporter.h"

#include "Vxa_VCallType1Importer.h"
#include "Vxa_VCallType2Importer.h"

#include "Vxa_VImportable.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vxa::VResultBuilder  *****
 *****                       *****
 *********************************
 *********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VResultBuilder::VResultBuilder (VTask *pTask) : m_pTask (pTask) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VResultBuilder::~VResultBuilder () {
}

/****************************
 ****************************
 *****  Remote Control  *****
 ****************************
 ****************************/

Vxa::VTask::RemoteControl *Vxa::VResultBuilder::getRemoteControl () const {
    return m_pTask->getRemoteControl (getRemoteControlInterface ());
}


/************************
 ************************
 *****              *****
 *****  Importable  *****
 *****              *****
 ************************
 ************************/

namespace {
    using namespace Vxa;
    class Importable : public VImportable<VResultBuilder&> {
	DECLARE_FAMILY_MEMBERS (Importable, VImportable<VResultBuilder&>);

    //  Construction
    public:
	Importable () {
	}

    //  Destruction
    public:
	~Importable () {
	}

    //  Retrieval
    private:
	template <class ImporterType> bool retrieveImpl (
	    scalar_return_t &rResult, VTask *pTask, ImporterType &rImporter
	) {
	    rResult.setTo (
		new VScalarInstance<VResultBuilder&,typename ImporterType::Exporter> (this, pTask, rImporter)
	    );
	    return true;
	}
	virtual bool retrieve (
            scalar_return_t &rResult, VTask *pTask, VCallType1Importer &rImporter
        ) OVERRIDE {
	    return retrieveImpl (rResult, pTask, rImporter);
	}
	virtual bool retrieve (
            scalar_return_t &rResult, VTask *pTask, VCallType2Importer &rImporter
        ) OVERRIDE {
	    return retrieveImpl (rResult, pTask, rImporter);
	}
    } g_iResultBuilderImportable;
}
