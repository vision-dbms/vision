#ifndef Vxa_VExportableDatum_Interface
#define Vxa_VExportableDatum_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VClass.h"
#include "Vxa_VExportable.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCallType1Exporter;
    class VCallType2Exporter;

    //****************************************************************
    //  VExportableDatum
    class Vxa_API VExportableDatum {
	DECLARE_NUCLEAR_FAMILY (VExportableDatum);

    //  Construction
    protected:
	VExportableDatum ();

    //  Destruction
    protected:
	~VExportableDatum ();

    //  Access
    private:
	virtual VExportableType *type_() const = 0;
    public:
	VExportableType *type () const {
	    return type_();
	}

    //  Use
    public:
	virtual bool returnResultUsing (VCallType1Exporter *pExporter) const = 0;
	virtual bool returnResultUsing (VCallType2Exporter *pExporter) const = 0;

    //  State
    private:
	VExportableType::Pointer const m_pType;
    };

    //****************************************************************
    //  template <class Type_T> VExportableDatum_<Type_T>
    template <class Type_T> class VExportableDatum_ : public VExportableDatum {
	DECLARE_FAMILY_MEMBERS (VExportableDatum_<Type_T>, VExportableDatum);

    //  Construction
    protected:
	VExportableDatum_(Type_T *pType) : m_pType (pType) {
	}

    //  Destruction
    protected:
	~VExportableDatum_() {
	}

    //  Access
    private:
	virtual VExportableType *type_() const {
	    return type ();
	}
    public:
	Type_T *type () const {
	    return m_pType;
	}

    //  State
    private:
	typename Type_T::Pointer const m_pType;
    };
}


#endif
