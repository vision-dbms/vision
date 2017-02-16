/*****  Vision Function Manager Guard Tool Implementation  *****/

/***********************
 ***********************
 *****  Interfaces *****
 ***********************
 ***********************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VfGuardTool.h"

/************************
 *****  Supporting  *****
 ************************/

#include "m.h"

#include "vdsc.h"

#include "RTptoken.h"


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VfGuardTool::VfGuardTool (rtPTOKEN_Handle *pCod)
: m_pCod (pCod), m_pGuard (0), m_iDomCardinality (0), m_iValidRunOrigin (0), m_iValidRunSize (0)
{
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VfGuardTool::~VfGuardTool () {
    if (m_pGuard)
	m_pGuard->release ();
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

/***********************************
 *****  Valid Run Termination  *****
 ***********************************/

void VfGuardTool::endValidRun () {
    if (IsNil (m_pGuard))
	m_pGuard = rtLINK_RefConstructor (m_pCod);
    if (m_iValidRunSize > 0) {
	rtLINK_AppendRange (m_pGuard, m_iValidRunOrigin, m_iValidRunSize);
	m_iDomCardinality += m_iValidRunSize;
	m_iValidRunOrigin += m_iValidRunSize;
	m_iValidRunSize    = 0;
    }
    m_iValidRunOrigin++;
}


/********************
 *****  Commit  *****
 ********************/

rtLINK_CType *VfGuardTool::commit () {
//  If a guard wasn't created, the guarded map is total,...
    if (IsNil (m_pGuard))
	return NilOf (rtLINK_CType*);

//  ... otherwise, close...
    if (m_iValidRunSize > 0) {
	rtLINK_AppendRange (m_pGuard, m_iValidRunOrigin, m_iValidRunSize);
	m_iDomCardinality += m_iValidRunSize;
    }
    m_pGuard->Close (new rtPTOKEN_Handle (m_pCod->ScratchPad (), m_iDomCardinality));

//  ... and return the guard:
    rtLINK_CType *pGuard = m_pGuard;
    m_pGuard = NilOf (rtLINK_CType*);

    return pGuard;
}
