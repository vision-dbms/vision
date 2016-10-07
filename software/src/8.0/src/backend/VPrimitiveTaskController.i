#ifndef VPrimitiveTaskController_Implementation
#define VPrimitiveTaskController_Implementation

/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

template <class DerivedClass> void VPrimitiveTaskController<DerivedClass>::run () {
    switch (m_xStage) {
    case Stage_Unrun:
	if (pausedOnEntry ())
	    return;
	/*****  NO BREAK  *****/

    case Stage_Entry:
	m_xStage = Stage_Running;
	/*****  NO BREAK  *****/

    case Stage_Running:
	while (runnable ()) {
	    if (m_pContinuation) {
		Continuation pContinuation = m_pContinuation;
		m_pContinuation = NilOf (Continuation);

		(((DerivedClass*)this)->*pContinuation) ();
	    }
	    else if (pausedOnExit ())
		return;
	    else
		exit ();
	}
	break;

    default:
	exit ();
	break;
    }
}

#endif
