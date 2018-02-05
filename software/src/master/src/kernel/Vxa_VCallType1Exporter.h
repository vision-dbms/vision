#ifndef Vxa_VCallType1Exporter_Interface
#define Vxa_VCallType1Exporter_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VResultBuilder.h"
#include "Vxa_VCallType1.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VError;
    class Vxa_API VCallType1Exporter : public VResultBuilder, public VCallType1 {
	DECLARE_FAMILY_MEMBERS (VCallType1Exporter, VResultBuilder);

    //  Aliases
    public:
	typedef ThisClass exporter_t;

    /*****************************
     *----  class ResultSet  ----*
     *****************************/

    public:
	class Vxa_API ResultSet : public VReferenceable {
	    DECLARE_ABSTRACT_RTTLITE (ResultSet, VReferenceable);

	//  Construction
	protected:
	    ResultSet ();

	//  Destruction
	protected:
	    ~ResultSet ();

	//  Update
	/**
	 * When setting a new type of result, it is important to add a pure virtual function here,
	 * adding a corresponding method in the subclass only will cause bugs that are hard to find.
	 */
	public:
	    virtual bool setResultTo (exporter_t *pExporter, bool iValue) = 0;
	    virtual bool setResultTo (exporter_t *pExporter, int iValue) = 0;
	    virtual bool setResultTo (exporter_t *pExporter, float iValue) = 0; 
	    virtual bool setResultTo (exporter_t *pExporter, double iValue) = 0;
	    virtual bool setResultTo (exporter_t *pExporter, VString const &rValue) = 0;
	    virtual bool setResultTo (exporter_t *pExporter, ISingleton *pValue) = 0;

	//  Transmission
	public:
	    virtual bool transmitUsing (VCallType1Exporter *pExporter) const = 0;
	};

    /*****************************************************
     *----  template class ResultSetOf<Val_T,Var_T>  ----*
     *****************************************************/

    public:
	template <
	    typename Val_T, typename Var_T = typename Vca::VTypePattern<Val_T>::var_t
	> class ResultSetOf : public ResultSet {
	    typedef ResultSetOf<Val_T,Var_T> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, ResultSet);

	//  Aliases
	public:
	    typedef Val_T val_t;
	    typedef Var_T var_t;
	    typedef VkDynamicArrayOf<var_t> storage_t;

	//  Construction
	public:
	    template <typename old_storage_t> ResultSetOf (
		cardinality_t cResult, cardinality_t xCopyBound, old_storage_t const &rOldStorage, val_t iValue
	    ) : m_iStorage (cResult) {
		for (unsigned int xElement = 0; xElement < xCopyBound; xElement++)
		    m_iStorage[xElement] = rOldStorage[xElement];
		m_iStorage[xCopyBound] = iValue;
	    }
	    ResultSetOf (cardinality_t const cResult, val_t iValue) : m_iStorage (cResult) {
		m_iStorage[0] = iValue;
	    }

	//  Destruction
	protected:
	    ~ResultSetOf () {
	    }

	//  Access
	public:
	    val_t element (unsigned int xElement) const {
		return m_iStorage[xElement];
	    }

	//  Update
	public:
	    virtual bool setResultTo (exporter_t *pExporter, bool iValue) OVERRIDE {
		return pExporter->storeResult (m_iStorage, iValue);
	    }
	    virtual bool setResultTo (exporter_t *pExporter, int iValue) OVERRIDE {
		return pExporter->storeResult (m_iStorage, iValue);
	    }
	    virtual bool setResultTo (exporter_t *pExporter, float iValue) OVERRIDE {
		return pExporter->storeResult (m_iStorage, iValue);
	    }
	    virtual bool setResultTo (exporter_t *pExporter, double iValue) OVERRIDE {
		return pExporter->storeResult (m_iStorage, iValue);
	    }
	    virtual bool setResultTo (exporter_t *pExporter, VString const &rValue) OVERRIDE {
		return pExporter->storeResult (m_iStorage, rValue);
	    }
	    virtual bool setResultTo (exporter_t *pExporter, ISingleton *pValue) OVERRIDE {
		return pExporter->storeResult (m_iStorage, pValue);
	    }

	//  Transmission
	public:
	    virtual bool transmitUsing (VCallType1Exporter *pExporter) const OVERRIDE {
		return pExporter->transmit (m_iStorage);
	    }

	//  State
	private:
	    storage_t m_iStorage;
	};
        typedef ResultSetOf<bool>			bool_result_set_t;
        typedef ResultSetOf<int>			int_result_set_t;
        typedef ResultSetOf<float>			float_result_set_t;
        typedef ResultSetOf<double>			double_result_set_t;
        typedef ResultSetOf<VString const&,VString>	string_result_set_t;
        typedef ResultSetOf<ISingleton*,ISingleton::Reference> object_result_set_t;

    //  Construction
    public:
	VCallType1Exporter (VTask *pTask, VCallType1Importer const &rCallImporter);

    //  Destruction
    public:
	~VCallType1Exporter ();

    //  Control
    public:
	void enable () {
	    m_bDisabled = false;
	}
	bool enabled () const {
	    return !disabled ();
	}

	void disable () {
	    m_bDisabled = true;
	}
	bool disabled () const {
	    return m_bDisabled;
	}

    //  Storage Management
    private:
	template <typename storage_t, typename value_t> bool storeValue (storage_t &rStorage, value_t iValue) {
	    rStorage[cursorPosition()] = iValue;
	    return true;
	}
	template <typename storage_t, typename value_t> bool promoteAndStoreValue (
	    storage_t &rStorage, value_t iValue
	) {
	    m_pResultSet.setTo (new ResultSetOf<value_t> (cardinality (), cursorPosition (), rStorage, iValue));
	    return true;
	}
    public:
	// unsupported storage:
	template <typename storage_t, typename value_t> bool storeResult (
	    storage_t &rStorage, value_t const &rValue
	) {
	    return raiseResultTypeException(typeid (typename storage_t::EType), typeid (value_t));
	}

	// bool storage:
	bool storeResult (bool_result_set_t::storage_t &rStorage, bool iValue);
	bool storeResult (bool_result_set_t::storage_t &rStorage, int iValue);
	bool storeResult (bool_result_set_t::storage_t &rStorage, float iValue);
	bool storeResult (bool_result_set_t::storage_t &rStorage, double iValue);

	// int storage:
	bool storeResult (int_result_set_t::storage_t &rStorage, bool iValue);
	bool storeResult (int_result_set_t::storage_t &rStorage, int iValue);
	bool storeResult (int_result_set_t::storage_t &rStorage, float iValue);
	bool storeResult (int_result_set_t::storage_t &rStorage, double iValue);

	// float storage:
	bool storeResult (float_result_set_t::storage_t &rStorage, bool iValue);
	bool storeResult (float_result_set_t::storage_t &rStorage, int iValue);
	bool storeResult (float_result_set_t::storage_t &rStorage, float iValue);
	bool storeResult (float_result_set_t::storage_t &rStorage, double iValue);

	// double storage:
	bool storeResult (double_result_set_t::storage_t &rStorage, bool iValue);
	bool storeResult (double_result_set_t::storage_t &rStorage, int iValue);
	bool storeResult (double_result_set_t::storage_t &rStorage, float iValue);
	bool storeResult (double_result_set_t::storage_t &rStorage, double iValue);

	// string storage:
	bool storeResult (string_result_set_t::storage_t &rStorage, VString const &rValue);

	// object storage:
	bool storeResult (object_result_set_t::storage_t &rStorage, ISingleton *pValue);

    //  Transmission
    public:
	template <typename result_t> bool transmit (result_t const &rResult) {
	    return isAlive () && returnVector (rResult);
	}

    //  Implementation
    private:
	template <typename value_t> bool makeResultSetOf (value_t iValue) {
	    m_pResultSet.setTo (new ResultSetOf<value_t> (cardinality (), iValue));
	    return true;
	}
	bool makeResultSetOf (VString const &rValue);
	bool makeResultSetOf (ISingleton *pValue);

    public:
	void logError (VError *pError) const;
	void logError (void *pOther) const {
	}

	template <typename object_reference_t> bool returnObject (VClass *pClass, object_reference_t const &rpObject) {
	    logError (rpObject);
	    return returnResult (pClass, Export (rpObject));
	}

	virtual bool returnResult (VExportableDatum const &rDatum);

	template <typename value_t> bool returnResult (VExportableType *pType, value_t iValue) {
	    return m_pResultSet ? m_pResultSet->setResultTo (this, iValue) : makeResultSetOf (iValue);
	}

	//  conversions:
	bool returnResult (VExportableType *pType, short iValue);
	bool returnResult (VExportableType *pType, unsigned int iValue);
	bool returnResult (VExportableType *pType, unsigned short iValue);
	bool returnResult (VExportableType *pType, char const *pValue);
	bool returnResult (VExportableType *pType, export_return_t const &rpValue);

    //  State
    private:
	ResultSet::Reference m_pResultSet;
	VTask *m_pTask;
	bool m_bDisabled;
    };
}

#endif
