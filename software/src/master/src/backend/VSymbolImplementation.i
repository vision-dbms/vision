#ifndef VSymbolImplementation_Implementation
#define VSymbolImplementation_Implementation

/**********************************
 **********************************
 *****  Task Implementations  *****
 **********************************
 **********************************/

/**********************************************
 *****  Conditional Instance Path Access  *****
 **********************************************/

template <class T, class R> void Vsi_cipath_c<T,R>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    M_CPD* pReordering;
    rtLINK_CType* pRestriction;
    R iDatum;
    if (pT && (pT->*m_pSource)(pReordering, pRestriction, iDatum)) {
	pTask->loadDucWith (iDatum);
	pTask->restrictAndReorderDuc (pRestriction, pReordering);
	pTask->restrictDuc (DSC_Descriptor_Pointer (rCurrent));
    }
    else
	pTask->loadDucWithNA ();
}


/***************************************************
 *****  Conditional Instance Reference Access  *****
 ***************************************************/

template <class T, class R> void Vsi_ciref<T,R>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    R iDatum;
    if (pT && (pT->*m_pSource)(iDatum)) {
	pTask->loadDucWith (iDatum);
	pTask->restrictDuc (DSC_Descriptor_Pointer (rCurrent));
    }
    else
	pTask->loadDucWithNA ();
}

template <class T, class R> void Vsi_ciref_c<T,R>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    R iDatum;
    if (pT && (pT->*m_pSource)(iDatum)) {
	pTask->loadDucWith (iDatum);
	pTask->restrictDuc (DSC_Descriptor_Pointer (rCurrent));
    }
    else
	pTask->loadDucWithNA ();
}


/*****************************************************
 *****  Unconditional Instance Reference Access  *****
 *****************************************************/

template <class T, class R> void Vsi_uiref<T,R>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT) {
	pTask->loadDucWith ((pT->*m_pSource)());
	pTask->restrictDuc (DSC_Descriptor_Pointer (rCurrent));
    }
    else
	pTask->loadDucWithNA ();
}

template <class T, class R> void Vsi_uiref_c<T,R>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT) {
	pTask->loadDucWith ((pT->*m_pSource)());
	pTask->restrictDuc (DSC_Descriptor_Pointer (rCurrent));
    }
    else
	pTask->loadDucWithNA ();
}


/**************************
 *****  Duc Feathers  *****
 **************************/

template <class T> void Vsi_setDucFeathers<T>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);

    if (pT) {
	pTask->loadDucWithNextParameter ();
	pTask->loadDucWithBoolean (
	    pT->setDucFeathers (DSC_Descriptor_Pointer (rCurrent), pTask->duc ())
	);
    }
    else
	pTask->loadDucWithNA ();
}


/***********************
 *****  Functions  *****
 ***********************/

template <class T, class R> void Vsi_f0<T,R>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT)
	(pTask->*this->m_pSink) ((pT->*m_pSource)());
    else
	pTask->loadDucWithNA ();
}

template <class T, class R> void Vsi_f0_c<T,R>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT)
	(pTask->*this->m_pSink) ((pT->*m_pSource)());
    else
	pTask->loadDucWithNA ();
}

template <class T, class A1, class R> void Vsi_f1<T,A1,R>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (!pT) {
	pTask->loadDucWithNA ();
	return;
    }

    A1* pA1 = pTask->loadDucWithNextParameterAsMonotype ()
	? (A1*)pTask->ducMonotype().storeTransientExtensionIfA (A1::RTT)
	: 0;

/********************************************************************
 *  Beware infinite message bounce if both 'T' and 'A1' implement the
 *  required messages but neither expects the other as a parameter.
 ********************************************************************/
    if (pA1)
	(pTask->*this->m_pSink) ((pT->*m_pSource) (pA1));
    else
	pTask->sendBinaryConverseWithCurrent (m_pConverse);
}


/*********************************
 *****  Functions (Boolean)  *****
 *********************************/

template <class T> void Vsi_b0<T>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT)
	(pTask->*this->m_pSink) ((pT->*m_pSource)());
    else
	pTask->loadDucWithNA ();
}

template <class T> void Vsi_b0_c<T>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT)
	(pTask->*this->m_pSink) ((pT->*m_pSource)());
    else
	pTask->loadDucWithNA ();
}

template <class T, class A1> void Vsi_b1<T,A1>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (!pT) {
	pTask->loadDucWithNA ();
	return;
    }

    A1* pA1 = pTask->loadDucWithNextParameterAsMonotype ()
	? (A1*)pTask->ducMonotype().storeTransientExtensionIfA (A1::RTT)
	: 0;

    if (!pA1)
	pTask->sendBinaryConverseWithCurrent (m_pConverse);
    else
	pTask->loadDucWithBoolean ((pT->*m_pSource) (pA1));
}


/****************************************
 *****  Functions (Representative)  *****
 ****************************************/

template <class T, class RSource, class RSink> void Vsi_r0<T,RSource,RSink>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT)
	(pTask->*this->m_pSink) ((RSink)(pT->*m_pSource)());
    else
	pTask->loadDucWithNA ();
}

template <class T, class RSource, class RSink> void Vsi_r0_c<T,RSource,RSink>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT)
	(pTask->*this->m_pSink) ((RSink)(pT->*m_pSource)());
    else
	pTask->loadDucWithNA ();
}


/************************
 *****  Procedures  *****
 ************************/

template <class T> void Vsi_p0<T>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT) {
	(pT->*m_pProcedure)();
	pTask->loadDucWithSelf ();
    }
    else
	pTask->loadDucWithNA ();
}

template <class T> void Vsi_p0_c<T>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT) {
	(pT->*m_pProcedure)();
	pTask->loadDucWithSelf ();
    }
    else
	pTask->loadDucWithNA ();
}


/************************************
 *****  Direct Property Access  *****
 ************************************/

template <class T, class R> void Vsi_property<T,R>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT)
	(pTask->*this->m_pSink)(pT->*m_pSource);
    else
	pTask->loadDucWithNA ();
}


/***********************************
 *****  Property 'get' Access  *****
 ***********************************/

template <class T, class R> void Vsi_getr_c<T,R>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT) {
	R iR;
	(pT->*m_pSource) (iR);
	(pTask->*this->m_pSink)(iR);
    }
    else
	pTask->loadDucWithNA ();
}


/***************************
 *****  Task Function  *****
 ***************************/

template <class T, class R> void Vsi_tf<T,R>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT)
	(pTask->*this->m_pSink) ((pT->*m_pProcessor) (pTask));
    else
	pTask->loadDucWithNA ();
}

/**************************
 *****  Task Boolean  *****
 **************************/

template <class T> void Vsi_tb<T>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT)
	pTask->loadDucWithBoolean ((pT->*m_pProcessor) (pTask));
    else
	pTask->loadDucWithNA ();
}

/***************************************
 *****  Task Procedure (full duc)  *****
 ***************************************/

template <class T> void Vsi_tpFullDuc<T>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT)
	(pT->*m_pProcessor) (pTask);
    else
	pTask->loadDucWithNA ();
}

/***************************************
 *****  Task Procedure (void duc)  *****
 ***************************************/

template <class T> void Vsi_tpVoidDuc<T>::implement (VSNFTask* pTask) const {
    DSC_Descriptor& rCurrent = pTask->getCurrent ();
    T* pT = (T*)rCurrent.storeTransientExtensionIfA (T::RTT);
    if (pT) {
	(pT->*m_pProcessor) (pTask);
	pTask->loadDucWithSelf ();
    }
    else
	pTask->loadDucWithNA ();
}


#endif
