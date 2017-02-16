/*****  Vca_VStreamSource Implementation  *****/

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

#include "Vca_VStreamSource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VStatus.h"


/***************************
 ***************************
 *****                 *****
 *****  VStreamSource  *****
 *****                 *****
 ***************************
 ***************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VStreamSource::VStreamSource (
    DeviceFace *pDeviceFace
) : BaseClass (pDeviceFace->deviceCohort ()), DeviceUser (this, pDeviceFace) {
    traceInfo ("Creating VStreamSource");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VStreamSource::~VStreamSource () {
    traceInfo ("Destroying VStreamSource");
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

size_t Vca::VStreamSource::GetByteCount () {
    return BaseClass::GetByteCount ();
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void Vca::VStreamSource::endTransfers () {
    close ();
}

/***************************
 ***************************
 *****  Data Transfer  *****
 ***************************
 ***************************/

void Vca::VStreamSource::transferData () {
    VStatus iStatus; char *pVoid; size_t sVoid;
    while (finish (iStatus)
	&& iStatus.isCompleted ()
	&& getArea (pVoid, sVoid)
	&& start (iStatus, pVoid, sVoid)
    );
    onStatus (iStatus);
}
