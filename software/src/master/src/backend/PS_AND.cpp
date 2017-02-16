/*****  PS_AND Implementation  *****/

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

#include "PS_AND.h"

/************************
 *****  Supporting  *****
 ************************/

#include "vutil.h"


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

/******************************
 ******************************
 *****  Settings Control  *****
 ******************************
 ******************************/

void PS_AND::SetUpdateAnnotationTo (char const* pValue) {
    if (m_pUpdateAnnotation)
	UTIL_Free ((void*)m_pUpdateAnnotation);

    char *pUpdateAnnotation = (char*)UTIL_Malloc (strlen (pValue) + 1);
    strcpy (pUpdateAnnotation, pValue);

    m_pUpdateAnnotation = pUpdateAnnotation;
}
