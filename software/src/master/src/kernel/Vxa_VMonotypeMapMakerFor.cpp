/*****  Vxa_VMonotypeMapMakerFor Implementation  *****/

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



/****************************
 ****************************
 *****  Instantiations  *****
 ****************************
 ****************************/

template class Vxa_API Vxa::VMonotypeMapMakerFor<bool>;

template class Vxa_API Vxa::VMonotypeMapMakerFor<short>;
template class Vxa_API Vxa::VMonotypeMapMakerFor<int>;

template class Vxa_API Vxa::VMonotypeMapMakerFor<unsigned short>;
template class Vxa_API Vxa::VMonotypeMapMakerFor<unsigned int>;

template class Vxa_API Vxa::VMonotypeMapMakerFor<float>;
template class Vxa_API Vxa::VMonotypeMapMakerFor<double>;

template class Vxa_API Vxa::VMonotypeMapMakerFor<char const*,VString>;
template class Vxa_API Vxa::VMonotypeMapMakerFor<VString>;
