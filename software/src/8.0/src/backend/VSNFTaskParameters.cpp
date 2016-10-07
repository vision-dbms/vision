/*****  VSNFTaskParameters Implementation  *****/

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

#include "VSNFTaskParameters.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VSNFTaskParametersList.h"

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VSNFTaskParameters);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSNFTaskParameters::VSNFTaskParameters (unsigned int xParameter)
: m_xParameter	(xParameter)
{
}

VSNFTaskParametersList::VSNFTaskParametersList (
    VSNFTaskParametersList const& rOther
) : VReference<VSNFTaskParameters> (rOther) {
}

VSNFTaskParametersList::VSNFTaskParametersList () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VSNFTaskParameters::~VSNFTaskParameters () {
}

VSNFTaskParametersList::~VSNFTaskParametersList () {
}
