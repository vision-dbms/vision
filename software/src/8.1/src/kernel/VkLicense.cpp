/***** VkLicense Implementation *****/

/*************************
 *************************
 *****  Interfaces  ******
 *************************
 *************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"
#include "VpSocket.h"

/************************
 *****  Supporting  *****
 ************************/


/******************
 *****  Self  *****
 ******************/

#include "VkLicense.h"

#if defined(_WIN32)
#pragma comment (lib, "ws2_32.lib")
#endif


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

void VkLicense::construct (VkLicense const& rOther)
{
    memcpy (m_iValue, rOther.m_iValue, sizeof (m_iValue));
}

void VkLicense::construct (char const* pLicenseFile)
{
    construct ();

    FILE* pLicenseStream = fopen (
	pLicenseFile ? pLicenseFile : "/vmaster/src/Data/master.license", "r"
    );
    if (IsNil (pLicenseStream))
	return;

    char iLineBuffer[256];
    unsigned int xItem = 0;
    while (xItem < size () && fgets (iLineBuffer, sizeof (iLineBuffer), pLicenseStream))
    {
	char const* pLineCursor = iLineBuffer;
	unsigned int nCharsRead;
	while (xItem < size () && 1 == sscanf (pLineCursor, "%x, %n", &m_iValue[xItem], &nCharsRead))
	{
	    pLineCursor += nCharsRead;
	    xItem++;
	}
    }

    fclose (pLicenseStream);
}

void VkLicense::construct ()
{
    memset (m_iValue, 0, sizeof (m_iValue));
}


/************************
 ************************
 *****  Comparison  *****
 ************************
 ************************/

bool VkLicense::operator== (VkLicense const& rOther) const {
    for (unsigned int i = 0; i < size (); i++)
	if (m_iValue[i] != rOther.m_iValue[i])
	    return false;

    return true;
}


/************************
 ************************
 *****  Validation  *****
 ************************
 ************************/

bool VkLicense::isValid () const {
    return true;
}


/*********************
 *********************
 *****  Display  *****
 *********************
 *********************/

void VkLicense::print () const
{
    printf ("^tmp Utility setUpdateLicenseTo: ");
    for (unsigned int i = 0; i < size (); i++)
	printf ("%s%s0x%08x", i ? "," : "", i % 4 ? " " : "\n    ", m_iValue[i]);
    printf (";\n");
}
