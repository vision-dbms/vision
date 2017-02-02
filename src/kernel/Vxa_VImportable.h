#ifndef Vxa_VImportable_Interface
#define Vxa_VImportable_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VType.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VScalar.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCallType1Importer;
    class VCallType2Importer;
    class VTask;

    Vxa_API    void InitializeStockImportables ();

/***********************************
 *----  class VImportableType  ----*
 ***********************************/

    class Vxa_API VImportableType : virtual public VType {
	DECLARE_FAMILY_MEMBERS (VImportableType, VType);

    //  Construction
    protected:
	VImportableType () {
	}

    //  Destruction
    protected:
	~VImportableType () {
	}
    };

/*****************************************************
 *----  template <typename T> class VImportable  ----*
 *****************************************************/

    template <typename T> class Vxa_API VImportable : virtual public VImportableType {
	DECLARE_FAMILY_MEMBERS (VImportable<T>, VImportableType);

    //  Aliases
    public:
	typedef typename VScalar<T>::Reference scalar_return_t;

    //  Instance
    public:
	class Instance {
	//  Construction
	public:
	    Instance () {
	    }

	//  Destruction
	public:
	    ~Instance () {
	    }

	//  Access
	public:
	    operator T () const {
		return m_pValue->value ();
	    }

	//  Update
	public:
	    template <typename CallImporter> bool retrieve (VTask *pTask, CallImporter &rImporter) {
		return Retrieve (m_pValue, pTask, rImporter);
	    }
	    void clear () {
		m_pValue.clear ();
	    }
	    
	//  State
	private:
	    scalar_return_t m_pValue;
	};

    //  Construction
    protected:
	VImportable () {
	    if (g_pTraits == 0)
		g_pTraits = this;
	}

    //  Destruction
    protected:
	~VImportable () {
	    if (g_pTraits == this)
		g_pTraits = 0;
	}

    //  Use
    private:
	virtual bool retrieve (scalar_return_t &rResult, VTask *pTask, VCallType1Importer &rImporter) = 0;
	virtual bool retrieve (scalar_return_t &rResult, VTask *pTask, VCallType2Importer &rImporter) = 0;
    public:
	static bool Retrieve (scalar_return_t &rResult, VTask *pTask, VCallType1Importer &rImporter) {
	    return g_pTraits && g_pTraits->retrieve (rResult, pTask, rImporter);
	}
	static bool Retrieve (scalar_return_t &rResult, VTask *pTask, VCallType2Importer &rImporter) {
	    return g_pTraits && g_pTraits->retrieve (rResult, pTask, rImporter);
	}

    //  State
    private:
	static ThisClass *g_pTraits;
    };

    template <typename T> VImportable<T>* VImportable<T>::g_pTraits = 0;
}


#endif
