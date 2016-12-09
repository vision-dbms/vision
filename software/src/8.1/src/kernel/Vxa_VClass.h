#ifndef Vxa_VClass_Interface
#define Vxa_VClass_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "VStaticTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VExportable.h"
#include "Vxa_VMethod.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VClass : virtual public VStaticTransient, virtual public VExportableType {
	DECLARE_FAMILY_MEMBERS (VClass, VStaticTransient);

    //  Aliases
    public:
	typedef VkMapOf<VString,VString const&,char const*,VMethod::Reference> dictionary_t;

    //  Construction
    protected:
	VClass ();

    //  Destruction
    protected:
	~VClass ();

    //  Initialization
    protected:
	void initialize () {
	}

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
    private:
	template <typename T> bool defineConstantImpl (VString const &rName, T const &rConstant) {
	    m_iHelpInfo << rName << "\n";
	    VMethod::Reference pMethod;
	    return VExportable<T>::CreateMethod (pMethod, rName, rConstant) && defineMethod (pMethod);
	}
    protected:
#ifndef sun
	template <typename T> bool defineConstant (VString const &rName, T rConstant[]) {
	    return defineConstantImpl (rName, static_cast<T*>(rConstant));
	}
#endif
	template <typename T> bool defineConstant (VString const &rName, T const &rConstant) {
	    return defineConstantImpl (rName, rConstant);
	}

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
    protected:
	VString m_iHelpInfo;
    };
} //  namespace Vxa


#endif
