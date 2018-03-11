#ifndef Vxa_VCallType1Importer_Interface
#define Vxa_VCallType1Importer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCallType1.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VCallAgent.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    template <class Handle_T> class VCallAgentFor;
    class VCallAgent;

    class VCallType1Exporter;
    class VCallType1Importer;

    class VImportableType;

    class VTask;

    /**************************************
     *----  class VCallType1Importer  ----*
     **************************************/

    class Vxa_API VCallType1Importer : public VCallType1 {
	DECLARE_FAMILY_MEMBERS (VCallType1Importer, VCallType1);

	friend class VCallAgent;

    //  Construction
    public:
	VCallType1Importer (VCallType1 const &rCallHandle);
	VCallType1Importer (ThisClass const &rOther);

    //  Destruction
    public:
	~VCallType1Importer ();

    //  Parameter Acquisition
    private:
	VCallAgent *agent (VTask *pTask) const;
    public:
	template <typename scalar_return_t> bool getObjectParameter (
	    VTask *pTask, VImportableType *pType, scalar_return_t &rResult
	) {
            return agent (pTask)->getObjectParameter (pType, rResult);
        }
	template <typename scalar_return_t> bool getParameter (
	    VTask *pTask, VImportableType *pType, scalar_return_t &rResult
	) {
	    return raiseParameterTypeException (pTask, typeid(*this), typeid (scalar_return_t));
	}
        bool getParameter (VTask *pTask, VImportableType *pType, any_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, bool_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, short_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, unsigned_short_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, int_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, unsigned_int_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, float_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, double_scalar_return_t &rResult);
	bool getParameter (VTask *pTask, VImportableType *pType, VString_scalar_return_t &rResult);

        cardinality_t getParameterIndex (VTask *pTask) const;
        cardinality_t getParameterCountRemaining (VTask *pTask) const;

    //  Exception Generation
    protected:
	bool raiseParameterTypeException (
	    VTask *pTask, std::type_info const &rOriginatorType, std::type_info const &rUnexpectedType
	) const;
    public:
	bool raiseUnimplementedOperationException (
	    VTask *pTask, std::type_info const &rOriginator, char const *pWhere
	) const;

    //  State
    private:
	VReference<VCallAgent> mutable m_pAgent;
    };
}


#endif
