#ifndef Vdd_Object_Interface
#define Vdd_Object_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VAddin.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VBenderenceable.h"

#include "rtype.h"

#include "M_KnownObjectTable.h"

class rtPTOKEN_Handle;

namespace V {
    class VSimpleFile;
}


/*************************
 *****  Definitions  *****
 *************************/

namespace Vdd {

/*******************
 *----  Object ----*
 *******************/

    class Object : public V::VAddin_<VBenderenceable> {
	DECLARE_FAMILY_MEMBERS (Object, V::VAddin_<VBenderenceable>);

    //  Aliases
    public:
	typedef VBenderenceable Referenceable;
	typedef VReference<ThisClass> Reference;

    //  Visitor
    public:
	class Visitor {
	public:
	    virtual void visitHandle (VContainerHandle *pHandle) = 0;
	};

    //  Construction
    protected:
	Object (Referenceable *pReferenceable) : BaseClass (pReferenceable) {
	}

    //  Destruction
    protected:
	~Object () {
	}

    //  Attention Mask
    private:
	virtual unsigned int attentionMask_() const = 0;
	virtual void setAttentionMaskTo_(unsigned int iValue) = 0;
    public:
	unsigned int attentionMask () const {
	    return attentionMask_();
	}
	void setAttentionMaskTo (unsigned int iValue) {
	    setAttentionMaskTo_(iValue);
	}

    //  Database Access
    private:
	virtual M_AND *database_() const = 0;
	virtual M_KOT *kot_() const = 0;
	virtual M_ASD *scratchPad_() const = 0;
	virtual M_ASD *objectSpace_() const = 0;
	virtual RTYPE_Type rtype_() const = 0;
    public:
	M_AND *database () const {
	    return database_();
	}
	M_KOT *kot () const {
	    return kot_();
	}
	M_ASD *objectSpace () const {
	    return objectSpace_();
	}
	RTYPE_Type rtype () const {
	    return rtype_();
	}
	char const *rtypeName () const {
	    return RTYPE_TypeIdAsString (rtype ());
	}
	M_ASD *scratchPad () const {
	    return scratchPad_();
	}

    //  Database Container Access
    private:
	virtual void getContainerHandle_(VReference<VContainerHandle>&rpResult) = 0;
    public:
	void getContainerHandle (VReference<VContainerHandle>&rpResult) {
	    getContainerHandle_(rpResult);
	}

    //  Database Container Management
    private:
	virtual bool isAForwardingTarget_() const = 0;
    public:
	bool isAForwardingTarget () const {
	    return isAForwardingTarget_();
	}

    //  Transient Extension Access
    private:
	virtual transientx_t *transientExtension_() const {
	    return 0;
	}
	virtual transientx_t *transientExtensionIfA_(VRunTimeType const &rRTT) const {
	    return 0;
	}
	virtual bool transientExtensionIsA_(VRunTimeType const &rRTT) const {
	    return false;
	}
    public:
	transientx_t *transientExtension () const {
	    return transientExtension_();
	}
	transientx_t *transientExtensionIfA (VRunTimeType const &rRTT) const {
	    return transientExtensionIfA_(rRTT);
	}
	bool transientExtensionIsA (VRunTimeType const &rRTT) const {
	    return transientExtensionIsA_(rRTT);
	}

    //  Cluster Access
    private:
	virtual rtPTOKEN_Handle *getPToken_() const = 0;
    public:
	rtPTOKEN_Handle *getPToken () const {
	    return getPToken_();
	}

    //  Cluster Identity
    public:
	bool Names (Object const *pThat) const {
	    return this == pThat;
	}
	bool DoesntName (Object const *pThat) const {
	    return !Names (pThat);
	}

    //  Cluster Maintenance
    protected:
	virtual bool align_() {
	    return false;
	}
	virtual bool alignAll_(bool bCleaning) {
	    return false;
	}
    public:
	bool align () {
	    return align_();
	}
	bool alignAll (bool bCleaning = true) {
	    return alignAll_(bCleaning);
	}

    //  Reference Reporting and Visitor Support
    public:
	virtual void generateReferenceReport (V::VSimpleFile &rOutputFile, unsigned int xLevel) const = 0;
	virtual void visitReferencesUsing (Visitor *visitor) = 0;

    //  Visitor Support
    public:
	virtual void visitUsing (Visitor *visitor) = 0;
    };
}


#endif
