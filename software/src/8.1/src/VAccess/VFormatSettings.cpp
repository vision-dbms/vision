/*****  VFormatSettings Implementation  *****/

/**************************
 **************************
 *****  Declarations  *****
 **************************
 **************************/

/********************
 *****  System  *****
 ********************/

#include "VStdLib.h"

#include <math.h>

/******************
 *****  Self  *****
 ******************/

#include "VFormatSettings.h"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****                   *****
 *****  VFormatSettings  *****
 *****                   *****
 *****************************
 *****************************/

/**************************************
 *****  Referenceable Requisites  *****
 **************************************/

DEFINE_CONCRETE_RTT (VFormatSettings);

/*********************
 *****  Globals  *****
 *********************/

VReference<VFormatSettings> const VFormatSettings::GlobalSettings(new VFormatSettings (0));

/**************************
 *****  Construction  *****
 **************************/

VFormatSettings::VFormatSettings (VFormatSettings* pParent)
: m_pParent			(pParent)
, m_fSetsDoubleNaN		(false)
, m_fSetsLongNaN		(false)
, m_fSetsReturningUnicode	(false)
{
}


/********************
 *****  Access  *****
 ********************/

double VFormatSettings::doubleNaN () const {
    VFormatSettings const* pSettings = this;

    while (pSettings) {
	if (pSettings->m_fSetsDoubleNaN)
	    return pSettings->m_iDoubleNaN;
	pSettings = pSettings->parent ();
    }
    return ::log (-1.0);
}

long VFormatSettings::longNaN () const {
    VFormatSettings const* pSettings = this;

    while (pSettings) {
	if (pSettings->m_fSetsLongNaN)
	    return pSettings->m_iLongNaN;
	pSettings = pSettings->parent ();
    }
    return INT_MIN;
}

bool VFormatSettings::returningUnicode () const {
    VFormatSettings const* pSettings = this;

    while (pSettings) {
	if (pSettings->m_fSetsReturningUnicode)
	    return pSettings->m_fReturningUnicode;
	pSettings = pSettings->parent ();
    }
    return true;
}
