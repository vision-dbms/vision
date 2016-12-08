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

#include "Vxa_VImportable.h"



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

    public:
	static ThisClass *ThisRBImportable () {
	    static ThisClass *pRBI = new ThisClass ();
	    return pRBI;
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
	virtual bool retrieve (scalar_return_t &rResult, VTask *pTask, VCallType1Importer &rImporter) {
	    return retrieveImpl (rResult, pTask, rImporter);
	}
	virtual bool retrieve (scalar_return_t &rResult, VTask *pTask, VCallType2Importer &rImporter) {
	    return retrieveImpl (rResult, pTask, rImporter);
	}
    };
}

namespace Vxa {
    void InitializeRBImportables () {
	Importable::ThisRBImportable ();	    
    }
}

