/*****  Vca::VcaPeerCharacteristics Implementation  *****/

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

#include "Vca_VcaPeerCharacteristics.h"

/************************
 *****  Supporting  *****
 ************************/


/****************************************************************
 ****************************************************************
 *****                                                      *****
 *****  Vca::VcaPeerCharacteristics::VersionRange::Version  *****
 *****                                                      *****
 ****************************************************************
 ****************************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaPeerCharacteristics::VersionRange::Version::Version (
    Version const &rOther
) : m_xFeatureLevel (rOther.featureLevel ()), m_iUnused (rOther.unused ()) 	{
}

Vca::VcaPeerCharacteristics::VersionRange::Version::Version (
    FeatureLevel xFeatureLevel
) : m_xFeatureLevel (xFeatureLevel), m_iUnused (0) {
}

/************************
 ************************
 *****  Assignment  *****
 ************************
 ************************/

Vca::VcaPeerCharacteristics::VersionRange::Version& Vca::VcaPeerCharacteristics::VersionRange::Version::operator= (
    Version const &rOther
) {
    m_xFeatureLevel = rOther.featureLevel ();
    m_iUnused = rOther.unused ();

    return *this;
}

/**************************
 **************************
 *****  Localization  *****
 **************************
 **************************/

void Vca::VcaPeerCharacteristics::VersionRange::Version::reverseByteOrder () {
    Vk_ReverseSizeable (&m_xFeatureLevel);
}


/*******************************************************
 *******************************************************
 *****                                             *****
 *****  Vca::VcaPeerCharacteristics::VersionRange  *****
 *****                                             *****
 *******************************************************
 *******************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaPeerCharacteristics::VersionRange::VersionRange (
    VersionRange const &rOther
) : m_iMaximum (rOther.maximum ()), m_iMinimum (rOther.minimum ()) {
}

Vca::VcaPeerCharacteristics::VersionRange::VersionRange ()
    : m_iMaximum (FeatureLevel_Maximum), m_iMinimum (FeatureLevel_Minimum)
{
}

/************************
 ************************
 *****  Assignment  *****
 ************************
 ************************/

Vca::VcaPeerCharacteristics::VersionRange &Vca::VcaPeerCharacteristics::VersionRange::operator= (
    VersionRange const &rOther
) {
    m_iMaximum = rOther.maximum ();
    m_iMinimum = rOther.minimum ();

    return *this;
}

/**************************
 **************************
 *****  Localization  *****
 **************************
 **************************/

void Vca::VcaPeerCharacteristics::VersionRange::reverseByteOrder () {
    m_iMaximum.reverseByteOrder ();
    m_iMinimum.reverseByteOrder ();
}


/*****************************************
 *****************************************
 *****                               *****
 *****  Vca::VcaPeerCharacteristics  *****
 *****                               *****
 *****************************************
 *****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaPeerCharacteristics::VcaPeerCharacteristics (VcaPeerCharacteristics const &rOther)
: m_xArchitecture	(rOther.architecture ())
, m_iUnused1		(rOther.unused1 ())
, m_iVersionRange	(rOther.versionRange ())
, m_iUUID		(rOther.uuid ())
{
}

Vca::VcaPeerCharacteristics::VcaPeerCharacteristics ()
: m_xArchitecture	(Architecture_Here)
, m_iUnused1		(0)
, m_iUUID		(VkUUID::ReturnUUID ())
{
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool Vca::VcaPeerCharacteristics::represents (uuid_t const &rUUID) const {
    return VkUUID::eq (m_iUUID, rUUID);
}

/**************************
 **************************
 *****  Localization  *****
 **************************
 **************************/

void Vca::VcaPeerCharacteristics::reverseByteOrder () {
    m_iVersionRange.reverseByteOrder ();
    VkUUID::reverseByteOrder (m_iUUID);
}

void Vca::VcaPeerCharacteristics::localize (void *pByteArray, size_t sElement, size_t cElements) const {
    if (architectureIsAntiLocal ()) {
	Vk_ReverseArray (pByteArray, sElement, cElements);
    }
}

void Vca::VcaPeerCharacteristics::localize (VcaPeerCharacteristics &rDatum) const {
    if (architectureIsAntiLocal ())
	rDatum.reverseByteOrder ();
}

void Vca::VcaPeerCharacteristics::localize (uuid_t &rDatum, size_t cElements) const {
    if (architectureIsAntiLocal ()) {
	uuid_t *pDatum = &rDatum;
	while (cElements-- > 0) {
	    VkUUID::reverseByteOrder (*pDatum++);
	}
    }
}

