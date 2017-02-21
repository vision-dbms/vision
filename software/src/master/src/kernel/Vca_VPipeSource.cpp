/*****  Vca_VPipeSource Implementation  *****/

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

#include "Vca_VPipeSource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#include "Vca_IPipeFactory.h"	// omit:Linux omit:SunOS

#include "Vca_VcaGofer.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::VPipeSource  *****
 *****                    *****
 ******************************
 ******************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VPipeSource::VPipeSource () : m_pIObjectSource (this), m_pIPipeSource (this) {
}

/***********************************
 ***********************************
 *****  IObjectSource Methods  *****
 ***********************************
 ***********************************/

void Vca::VPipeSource::Supply (IObjectSource *pRole, IObjectSink *pClient) {
    VQueryInterface<IVUnknown> iQuery (pClient);
    IPipeSource::Reference pIPipeSource;
    getRole (pIPipeSource);
    Gofer::Supply (iQuery, pIPipeSource);
}

/*********************************
 *********************************
 *****  IPipeSource Methods  *****
 *********************************
 *********************************/

void Vca::VPipeSource::Supply (IPipeSource *pRole, IPipeSourceClient *pClient) {
    if (pClient) {
	supply_(pClient);
    }
}
