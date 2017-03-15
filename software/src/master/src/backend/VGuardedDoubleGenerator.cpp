/*****  VGuardedDoubleGenerator Implementation  *****/

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

#include "VGuardedDoubleGenerator.h"

/************************
 *****  Supporting  *****
 ************************/

#include "RTdoubleuv.h"


/**************************************
 **************************************
 *****  Guarded Double Generator  *****
 **************************************
 **************************************/

/**************************
 *****  Construction  *****
 **************************/

VGuardedDoubleGenerator::VGuardedDoubleGenerator (VTask* pTask)
: m_pTask   (pTask)
, m_pVector (0, pTask->NewDoubleUV ())
, m_iGuard  (pTask->ptoken ())
{
    m_pCursor = rtDOUBLEUV_CPD_Array (m_pVector);
}

/********************
 *****  Commit  *****
 ********************/

void VGuardedDoubleGenerator::commit () {
    rtLINK_CType* pGuardLink = m_iGuard.commit ();
    if (pGuardLink) {
	m_pTask->loadDucWithPartialFunction (pGuardLink, m_pVector);
	pGuardLink->release ();
    }
    else
	m_pTask->loadDucWithMonotype (m_pVector);
}
