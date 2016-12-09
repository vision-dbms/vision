/*****  Vca_Registry_VAuthority Implementation  *****/
#define Vca_Registry_VAuthority_Implementation

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

#include "Vca_Registry_VAuthority.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VcaSerializer.h"


/*********************************************
 *********************************************
 *****                                   *****
 *****  Vca::Registry::VAuthority::Path  *****
 *****                                   *****
 *********************************************
 *********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Registry::VAuthority::Path::Path (
    ThisClass *pPrefix, IAuthority *pAuthority
) : m_pAuthority (pAuthority), m_pPrefix (pPrefix), m_iLength (pPrefix ? pPrefix->length () + 1 : 1) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Registry::VAuthority::Path::~Path () {
}


/***************************************************
 ***************************************************
 *****                                         *****
 *****  Vca::Registry::VAuthority::Serializer  *****
 *****                                         *****
 ***************************************************
 ***************************************************/

namespace Vca {
    namespace Registry {
	class VAuthority::Serializer : public VcaSerializer {
	    DECLARE_CONCRETE_RTTLITE (Serializer, VcaSerializer);

	//  Construction
	public:
	    
	//  Destruction
	private:

	//  State
	private:
	};
    }
}


/***************************************
 ***************************************
 *****                             *****
 *****  Vca::Registry::VAuthority  *****
 *****                             *****
 ***************************************
 ***************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Registry::VAuthority::VAuthority (
    ThisClass const &rPrefix, IAuthority *pAuthority
) : m_pPath (new Path (rPrefix.path (), pAuthority)) {
}

Vca::Registry::VAuthority::VAuthority (ThisClass const &rOther) : m_pPath (rOther.path ()) {
}

Vca::Registry::VAuthority::VAuthority () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Registry::VAuthority::~VAuthority () {
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/
