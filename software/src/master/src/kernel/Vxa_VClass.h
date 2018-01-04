#ifndef Vxa_VClass_Interface
#define Vxa_VClass_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VExportable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VMethod.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VClass : virtual public VExportableType {
	DECLARE_FAMILY_MEMBERS (VClass, VExportableType);

    //  Aliases
    public:
	typedef VkMapOf<VString,VString const&,char const*,VMethod::Reference> dictionary_t;

    //  Construction
    protected:
	VClass ();

    //  Destruction
    protected:
	~VClass ();

    //  Accounting and Labeling
    public:
	void onCollectionCreation (unsigned int cInstances) {
	    m_cCollectionsCreated++;
	    m_cInstancesReported += cInstances;
	}
	void onCollectionDeletion (unsigned int cInstances) {
	    m_cCollectionsDeleted++;
	    m_cInstancesReleased += cInstances;
	}
	void setIdentificationTo (VString const &rID) {
	    m_iIdentification.setTo (rID);
	}

    //  Method Definition
    public:
	bool defineMethod (VMethod *pMethod);

    //  Method Execution
    public:
	bool invokeMethod (
	    VString const &rName, VCallHandle const &rCallHandle, VCollection *pCollection
	) const;

    //  State
    private:
	dictionary_t m_iDictionary;
	unsigned __int64 m_cCollectionsCreated;
	unsigned __int64 m_cCollectionsDeleted;
	unsigned __int64 m_cInstancesReported;
	unsigned __int64 m_cInstancesReleased;
	VString m_iIdentification;
    };
} //  namespace Vxa


#endif
