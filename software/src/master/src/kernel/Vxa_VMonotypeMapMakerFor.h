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
	bool transmitValues_(VCallType2Exporter *pExporter, VCollection *pCluster, object_reference_array_t const &rInjection) OVERRIDE;
	bool transmitValues_(VCallType2Exporter *pExporter, VCollection *pCluster) OVERRIDE;

	bool transmitValues_(VCallType2Exporter *pExporter, object_reference_array_t const &rInjection) OVERRIDE;
	bool transmitValues_(VCallType2Exporter *pExporter) OVERRIDE;
	
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
 *************************************
 *
 *  Recursive dependencies exist between Vxa::VMonotypeMapMakerFor<T> and templated
 *  member functions in Vxa::VCallType2Exporter.  While other compilers are willing
 *  to accept explicit instantiations of Vxa::VMonotypeMapMaker<T> placed in this
 *  header file, MacOS' clang/llvm compiler is not.  That is presumably because all
 *  definitions needed to instantiate VMonotypeMapMakerFor<T> have not yet been seen
 *  by the compiler.  The workaround is to wait until those definitions have been made
 *  before attempting the instantiations (read, instantiate the templates as the last
 *  thing Vxa_VMonotypeMapMakerFor.cpp does, not here).
 *
 *  There is no corresponding problem declaring these instantiations as 'extern', so
 *  when that's the goal (i.e., not their 'implementation/definition'), continue to
 *  include them here.
 *
 *****/

#ifndef Vxa_VMonotypeMapMakerFor_Implementation
#include "Vxa_VMonotypeMapMakerFor_Instantiations.h"
#endif

#endif // #ifndef Vxa_VMonotypeMapMakerFor_Interface
