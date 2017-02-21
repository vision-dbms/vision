/*****  VCodeDescriptor Implementation  *****/

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

#include "VCodeDescriptor.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VCodeDescriptor::VCodeDescriptor (VCodeDescriptor const& rOther)
: m_xCode (rOther.m_xCode), m_pName (rOther.m_pName)
{
}

VCodeDescriptor::VCodeDescriptor (unsigned int xCode, char const* pName)
: m_xCode (xCode), m_pName (pName)
{
}
