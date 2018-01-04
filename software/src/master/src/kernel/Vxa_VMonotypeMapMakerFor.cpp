/*****  Vxa_VMonotypeMapMakerFor Implementation  *****/
#define Vxa_VMonotypeMapMakerFor_Implementation

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vxa_VMonotypeMapMakerFor.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCallType2Exporter.h"


/****************************************************
 ****************************************************
 *****                                          *****
 *****  Vxa::VMonotypeMapMakerFor<Val_T,Var_T>  *****
 *****                                          *****
 ****************************************************
 ****************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

template <typename Val_T, typename Var_T> Vxa::VMonotypeMapMakerFor<Val_T,Var_T>::VMonotypeMapMakerFor (
    VCardinalityHints *pTailHints, VSet *pCodomain, Val_T iValue
) : BaseClass (pTailHints, pCodomain), m_iContainer (pTailHints ? pTailHints->span () : 0) {
    setCurrentValueTo (iValue);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

template <typename Val_T, typename Var_T> Vxa::VMonotypeMapMakerFor<Val_T,Var_T>::~VMonotypeMapMakerFor () {
}

/**************************
 **************************
 *****  Transmission  *****
 **************************
 **************************/

template <typename Val_T, typename Var_T> bool Vxa::VMonotypeMapMakerFor<Val_T,Var_T>::transmitValues_(
    VCallType2Exporter *pExporter, VCollectableCollection *pCluster, object_reference_array_t const &rInjection
) {
    return pExporter->returnSegment (rInjection, pCluster, trimmedContainer ());
}

template <typename Val_T, typename Var_T> bool Vxa::VMonotypeMapMakerFor<Val_T,Var_T>::transmitValues_(
    VCallType2Exporter *pExporter, object_reference_array_t const &rInjection
) {
    return pExporter->returnSegment (rInjection, trimmedContainer ());
}

template <typename Val_T, typename Var_T> bool Vxa::VMonotypeMapMakerFor<Val_T,Var_T>::transmitValues_(
    VCallType2Exporter *pExporter, VCollectableCollection *pCluster
) {
    return pExporter->returnObjects (pCluster, trimmedContainer ());
}

template <typename Val_T, typename Var_T> bool Vxa::VMonotypeMapMakerFor<Val_T,Var_T>::transmitValues_(
    VCallType2Exporter *pExporter
) {
    return pExporter->returnVector (trimmedContainer ());
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

template <typename Val_T, typename Var_T> void Vxa::VMonotypeMapMakerFor<Val_T,Var_T>::append (Val_T iValue) {
    setCurrentValueTo (iValue);
    commitThrough (updateLimit ());
}

template <typename Val_T, typename Var_T> void Vxa::VMonotypeMapMakerFor<Val_T,Var_T>::setCurrentValueTo (Val_T iValue) {
    noteUpdate ();
    cardinality_t const xValue = commitLimit ();
    if (xValue >= m_iContainer.cardinality ())
	m_iContainer.Append (xValue - m_iContainer.cardinality () + 1);
    m_iContainer[xValue] = iValue;
    show (iValue);
}

template <typename Val_T, typename Var_T> void Vxa::VMonotypeMapMakerFor<Val_T,Var_T>::trimContainer () {
    if (m_iContainer.cardinality () > commitLimit ()) {
	m_iContainer.Delete (commitLimit (), m_iContainer.cardinality () - commitLimit ());
    }
}


/*************************************
 *************************************
 *****  Template Instantiations  *****
 *************************************
 *************************************
 *
 *  Recursive dependencies exist between Vxa::VMonotypeMapMakerFor<T> and templated
 *  member functions in Vxa::VCallType2Exporter.  While other compilers are willing
 *  to accept explicit instantiations of Vxa::VMonotypeMapMaker<T> placed in the
 *  "Vxa_VMonotypeMapMakerFor.h" header file included at this beginning of this file,
 *  MacOS' clang/llvm compiler is not.  That is presumably because all definitions
 *  needed to instantiate VMonotypeMapMakerFor<T> have not yet been seen by the
 *  compiler.  The workaround is to wait until those definitions have been made
 *  before attempting the instantiations (read, instantiate the templates at this
 *  point in the file, not in a file included near its beginning).
 *
 *****/
#include "Vxa_VMonotypeMapMakerFor_Instantiations.h"
