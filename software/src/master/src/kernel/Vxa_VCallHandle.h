#ifndef Vxa_VCallHandle_Interface
#define Vxa_VCallHandle_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCallData.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VPack.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCollection;
    class VMethod;

    class Vxa_API VCallHandle : public VCallData {
	DECLARE_FAMILY_MEMBERS (VCallHandle, VCallData);

    //  Construction
    protected:
	VCallHandle (VString const &rMethodName, cardinality_t cParameters, cardinality_t cTask);
	VCallHandle (ThisClass const &rOther);

    //  Destruction
    protected:
	~VCallHandle ();

    //  Access
    private:
	virtual IVUnknown *caller () const = 0;

    //  Reporting
    private:
	void report (char const *pFormat, ...) const;
    protected:
	bool makingReport () const;

	void reportInvocation () const;
	void reportParameterRequest (unsigned int xParameter) const;
	void reportParameterReceipt (unsigned int xParameter) const;
	void reportCompletion () const;

    //  Use
    public:
	bool raiseTypeException (
	    std::type_info const &rOriginatorType, std::type_info const &rResultType, char const *pWhich
	) const;
	bool raiseParameterTypeException (
	    std::type_info const &rOriginatorType, std::type_info const &rResultType
	) const;
	bool raiseResultTypeException (
	    std::type_info const &rOriginatorType, std::type_info const &rResultType
	) const;

	bool raiseUnimplementedOperationException (
	    std::type_info const &rOriginatorType, char const *pWhere
	) const;
    public:
	virtual bool invokeMethod (VMethod *pMethod, VCollection *pCluster) const = 0;

	virtual bool returnError (VString const &rMessage) const = 0;
	virtual bool returnSNF () const = 0;
	virtual bool returnNA () const;
    };
}


#endif
