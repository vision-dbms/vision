/*****  VPrimitiveTask Implementation  *****/

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

#include "VPrimitiveTask.h"

/************************
 *****  Supporting  *****
 ************************/

#include "verrdef.h"

#include "VExternalGroundStore.h"
#include "VListEnumerationController.h"
#include "VMutexController.h"
#include "VReadEvalPrint.h"
#include "VWhileTrueController.h"

#include "VSymbol.h"
#include "VSymbolImplementation.h"

/*****  External Object Support *****/
#include "V_VString.h"

#include "Vca_IDirectory.h"
#include "Vca_IPipeFactory.h"

#include "Vxa_ISingleton.h"


/**********************************
 **********************************
 *****                        *****
 *****  VPrimitiveDescriptor  *****
 *****                        *****
 **********************************
 **********************************/

/********************************
 ********************************
 *****  Task Instantiation  *****
 ********************************
 ********************************/

VPrimitiveTaskBase* VPrimitiveDescriptor::instantiate (
    VTask::ConstructionData const& rTCData, unsigned short iFlags
) {
    m_iUseCount++;

    switch (m_xIClass) {
    case IClass_PrimitiveTask:
	return new VPrimitiveTask (rTCData, this, iFlags);

    case IClass_ListEnumerationController:
	return new VListEnumerationController (rTCData, this, iFlags);

    case IClass_MutexController:
	return new VMutexController (rTCData, this, iFlags);

    case IClass_ReadEvalPrintController:
	return new VReadEvalPrintController (rTCData, this, iFlags);

    case IClass_WhileTrueController:
	return new VWhileTrueController (rTCData, this, iFlags);

    default:
	raiseException (
	    EC__InternalInconsistency,
	    "VPrimitiveDescriptor::instantiate: Invalid IClass Index %u For Primitive '%s[%u]'",
	    m_xIClass,
	    m_pName,
	    m_xPrimitive
	);
	return 0;
    }
}


/********************************
 ********************************
 *****                      *****
 *****  VPrimitiveTaskBase  *****
 *****                      *****
 ********************************
 ********************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT (VPrimitiveTaskBase);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

template class Vsi_f0_c<VPrimitiveTaskBase, unsigned int>;

void VPrimitiveTaskBase::MetaMaker () {
    static Vsi_f0_c<VPrimitiveTaskBase, unsigned int> const
	si_primitiveIndex	(&VPrimitiveTaskBase::primitive),
	si_primitiveFlags	(&VPrimitiveTaskBase::flags);

    CSym ("isAPrimitiveTask"	)->set (RTT, &g_siTrue);

    CSym ("primitiveIndex"	)->set (RTT, &si_primitiveIndex);
    CSym ("primitiveFlags"	)->set (RTT, &si_primitiveFlags);
}

/***************************************
 ***************************************
 *****  Primitive Dispatch Vector  *****
 ***************************************
 ***************************************/

VPrimitiveDescriptor* VPrimitiveTaskBase::g_iPDV [V_PF_MaxPrimitiveIndex];


/*************************************
 *************************************
 *****  Display and Description  *****
 *************************************
 *************************************/

void VPrimitiveTaskBase::reportInfo (unsigned int xLevel, VCall const* Unused(pContext)) const {
    reportInfoHeader (xLevel);

    report (m_iFlags ? "<%s[%u]>\n" : "<%s>\n", m_pDescriptor->name (), m_iFlags);
}

void VPrimitiveTaskBase::reportTrace () const {
    reportTraceHeader ("P");

    report (m_iFlags ? "%s <%04X>\n" : "%s\n", m_pDescriptor->name (), m_iFlags);
}

char const* VPrimitiveTaskBase::description () const {
    return string (m_iFlags ? "<%s[%u]>" : "<%s>", m_pDescriptor->name (), m_iFlags);
}


/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

void VPrimitiveTaskBase::raiseUnimplementedAliasException (char const* pCaller) {
    if (m_iFlags) raiseException (
	EC__InternalInconsistency,
	"Alias '%s[%u]' Not Recognized By '%s'",
	m_pDescriptor->name (),
	m_iFlags,
	pCaller
    );
    else raiseException (
	EC__InternalInconsistency,
	"Alias '%s' Not Recognized By '%s'",
	m_pDescriptor->name (),
	pCaller
    );
}

/*************************************
 *************************************
 *****  External Object Support  *****
 *************************************
 *************************************/

void VPrimitiveTaskBase::accessClientObject () {
    object_gofer_t::Reference pObjectGofer;
    if (channel()->objectGofer (pObjectGofer))
	accessExternalObject (pObjectGofer);
    else {
	suspend ();
	onExternalObject (0);
    }
}

void VPrimitiveTaskBase::accessExternalObject (VString const &rObjectName, bool bUsingDirectory) {
    object_gofer_t::Reference const pObjectGofer (
	bUsingDirectory ? static_cast<object_gofer_t*> (
	    new Vca::Gofer::Named<Vxa::ISingleton,Vca::IDirectory>(rObjectName)
	) : static_cast<object_gofer_t*> (
	    new Vca::Gofer::Named<Vxa::ISingleton,Vca::IPipeFactory>(rObjectName)
	)
    );
    accessExternalObject (pObjectGofer);
}


void VPrimitiveTaskBase::accessExternalObject (object_gofer_t* pObjectGofer) {
    suspend ();
    if (!pObjectGofer)
	onExternalObject (0);
    else {
	pObjectGofer->supplyMembers (this, &ThisClass::onExternalObject, &ThisClass::onExternalError);
    }
}

void VPrimitiveTaskBase::onExternalError (Vca::IError *pInterface, VString const &rMessage) {
    loadDucWith (rMessage);
    resume ();
}

void VPrimitiveTaskBase::onExternalObject (Vxa::ISingleton *pInterface) {
    if (pInterface && pInterface->oidr ())
	pInterface->oidr()->logAsInteresting ("OXO");
    if (!pInterface)
        loadDucWithNA ();
    else loadDucWithRepresentative (
	new VExternalGroundStore (pInterface)
    );
    resume ();
}


/****************************
 ****************************
 *****                  *****
 *****  VPrimitiveTask  *****
 *****                  *****
 ****************************
 ****************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VPrimitiveTask);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

void VPrimitiveTask::MetaMaker () {
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

void VPrimitiveTask::run () {
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

		(*pContinuation) (this);
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
