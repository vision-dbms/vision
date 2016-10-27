/*****  Vxa_VServerApplication Implementation  *****/

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

#include "Vxa_VServerApplication.h"

/************************
 *****  Supporting  *****
 ************************/


/*************************************
 *************************************
 *****                           *****
 *****  Vxa::VServerApplication  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VServerApplication::VServerApplication (Context *pContext) : BaseClass (pContext) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VServerApplication::~VServerApplication () {
}


/*******************************
 *******************************
 *****  State Transitions  *****
 *******************************
 *******************************/

bool Vxa::VServerApplication::start_() {
    if (!BaseClass::start_())
	return false;

    getRootObject ();

    if (!offerSelf ()) {
	fprintf (stderr, "Usage: No address for offering.\n");
	setExitStatus (ErrorExitValue);
    }

    return isStarting ();
}
