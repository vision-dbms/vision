/*****  Vps_VRelayPublicationDirectory Implementation  *****/

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

#include "Vps_VRelayPublicationDirectory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VcaGofer.h"

#include "Vps_VRelayPublication.h"


/**************************************************
 **************************************************
 *****					      *****
 *****  Vca::Vps::VRelayPublicationDirectory  *****
 *****					      *****
 **************************************************
 **************************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (Vca::Vps::VRelayPublicationDirectory);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Vps::VRelayPublicationDirectory::VRelayPublicationDirectory (
    VString const &rSource, RelayPublicationRegistrar* registrar
) : m_pSourceGofer (new Gofer::Named<IDirectory,IDirectory> (rSource)), m_pRegistrar(registrar) {
    traceInfo ("Creating Vca::Vps::VRelayPublicationDirectory");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Vps::VRelayPublicationDirectory::~VRelayPublicationDirectory () {
    traceInfo ("Destroying Vca::Vps::VRelayPublicationDirectory");
}


/*************************
 *************************
 *****  Maintenance  *****
 *************************
 *************************/

bool Vca::Vps::VRelayPublicationDirectory::locate (
    Entry::Reference &rpEntry, VString const &rName
) {
    if (BaseClass::locate (rpEntry, rName))
	return true;

    if (supply (rpEntry, rName)) {
        IPublication::Reference pPublication; {
	//  Create new publications and their gofers in the same cohort as the directory...
	    VCohort::Claim iClaim (this, false);

	    IDataSource<IPublication*>::Reference pPublicationSource;
	    (new Gofer::Named<IPublication,IDirectory> (rName, m_pSourceGofer))->getRole (pPublicationSource);
	    (new VRelayPublication (rName, pPublicationSource, m_pRegistrar))->getRole (pPublication);
	}
	rpEntry->setObject (0, this, pPublication);
	return true;
    }
    return false;
}
