/*****  Vsa_VsaDirectoryBuilder Implementation *****/

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

#include "Vsa_VsaDirectoryBuilder.h"

/************************
 *****  Supporting  *****
 ************************/

#if defined(_WIN32)		// extra strength compiler happy pill needed here
#include "Vca_VDeviceFactory.h"	// omit:Linux omit:SunOS
#include "Vsa_CompilerHappyPill.h"
#endif

#include "Vca_VCohort.h"
#include "Vca_VDirectory.h"

#include "Vsa_VSmartEvaluatorSource.h"
#include "Vsa_VEvaluatorPool.h"


/*********************************************************************
 *********************************************************************
 *****                                                           *****
 *****  Vsa::VsaDirectoryBuilder::ThisOrder::SpecialPoolSession  *****
 *****                                                           *****
 *********************************************************************
 *********************************************************************/

namespace Vsa {
    class VsaDirectoryBuilder::ThisOrder::SpecialPoolSession: public Session {
	DECLARE_CONCRETE_RTTLITE (SpecialPoolSession, Session);

    //  Constructor
    public:
	SpecialPoolSession (VString iName, SessionType iType, Vca::VDirectory *pDirectory);
    
    //  Destructor
    private:
	~SpecialPoolSession () {}

    //  Query
    public:
	void getInfo (VString &rInfo) const;
        using BaseClass::getInfo;
    private:
	void getStatusMessage_(VString &rInfo) const;

    private:
	bool supplyObjectSource_(IObjectSource::Reference &rpObjectSource) const;

    //  Update
    public:
	void setPoolInfo (VString const &iInfo) {
	    m_iPoolInfo.setTo (iInfo);
	}

	void setWorkersBeingCreated (Vca::U32 iValue) {
	    m_iWorkersBeingCreatedMaximum = iValue;
	}
	void setWorkerMax (Vca::U32 iValue) {
	    m_iWorkerMaximum = iValue;
	}
	void setWorkerMin (Vca::U32 iValue) {
	    m_iWorkerMinimum = iValue;
	}
	void setEvaluationTimeout (Vca::U64 iValue) {
	    m_iEvaluationTimeout = iValue;
	}
	void setEvaluationAttemptMax (Vca::U32 cMax) {
	    m_cEvaluationAttemptMaximum = cMax;
	}
	void setWorkerMinimumAval (Vca::U32 iValue) {
	    m_iWorkerMinimumAvailable = iValue;
	}
	void setWorkerMaximumAvail (Vca::U32 iValue) {
	    m_iWorkerMaximumAvailable = iValue;
	}
	void setWorkerCreationFailureHardLimit (Vca::U32 iValue) {
	    m_iWorkerCreationFailureHardLimit = iValue;
	}
	void setWorkerCreationFailureSoftLimit (Vca::U32 iValue) {
	    m_iWorkerCreationFailureSoftLimit = iValue;
	}
	void setShrinkTimeOut (Vca::U32 iValue) {
	    m_iShrinkTimeOutMins = iValue;   
	}
	void setStopTimeOut (Vca::U32 iValue) {
	    m_iStopTimeOutMins = iValue;   
	}

	void setSourceDirectory (Vca::VDirectory *pDirectory) {
	    m_pDirectory.setTo (pDirectory);
	}
    
    //  State
    private:
	VString		m_iPoolInfo;
	Vca::U32	m_iWorkerMaximum;
	Vca::U32	m_iWorkerMinimum;
	Vca::U32	m_iWorkersBeingCreatedMaximum;
	Vca::U64	m_iEvaluationTimeout;
	Vca::U32	m_cEvaluationAttemptMaximum;
	Vca::U32	m_iWorkerMinimumAvailable;
	Vca::U32	m_iWorkerMaximumAvailable;
	Vca::U32	m_iWorkerCreationFailureHardLimit;
	Vca::U32	m_iWorkerCreationFailureSoftLimit;
	Vca::U32	m_iShrinkTimeOutMins;
	Vca::U32	m_iStopTimeOutMins;
    
	Vca::VDirectory::Reference m_pDirectory;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VsaDirectoryBuilder::ThisOrder::SpecialPoolSession::SpecialPoolSession (
   VString iName, SessionType iType, Vca::VDirectory *pDirectory
) : BaseClass (iName, iType), m_pDirectory (pDirectory)
, m_iWorkerMaximum		(UINT_MAX)
, m_iWorkerMinimum		(0)
, m_iWorkerMinimumAvailable     (0)
, m_iWorkerMaximumAvailable     (0)
, m_iWorkersBeingCreatedMaximum (UINT_MAX)
, m_iEvaluationTimeout          (U64_MAX) 
, m_cEvaluationAttemptMaximum	(UINT_MAX)
, m_iWorkerCreationFailureHardLimit (UINT_MAX)
, m_iWorkerCreationFailureSoftLimit (1)
, m_iShrinkTimeOutMins	        (0)
, m_iStopTimeOutMins	        (0) {
}

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vsa::VsaDirectoryBuilder::ThisOrder::SpecialPoolSession::getInfo (VString &rInfo) const {
    rInfo.setTo (m_iPoolInfo);
}

void Vsa::VsaDirectoryBuilder::ThisOrder::SpecialPoolSession::getStatusMessage_(VString &rInfo) const {
    rInfo << getName ();
}

bool Vsa::VsaDirectoryBuilder::ThisOrder::SpecialPoolSession::supplyObjectSource_(
    IObjectSource::Reference &rpObjectSource
) const {
  Vca::IDirectory::Reference pDirectory;
  m_pDirectory->getRole (pDirectory);
  VSmartEvaluatorSource::Reference const pSource (new VSmartEvaluatorSource (pDirectory));
  pSource->init ();
  IEvaluatorSource::Reference pISource;
  pSource->getRole (pISource);
  
  //  construct the pool settings object
  Vsa::VEvaluatorPool::PoolSettings::Reference const pSettings (new Vsa::VEvaluatorPool::PoolSettings ());
  
  pSettings->setWorkerMaximumTo (m_iWorkerMaximum);
  pSettings->setWorkerMinimumTo (m_iWorkerMinimum);
  pSettings->setWorkersBeingCreatedMaximumTo (m_iWorkersBeingCreatedMaximum);
  pSettings->setEvaluationTimeOutTo (m_iEvaluationTimeout);
  pSettings->setEvaluationAttemptMaximumTo (m_cEvaluationAttemptMaximum);
  pSettings->setWorkerMinimumAvailableTo (m_iWorkerMinimumAvailable);
  pSettings->setWorkerMaximumAvailableTo (m_iWorkerMaximumAvailable);
  pSettings->setWorkerCreationFailureHardLimitTo (m_iWorkerCreationFailureHardLimit);
  pSettings->setWorkerCreationFailureSoftLimitTo (m_iWorkerCreationFailureSoftLimit);
  pSettings->setStopTimeOutTo (m_iStopTimeOutMins);
  pSettings->setShrinkTimeOutTo (m_iShrinkTimeOutMins);

  pSettings->setWorkerSourceTo (pISource);

  pSettings->getRole (rpObjectSource);
  
  return true;
}


/************************************************
 ************************************************
 *****                                      *****
 *****  Vsa::VsaDirectoryBuilder::ThisOrder *****
 *****                                      *****
 ************************************************
 ************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VsaDirectoryBuilder::ThisOrder::ThisOrder (
    VcaDirectoryBuilder* pBuilder, Vca::VDirectory* pDirectory
) : BaseClass (pBuilder, pDirectory) {
  traceInfo ("Creating Vsa::VsaDirectoryBuilder::ThisOrder");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VsaDirectoryBuilder::ThisOrder::~ThisOrder () {
  traceInfo ("Destroying Vsa::VsaDirectoryBuilder::ThisOrder");
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool Vsa::VsaDirectoryBuilder::ThisOrder::includeIfTagAccepted (char const *pTag) const {
    return strcasecmp (pTag, "vsa") == 0 || BaseClass::includeIfTagAccepted (pTag);
}


/*********************
 *********************
 *****  Parsing  *****
 *********************
 *********************/

/*****
 * Method: parseSessionsFile
 *   Contains main loop parsing the Vsa related sessions file...
 *****/

void Vsa::VsaDirectoryBuilder::ThisOrder::parseSessionsFile (
    char const *pSessionsFileName, SessionType xType
) {
    VSimpleFile iSessionsFile;
    if (!pSessionsFileName || !iSessionsFile.Open (pSessionsFileName, "r"))
	return;
    log ("VsaDirectoryBuilder::ThisOrder::parseSessionsFile '%s'", pSessionsFileName);
    VString iBuffer;
    while (iSessionsFile.GetLine (iBuffer)) {
	ParseState xState = ParseState_ExpectingTag;
	char const *pBreakSet = g_pWhitespaceBreakSet;
	char *pBufferPtr;
	bool bIncludeIfTagAccepted = false;
	for (
	    char *pToken = strtok_r (iBuffer.storage (), pBreakSet, &pBufferPtr);
	    pToken && ParseState_Error != xState;
	    pToken = strtok_r (0, pBreakSet, &pBufferPtr)
	) {
	    pToken += strspn (pToken, pBreakSet);	//  needed to work around a buggy VMS strtok.
	    switch (xState) {
	    case ParseState_ExpectingTag:
		xState
		    = 0 == strcasecmp (pToken, "Connection_Template")
		    ? ParseState_ExpectingSessionSentinel
		    : 0 == strcasecmp (pToken, "Include")
		    ? ParseState_ExpectingIncludeTarget
		    : 0 == strcasecmp (pToken, "IncludeIf")
		    ? ParseState_ExpectingIncludeIfTag
		    : 0 == strcasecmp (pToken, "PoolConnection_Template")
		    ? ParseState_ExpectingPoolSessionSentinel
		    : ParseState_ExpectingUnknown;  
		break;
	    case ParseState_ExpectingSessionSentinel:
		if (strcmp (pToken, "Begin") != 0) {
		    log ("VsaDirectoryBuilder::ThisOrder::parseSessionsFile --> Error: found '%s' when expecting 'Begin' for Connection_Template", pToken);
		    xState = ParseState_Error;
		} else {
		    Session::Reference pSession;
		    if (parseSessionTemplate (iSessionsFile, xType, pSession))
			BaseClass::insertSession (pSession);
		    xState = ParseState_ExpectingTag;
		}
		break;
	    case ParseState_ExpectingPoolSessionSentinel:
		if (strcmp (pToken, "Begin") != 0) {
		    log ("VsaDirectoryBuilder::ThisOrder::parseSessionsFile --> Error: found '%s' when expecting 'Begin' for PoolConnection_Template", pToken);
		    xState = ParseState_Error;
		} else {
		    Session::Reference pSession;
		    if (parsePoolSessionTemplate (iSessionsFile, xType, pSession))
			BaseClass::insertSession (pSession);
		    xState = ParseState_ExpectingTag; 
		}
		break;
	    case ParseState_ExpectingIncludeTarget:
		parseSessionsFile (pToken, xType);
		xState = ParseState_ExpectingTag;
		break;
	    case ParseState_ExpectingIncludeIfTag:
		bIncludeIfTagAccepted = includeIfTagAccepted (pToken);
		xState = ParseState_ExpectingIncludeIfTarget;
		break;
	    case ParseState_ExpectingIncludeIfTarget:
		if (bIncludeIfTagAccepted)
		    parseSessionsFile (pToken, xType);
		xState = ParseState_ExpectingTag;
		break;
	    case ParseState_ExpectingUnknown:
		xState = ParseState_ExpectingTag;
	    //  ignore the unknown value...
		break;
	
	    default:
		log ("VsaDirectoryBuilder::ThisOrder::parseSessionsFile --> Error: unknown parse state %d[%s]", xState, pToken);
		xState = ParseState_Error;
		break;
	    }
	//  set the break set to use for next parsing iteration
	    switch (xState) {
	    case ParseState_ExpectingIncludeTarget:
	    case ParseState_ExpectingIncludeIfTarget:
	    case ParseState_ExpectingPoolSessionSentinel:
	    case ParseState_ExpectingSessionSentinel:
	    case ParseState_ExpectingUnknown:
		pBreakSet = g_pRestOfLineBreakSet;
		break;
	    default:
		pBreakSet = g_pWhitespaceBreakSet;
	    }
	}
	iBuffer.clear ();
    }
    iSessionsFile.close ();
}


bool Vsa::VsaDirectoryBuilder::ThisOrder::parsePoolSessionTemplate (
    VSimpleFile &rSessionsFile, SessionType xType, Session::Reference &rpSession
) const {

    bool bCompleted = false;

    Vca::U32 iMaxWorkers (UINT_MAX), iMinWorkers (0), iMinAvail (0), iMaxAvail (0);
    Vca::U32 iEvalAttempts (UINT_MAX) , iWorkerCreationFailureHardLmt (UINT_MAX), iWorkerCreationFailureSoftLmt (1);
    Vca::U32 iWorkersBeingCreated (UINT_MAX), iStopTimeOut (0), iShrinkTimeOut (0);
    Vca::U64 iEvaluationTimeout (U64_MAX);

    VString iSpecialPoolName;

    Vca::VCohort::Claim iClaim (Vca::VCohort::Vca (), false);
    Directory::Reference const pLocalDirectory (new Directory ());
    ThisOrder::Reference const pNewOrder (new ThisOrder (builder (), pLocalDirectory));
    
    VString iBuffer;
    while (!bCompleted && rSessionsFile.GetLine (iBuffer)) {
      ParseState xState = ParseState_ExpectingTag;
      char const *pBreakSet = g_pWhitespaceBreakSet;
      char *pBufferPtr;
      for (
	   char *pToken = strtok_r (iBuffer.storage (), pBreakSet, &pBufferPtr);
	   pToken && ParseState_Error != xState;
	   pToken = strtok_r (0, pBreakSet, &pBufferPtr)
	   ) {
	pToken += strspn (pToken, g_pWhitespaceBreakSet);	//  needed to work around a buggy VMS strtok.
	switch (xState) {
	case ParseState_ExpectingTag:
	  xState = 0 == strcasecmp (pToken, "Name")
            ? ParseState_ExpectingPoolSessionName
	    : 0 == strcasecmp (pToken, "Connection_Template")
	    ? ParseState_ExpectingSessionSentinel
	    : 0 == strcasecmp (pToken, "PoolConnection_Template")
	    ? ParseState_ExpectingPoolSessionSentinel
	    : 0 == strcasecmp (pToken, "Option")
	    ? ParseState_ExpectingOption
	    : 0 == strcasecmp (pToken, "MaxWorkers")
	    ? ParseState_ExpectingWorkerMax
	    : 0 == strcasecmp (pToken, "MinWorkers")
	    ? ParseState_ExpectingWorkerMin
	    : 0 == strcasecmp (pToken, "MinAvailWorkers")
	    ? ParseState_ExpectingWorkerMinAvail
	    : 0 == strcasecmp (pToken, "MaxAvailWorkers")
	    ? ParseState_ExpectingWorkerMaxAvail
	    : 0 == strcasecmp (pToken, "WorkersBeingCreated")
	    ? ParseState_ExpectingWorkersBeingCreated
	    : 0 == strcasecmp (pToken, "EvaluationTimeout")
	    ? ParseState_ExpectingEvaluationTimeout
	    : 0 == strcasecmp (pToken, "EvaluationAttempts")
	    ? ParseState_ExpectingEvaluationAttempts
	    : 0 == strcasecmp (pToken, "WorkerCreationFailureHardLimit")
	    ? ParseState_ExpectingWorkerCreationFailureHardLmt
	    : 0 == strcasecmp (pToken, "WorkerCreationFailureSoftLimit")
	    ? ParseState_ExpectingWorkerCreationFailureSoftLmt
	    : 0 == strcasecmp (pToken, "ShrinkTimeOut")
	    ? ParseState_ExpectingShrinkTimeOut
	    : 0 == strcasecmp (pToken, "StopTimeOut")
	    ? ParseState_ExpectingStopTimeOut
	    : ParseState_Error;  
	  if (ParseState_Error == xState)
	      log ("VsaDirectoryBuilder::ThisOrder::parsePoolSessionTemplate --> Unknown tag '%s'", pToken);
	  break;
	case ParseState_ExpectingSessionSentinel:
	  if (strcmp (pToken, "Begin") == 0) {
	    Session::Reference pSession;
	    bool bResult = parseSessionTemplate (rSessionsFile, xType, pSession);
            if (bResult) {
              pNewOrder->insertSession (pSession);
            }
          }
	  else {
	    log ("VsaDirectoryBuilder::ThisOrder::parsePoolSessionTemplate --> Error: found '%s' when expecting 'Begin'", pToken);
	    xState = ParseState_Error;
	  }
 	  break;
	case ParseState_ExpectingPoolSessionSentinel:
	  if (strcmp (pToken, "End") == 0) {
	    bCompleted = true;
          }
	  else if (strcmp (pToken, "Begin") == 0) { //  pool inside pool
	    Session::Reference pSession;
            bool bResult = parsePoolSessionTemplate (rSessionsFile, xType, pSession);
            if (bResult) {
              pNewOrder->insertSession (pSession);
            }
            
            xState = ParseState_ExpectingTag; 
          }
          else {
	    log ("VsaDirectoryBuilder::ThisOrder::parsePoolSessionTemplate --> Error: found '%s' when expecting 'Begin' or 'End'", pToken);
	    xState = ParseState_Error;
	  }
	  break;
	case ParseState_ExpectingPoolSessionName:
	  iSpecialPoolName.setTo (pToken);
	  break;
	case ParseState_ExpectingOption:
	  if (0 == strcasecmp (pToken, "hidden"))
	    xType = SESSION_HIDDEN;
	  break;
        case ParseState_ExpectingWorkerMax:
          iMaxWorkers = atoi (pToken);
          break;
        case ParseState_ExpectingWorkerMin:
          iMinWorkers = atoi (pToken);
          break;
        case ParseState_ExpectingWorkerMaxAvail:
          iMaxAvail = atoi (pToken);
          break;
        case ParseState_ExpectingWorkerMinAvail:
          iMinAvail = atoi (pToken);
          break;
        case ParseState_ExpectingWorkersBeingCreated:
          iWorkersBeingCreated = atoi (pToken);
          break;
        case ParseState_ExpectingEvaluationTimeout:
          iEvaluationTimeout = atoll (pToken);
          break;
        case ParseState_ExpectingEvaluationAttempts:
          iEvalAttempts = atoi (pToken);
          break;
        case ParseState_ExpectingWorkerCreationFailureHardLmt:
          iWorkerCreationFailureHardLmt = atoi (pToken);          
          break;
        case ParseState_ExpectingWorkerCreationFailureSoftLmt:
          iWorkerCreationFailureSoftLmt = atoi (pToken);          
          break;
        case ParseState_ExpectingShrinkTimeOut:
          iShrinkTimeOut = atoi (pToken);
          break;
        case ParseState_ExpectingStopTimeOut:
          iStopTimeOut = atoi (pToken);
          break;
	default:
	  log ("VsaDirectoryBuilder::ThisOrder::parsePoolSessionTemplate --> Error: unknown parse state %d[%s]", xState, pToken);
  
	  xState = ParseState_Error;
	  break;
	}
        //  set the break set to use for next parsing iteration
	switch (xState) {
	case ParseState_ExpectingTag:
	  pBreakSet = g_pWhitespaceBreakSet;
	  break;
	default:
	  pBreakSet = g_pRestOfLineBreakSet;
	  break;
	}
      }
      
      iBuffer.clear ();
    } 
    if (bCompleted) {

      //  convert evaluation timeout secs to microsecs with overflow checking..
      Vca::U64 iEvaluationTimeoutMicroSecs;
      if (iEvaluationTimeout > U64_MAX/1000000) 
        iEvaluationTimeoutMicroSecs = U64_MAX;
      else
        iEvaluationTimeoutMicroSecs = iEvaluationTimeout * 1000000;

      //  get directory description
      VString iDescription;
      pNewOrder->getDirectoryDescription (iDescription);
      
      //  elaborate description with pool parameters
      VString iPoolDesc;
      iPoolDesc << "\r\nPOOL_BEGIN: " <<iSpecialPoolName << "\r\n" << iDescription; 
      iPoolDesc << "\r\nMAX WORKERS: " << iMaxWorkers;
      iPoolDesc << "\r\nMIN WORKERS: " << iMinWorkers;
      iPoolDesc << "\r\nWORKER MIN AVAILABLE: " << iMinAvail;
      iPoolDesc << "\r\nWORKER MAX AVAILABLE: " << iMaxAvail;
      iPoolDesc << "\r\nWORKERS IN CREATION LMT: " << iWorkersBeingCreated;
      iPoolDesc << "\r\nEVALUATION TIME OUT (secs): " << iEvaluationTimeout;
      iPoolDesc << "\r\nEVALUATION ATTEMPTS: " << iEvalAttempts;
      iPoolDesc << "\r\nWORKER CREATION FAILURE SOFT LMT: " << iWorkerCreationFailureSoftLmt;
      iPoolDesc << "\r\nWORKER CREATION FAILURE HARD LMT: " << iWorkerCreationFailureHardLmt;
      iPoolDesc << "\r\nSHRINK TIMEOUT (mins): " << iShrinkTimeOut;
      iPoolDesc << "\r\nSTOP TIMEOUT (mins): " << iStopTimeOut << "\r\n";
      iPoolDesc << "\r\nPOOL_END: " << iSpecialPoolName << "\r\n";

      SpecialPoolSession::Reference pSession;
      pSession.setTo (new SpecialPoolSession (iSpecialPoolName, xType, pNewOrder->m_pDirectory));
      pSession->setPoolInfo (iPoolDesc);
      pSession->setWorkerMax (iMaxWorkers);
      pSession->setWorkerMin (iMinWorkers);
      pSession->setWorkerMinimumAval (iMinAvail);
      pSession->setWorkerMaximumAvail (iMaxAvail);
      pSession->setWorkersBeingCreated (iWorkersBeingCreated);
      pSession->setEvaluationTimeout (iEvaluationTimeoutMicroSecs);
      pSession->setEvaluationAttemptMax (iEvalAttempts);
      pSession->setWorkerCreationFailureHardLimit (iWorkerCreationFailureHardLmt);
      pSession->setWorkerCreationFailureSoftLimit (iWorkerCreationFailureSoftLmt);
      pSession->setShrinkTimeOut (iShrinkTimeOut);
      pSession->setStopTimeOut (iStopTimeOut);
      rpSession.setTo (pSession);
    }

    return bCompleted;
}


bool Vsa::VsaDirectoryBuilder::ThisOrder::insertSession (Session *pSession) {
  if (pSession) {
    Session::gofer_t::Reference pSource;
    VString iName (pSession->getName ());
    VString iDesc; pSession->getInfo (iDesc);
    if (pSession->supplyObjectSource (pSource)) {
      //  as this is a session inside poolsession, 
      //  description without the extra "GLOBAL/LOCAL",status message tags at the end will suffice
      m_pDirectory->setSource (iName, pSource, iDesc);

      return true;
    }
    log ("VsaDirectoryBuilder::ThisOrder::insertSession --> Faulty template specification ... '%s'[%s] ",
	     iName.content(), iDesc.content());
  }
  return false;
}


/**************************************
 **************************************
 *****                            *****
 *****  Vsa::VsaDirectoryBuilder  *****
 *****                            *****
 **************************************
 **************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VsaDirectoryBuilder::VsaDirectoryBuilder () {
  traceInfo ("Creating Vsa::VsaDirectoryBuilder");  
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VsaDirectoryBuilder::~VsaDirectoryBuilder () {
  traceInfo ("Destroying Vsa::VsaDirectoryBuilder");
}

/*******************
 *******************
 *****  Build  *****
 *******************
 *******************/

void Vsa::VsaDirectoryBuilder::build (Vca::VDirectory *pDirectory) {
    ThisOrder::Reference const pOrder (new ThisOrder (this, pDirectory));
    pOrder->buildDirectory ();
}
