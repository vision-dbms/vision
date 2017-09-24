#ifndef Vxa_VMonotypeMapMakerFor_Interface
#define Vxa_VMonotypeMapMakerFor_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VMonotypeMapMaker.h"

#include "VkDynamicArrayOf.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTypePattern.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    template <
	typename Val_T, typename Var_T = typename Vca::VTypePattern<Val_T>::var_t
    > class VMonotypeMapMakerFor : public VMonotypeMapMaker {
	typedef VMonotypeMapMakerFor<Val_T,Var_T> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t, VMonotypeMapMaker);

    //  Aliases
    public:
	typedef VkDynamicArrayOf<Var_T> container_t;
	typedef Val_T val_t;
	typedef Var_T var_t;

	typedef val_t value_t;

    //  Construction
    public:
	VMonotypeMapMakerFor (VCardinalityHints *pTailHints, VSet *pCodomain, Val_T iValue);

    //  Destruction
    protected:
	~VMonotypeMapMakerFor ();

    //  Access
    protected:
	container_t const &trimmedContainer () {
	    trimContainer ();
	    return m_iContainer;
	}

    //  Transmission
    public:
	bool transmitValues_(VCallType2Exporter *pExporter, VCollectableCollection *pCluster, object_reference_array_t const &rInjection);
	bool transmitValues_(VCallType2Exporter *pExporter, VCollectableCollection *pCluster);

	bool transmitValues_(VCallType2Exporter *pExporter, object_reference_array_t const &rInjection);
	bool transmitValues_(VCallType2Exporter *pExporter);
	
    //  Update
    public:
	void append (Val_T iValue);
	void setCurrentValueTo (Val_T iValue);
	void trimContainer ();

    //  State
    private:
	container_t m_iContainer;
    };
}


/*************************************
*****  Template Instantiations  *****
*************************************/

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(Vxa_VMonotypeMapMakerFor_Implementation)

#ifndef Vxa_VMonotypeMapMakerFor_Implementation
#define Vxa_VMonotypeMapMakerFor_Implementation extern
#endif

Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<bool>;

Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<short>;
Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<int>;

Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<unsigned short>;
Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<unsigned int>;

Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<float>;
Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<double>;

Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<char const*, VString>;
Vxa_VMonotypeMapMakerFor_Implementation template class Vxa_API Vxa::VMonotypeMapMakerFor<VString>;

#endif


#endif
