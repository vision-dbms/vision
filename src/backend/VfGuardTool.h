/*****  Vision Function Manager Guard Tool Interface  *****/
#ifndef VfGuardTool_Interface
#define VfGuardTool_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VCPDReference.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "RTlink.h"

/*************************
 *****  Definitions  *****
 *************************/

class VfGuardTool : public VTransient {
//  Construction
public:
    VfGuardTool (rtPTOKEN_Handle *pCod);

//  Destruction
public:
    ~VfGuardTool ();

//  Use
protected:
    void endValidRun ();

public:
    void addNext () {
	m_iValidRunSize++;
    }

    void skipNext () {
	if (m_pGuard && 0 == m_iValidRunSize)
	    m_iValidRunOrigin++;
	else endValidRun ();
    }

    rtLINK_CType* commit ();

//  State
protected:
    rtPTOKEN_Handle::Reference	m_pCod;
    rtLINK_CType*		m_pGuard;
    size_t			m_iDomCardinality;
    size_t			m_iValidRunOrigin;
    size_t			m_iValidRunSize;
};


#endif
