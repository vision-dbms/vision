/*****  Vca_VStatus Implementation  *****/

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

#include "Vca_VStatus.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IClient.h"
#include "V_VString.h"


/***********************
 ***********************
 *****  Reporting  *****
 ***********************
 ***********************/

bool Vca::VStatus::communicateTo (IClient *pClient) const {
    bool bClosedOrFailed = false;
    if (pClient) {
	if (isClosed ()) {
	    pClient->OnEnd ();
	    bClosedOrFailed = true;
	}
	else if (isFailed ()) {
	    VString iMessage;
	    getDescription (iMessage);
	    pClient->OnError (0, iMessage);
	    bClosedOrFailed = true;
	}
    }
    return bClosedOrFailed;
}
