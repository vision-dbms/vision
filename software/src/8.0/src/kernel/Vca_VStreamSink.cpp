/*****  Vca_VStreamSink Implementation  *****/

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

#include "Vca_VStreamSink.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VStatus.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::VStreamSink  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VStreamSink::VStreamSink (
    DeviceFace *pDeviceFace
) : BaseClass (pDeviceFace->deviceCohort ()), DeviceUser (this, pDeviceFace) {
    traceInfo ("Creating VStreamSink");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VStreamSink::~VStreamSink () {
    traceInfo ("Destroying VStreamSink");
}

/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void Vca::VStreamSink::endTransfers () {
    close ();
}

/***************************
 ***************************
 *****  Data Transfer  *****
 ***************************
 ***************************/

void Vca::VStreamSink::transferData () {
    VStatus iStatus; char const *pData; size_t sData;
    while (finish (iStatus)
	&& iStatus.isCompleted ()
	&& getArea (pData, sData)
	&& start (iStatus, pData, sData)
    );
    onStatus (iStatus);
}
