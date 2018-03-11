#ifndef Vxa_VCallType2Importer_Interface
#define Vxa_VCallType2Importer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCallType2.h"

//#include "Vxa_VCollectableCollectionOf.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VTaskCursor.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    /**************************************
     *----  class VCallType2Importer  ----*
     **************************************/

    class Vxa_API VCallType2Importer : public VCallType2 {
	DECLARE_FAMILY_MEMBERS (VCallType2Importer, VCallType2);

	friend class VCallAgent;

    //  Construction
    public:
	VCallType2Importer (VCallType2 const &rCallHandle);
	VCallType2Importer (ThisClass const &rOther);

    //  Destruction
    public:
	~VCallType2Importer ();

    //  Parameter Acquisition
    private:
	VCallAgent *agent (VTask *pTask) const;
    public:
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
