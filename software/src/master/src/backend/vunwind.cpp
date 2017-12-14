/*****	Execution Unwinding Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName UNWIND

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "batchvision.h"

#include "vfac.h"
#include "vfault.h"
#include "vstdio.h"

/*****  Shared  *****/
#include "vunwind.h"

#include "Vca.h"


/**************************************
 *****  Global State Definitions  *****
 **************************************/

PrivateVarDef UNWIND_Target*	DestinationStack = NilOf (UNWIND_Target*);

PrivateVarDef unsigned int	ExitInsteadOfLongJump = 0,
				RestartInsteadOfLongJump = 0,
				MaxNumberOfLongJumps = 5;


/********************************************************************
 *****  Externally Visible Internal Long Jump Support Routines  *****
 ********************************************************************/

/*********************************
 *----  Target Construction  ----*
 *********************************/

UNWIND_Target::UNWIND_Target () : m_pPredecessor (DestinationStack), m_fRestarting (false) {
}

/***************************
 *----  Stack Control  ----*
 ***************************/

unsigned int UNWIND_Target::set (int iSetjmpResult) {
    if (iSetjmpResult) {
	m_fRestarting = true;
	return false;
    }

    DestinationStack = this;
    m_fRestarting = false;
    return true;
}

void UNWIND_Target::pop () {
    if (this != DestinationStack)
        panic ();

    DestinationStack = m_pPredecessor;
}

void UNWIND_Target::hop () {

/*****  Jump to the nearest trap or destination, ...  *****/
    if (DestinationStack)
        longjmp (DestinationStack->jumpBuffer, true);

/*****  ... or exit the system.  *****/
    stop ();
}

void UNWIND_Target::stop () {
    Vca::Exit (ErrorExitValue);
}

/*******************
 *----  Panic  ----*
 *******************/

void UNWIND_Target::panic () const {
    IO_fprintf (
	stderr,
	">>> UNWIND Panic. TOS:%08X DD:%08X, Restarting?:%s <<<\n",
	DestinationStack, this, m_fRestarting ? "Y" : "N"
    );
    stop ();
}


/************************************************
 *****  Public Exit and Long Jump Routines  *****
 ************************************************/

PublicFnDef void UNWIND_Exit (int exitValue) {
    Batchvision::completeUsageLog ();
    Vca::Exit (exitValue);
}

PublicFnDef void UNWIND_ThrowException () {
    static unsigned int counter = 0;
/*****  Configurable alternatives to longjmp ()  *****/
    if (ExitInsteadOfLongJump)
	UNWIND_Exit ();
    if (RestartInsteadOfLongJump)
	Batchvision::RestartProcess ();
    if (++counter > MaxNumberOfLongJumps || FAULT_SessionErrorCount >= MaxNumberOfLongJumps)
	UNWIND_Exit ();


/*****  ... and long jump to it:  *****/
    if (DestinationStack)
        DestinationStack->hop ();
    else {
        Vca::Exit (ErrorExitValue);
    }
}


/**************************
 *****  Action Tuning *****
 **************************/

PublicFnDef void UNWIND_SetExit (unsigned int onOff) {
    ExitInsteadOfLongJump = onOff;
}

PublicFnDef void UNWIND_SetRestart (unsigned int onOff) {
    RestartInsteadOfLongJump = onOff;
}

PublicFnDef void UNWIND_SetMaximumNumberOfLongjumps (unsigned int num) {
    MaxNumberOfLongJumps = num;
}

/****************************
 *****  Settings Query  *****
 ****************************/

PublicFnDef unsigned int UNWIND_GetMaximumNumberOfSeriousErrors () {
    return MaxNumberOfLongJumps;
}




/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (UNWIND);
    FAC_FDFCase_FacilityDescription ("Execution Unwinder");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  unwind.c 1 replace /users/mjc/system
  860226 16:25:47 mjc create

 ************************************************************************/
/************************************************************************
  unwind.c 2 replace /users/jad/system
  860422 15:16:22 jad use new io module

 ************************************************************************/
/************************************************************************
  unwind.c 3 replace /users/mjc/translation
  870524 09:42:15 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  unwind.c 4 replace /users/mjc/system
  870526 16:21:30 mjc Deleted redundant (with 'unwind.d') #include <setjmp.h>

 ************************************************************************/
/************************************************************************
  unwind.c 5 replace /users/m2/backend
  890503 15:33:46 m2 STD_ exit values returned

 ************************************************************************/
