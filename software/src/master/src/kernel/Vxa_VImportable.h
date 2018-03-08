#ifndef Vxa_VImportable_Interface
#define Vxa_VImportable_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VType.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VPack.h"
#include "Vxa_VScalar.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCallType1Importer;
    class VCallType2Importer;
    class VResultBuilder;
    class VTask;

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

    template <typename T> class VImportable : virtual public VImportableType {
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


/**********************************************************************
 *----  template <typename T> class VImportable<VPack<T> const&>  ----*
 **********************************************************************/

    template <typename T> class VImportable<VPack<T> const&> {
    //  Aliases
    public:
        typedef VPack<T> pack_t;
        typedef typename pack_t::value_t pack_value_t;
        typedef typename pack_t::return_t pack_return_t;

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
	    operator pack_value_t () const {
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
	    pack_return_t m_pValue;
	};

    //  PackImplementation
    public:
        class PackImplementation;

    //  Construction
    protected:
	VImportable () {
	}

    //  Destruction
    protected:
	~VImportable () {
	}

    //  Use
    private:
	template <class CallImporter> static bool RetrieveImpl (
	    pack_return_t &rResult, VTask *pTask, CallImporter &rImporter
	) {
        //  GOTTA GET A TYPE??? MAYBE NOT!!!
	    rResult.setTo (
                new VScalarInstance<pack_value_t,PackImplementation> (
                    static_cast<VImportableType*>(0), pTask, rImporter
                )
	    );
	    return true;
	}

    public:
	static bool Retrieve (pack_return_t &rResult, VTask *pTask, VCallType1Importer &rImporter) {
	    return RetrieveImpl (rResult, pTask, rImporter);
	}
	static bool Retrieve (pack_return_t &rResult, VTask *pTask, VCallType2Importer &rImporter) {
	    return RetrieveImpl (rResult, pTask, rImporter);
	}
    };



/******************************************************************************************
 *----  template <typename T> class VImportable<VPack<T> const&>::PackImplementation  ----*
 ******************************************************************************************/

    template <typename T> class VImportable<VPack<T> const&>::PackImplementation : public VPack<T> {
    //  Aliases
    public:
        typedef typename VImportable<T>::Instance element_instance_t;

    //  Construction
    public:
        template <typename CallImporter> PackImplementation (
            VTask *pTask, CallImporter &rImporter
        ) : m_iElementArray (rImporter.getParameterCountRemaining (pTask)) {
            for (cardinality_t xParameter = 0; xParameter < parameterCount (); xParameter++)
                m_iElementArray[xParameter].retrieve (pTask, rImporter);
        }

    //  Destruction
    public:
        ~PackImplementation () {
        }

    //  Access
    public:
        cardinality_t parameterCount () const OVERRIDE {
            return m_iElementArray.elementCount ();
        }
        T parameterValue (cardinality_t xParameter) const OVERRIDE {
            return m_iElementArray[xParameter];
        }

    //  State
    private:
        VkDynamicArrayOf<element_instance_t> m_iElementArray;
    };
}

/*************************************
 *****  Template Instantiations  *****
 *************************************/

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(Vxa_VImportable_Implementation)

#ifndef Vxa_VImportable_Implementation
#define Vxa_VImportable_Implementation extern
#endif

Vxa_VImportable_Implementation template class Vxa_API Vxa::VImportable<Vxa::VAny const&>;
Vxa_VImportable_Implementation template class Vxa_API Vxa::VImportable<bool>;
Vxa_VImportable_Implementation template class Vxa_API Vxa::VImportable<short>;
Vxa_VImportable_Implementation template class Vxa_API Vxa::VImportable<unsigned short>;
Vxa_VImportable_Implementation template class Vxa_API Vxa::VImportable<int>;
Vxa_VImportable_Implementation template class Vxa_API Vxa::VImportable<unsigned int>;
Vxa_VImportable_Implementation template class Vxa_API Vxa::VImportable<float>;
Vxa_VImportable_Implementation template class Vxa_API Vxa::VImportable<double>;

Vxa_VImportable_Implementation template class Vxa_API Vxa::VImportable<char const*>;
Vxa_VImportable_Implementation template class Vxa_API Vxa::VImportable<VString const&>;

Vxa_VImportable_Implementation template class Vxa_API Vxa::VImportable<Vxa::VResultBuilder&>;

#endif

#endif
