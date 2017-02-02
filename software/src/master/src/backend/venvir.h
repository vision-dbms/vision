/*****  Value-Descriptor Facility Header File  *****/
#ifndef ENVIR_H
#define ENVIR_H

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "DSC_Descriptor.h"
#include "RTindex.h"

#include "venvir.d"

class VDatabaseFederatorForBatchvision;


/*************************
 *****  Definitions  *****
 *************************/

class VSession : public VTransient {
//  Construction
public:
    VSession ();

//  Destruction
private:
    void ReleaseTLEComponents ();

public:
    ~VSession ();

//  Initialization
public:
    M_AND *Boot (char const *pNDFPathName);
    M_AND *Open (
	char const *pNDFPathName,
	char const *pVersionSpec,
	char const *pXUSpecPath,
	char const *pObjectSpaceName,
	bool	    makingNewNDF
    );

//  Access
public:
    unsigned int DatabaseActivationCount () const;

    M_AND *InitialDatabase () const {
	return m_pInitialDatabase;
    }

    M_CPD *TheTLEDescriptor () const {
	return m_pTLEDescriptor;
    }

//  Use
public:
    M_AND *Activate (
	char const *pNDFPathName, char const *pVersionSpec = 0, bool makingNewNDF = false
    );

//  Resource Utilization Queries
public:
    void AccumulateAllocationStatistics (
	double *pAllocationTotal, double *pMappingTotal
    ) const;

    double CurrentTransientAllocationLevel () const;
    double TransientAllocationHighWaterMark () const;

//  Resource Utilization Management
public:
    bool DisposeOfSessionGarbage () const;

    void FlushCachedResources () const;

    unsigned int ReclaimSegments () const;
    unsigned int ReclaimAllSegments () const;

//  State
protected:
    VReference<VDatabaseFederatorForBatchvision> m_pDatabaseFederator;
    VReference<M_AND> m_pInitialDatabase;
    M_CPD *m_pTLEDescriptor;
};


/*********************
 *****  Globals  *****
 *********************/

PublicFnDecl VSession *ENVIR_Session ();
PublicFnDecl double ENVIR_SessionMemoryUse ();

PublicVarDecl DSC_Descriptor	ENVIR_KDsc_TheTLE,
				ENVIR_KDsc_TheTmpTLE;

PublicVarDecl rtINDEX_Key*	ENVIR_KTemporalContext_Today;


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  envir.h 1 replace /users/mjc/system
  860531 10:41:43 mjc Added run time 'envir' facility

 ************************************************************************/
/************************************************************************
  envir.h 2 replace /users/mjc/system
  860531 17:57:12 mjc Release new known CPD names

 ************************************************************************/
/************************************************************************
  envir.h 3 replace /users/mjc/system
  860606 13:28:14 mjc Release numeric/primitive function method dictionary initialization

 ************************************************************************/
/************************************************************************
  envir.h 4 replace /users/mjc/system
  860713 17:13:34 mjc Release new format environment structure

 ************************************************************************/
/************************************************************************
  envir.h 5 replace /users/mjc/system
  860728 13:55:54 mjc Added new property types

 ************************************************************************/
/************************************************************************
  envir.h 6 replace /users/mjc/system
  860803 00:18:43 mjc Release new version of system

 ************************************************************************/
/************************************************************************
  envir.h 7 replace /users/mjc/system
  860807 18:05:24 mjc Release basic time operations

 ************************************************************************/
/************************************************************************
  envir.h 8 replace /users/mjc/system
  860826 14:22:00 mjc Added default list and string properties and character behavior, deleted class, metaclass, and date offset

 ************************************************************************/
/************************************************************************
  envir.h 9 replace /users/mjc/system
  861002 18:02:56 mjc Release of modules updated in support of the new list architecture

 ************************************************************************/
/************************************************************************
  envir.h 10 replace /users/jck/system
  870420 13:34:26 jck Initial release of new implementation for time series

 ************************************************************************/
/************************************************************************
  envir.h 11 replace /users/mjc/translation
  870524 09:37:47 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  envir.h 12 replace /users/jck/system
  870605 16:06:44 jck Added known objects to support associative lists

 ************************************************************************/
