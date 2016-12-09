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

#include "VTransientServices.h" 


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

    typedef VkMapOf<VString, char const*, char const*, Vca::U32> BusynessOverrideMap; 
        
    //  Constructor
    public:
	SpecialPoolSession (VString iName, SessionType iType);
    
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
	void setEvaluationOnErrorAttemptMax (Vca::U32 cMax) {
	    m_cEvaluationOnErrorAttemptMaximum = cMax;
	}
	void setEvaluationTimeOutAttemptMax (Vca::U32 cMax) {
	    m_cEvaluationTimeOutAttemptMaximum = cMax;
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
    
	void setBusynessThreshold (Vca::U32 iValue) {
	    m_iBusynessThreshold = iValue;
	}
		
	void setMaxBusynessChecks (Vca::U32 iValue) {
	    m_iMaxBusynessChecks = iValue;
	}
		
	void addBusynessMapEntry (const VString &iName, Vca::U32 iBusynessOverride) {
	    unsigned int xIndex;
	    if(m_iBusynessOverrides.Insert(iName.content(), xIndex)) {
		m_iBusynessOverrides[xIndex] = iBusynessOverride;
	    }
	}
		
    //  State
    private:
	VString		m_iPoolInfo;
	Vca::U32	m_iWorkerMaximum;
	Vca::U32	m_iWorkerMinimum;
	Vca::U32	m_iWorkersBeingCreatedMaximum;
	Vca::U64	m_iEvaluationTimeout;
	Vca::U32	m_cEvaluationAttemptMaximum;
	Vca::U32	m_cEvaluationOnErrorAttemptMaximum;
	Vca::U32	m_cEvaluationTimeOutAttemptMaximum;
	Vca::U32	m_iWorkerMinimumAvailable;
	Vca::U32	m_iWorkerMaximumAvailable;
	Vca::U32	m_iWorkerCreationFailureHardLimit;
	Vca::U32	m_iWorkerCreationFailureSoftLimit;
	Vca::U32	m_iShrinkTimeOutMins;
	Vca::U32	m_iStopTimeOutMins;
	Vca::U32	m_iBusynessThreshold;
	Vca::U32	m_iMaxBusynessChecks;
	BusynessOverrideMap m_iBusynessOverrides;
    
	Vca::VDirectory::Reference m_pDirectory;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VsaDirectoryBuilder::ThisOrder::SpecialPoolSession::SpecialPoolSession (
   VString iName, SessionType iType
) : BaseClass (iName, iType)
, m_iWorkerMaximum		(100)
, m_iWorkerMinimum		(1)
, m_iWorkerMinimumAvailable     (m_iWorkerMinimum - 1)
, m_iWorkerMaximumAvailable     (m_iWorkerMinimum)
, m_iWorkersBeingCreatedMaximum (1)
, m_iEvaluationTimeout          (900) 
, m_cEvaluationAttemptMaximum   (0)
, m_cEvaluationOnErrorAttemptMaximum	(5)
, m_cEvaluationTimeOutAttemptMaximum    (0)
, m_iWorkerCreationFailureHardLimit (11)
, m_iWorkerCreationFailureSoftLimit (5)
, m_iBusynessThreshold		(3)
, m_iShrinkTimeOutMins	        (0)
, m_iStopTimeOutMins	        (0) 
, m_iMaxBusynessChecks		(UINT_MAX) {
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
  VSmartEvaluatorSource::Reference const pSource (new VSmartEvaluatorSource (pDirectory, m_iBusynessThreshold, m_iMaxBusynessChecks));
  pSource->setBusynessMap(m_iBusynessOverrides);
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
  pSettings->setEvaluationOnErrorAttemptMaximumTo (m_cEvaluationOnErrorAttemptMaximum);
  pSettings->setEvaluationTimeOutAttemptMaximumTo (m_cEvaluationTimeOutAttemptMaximum);
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
		    : 0 == strcasecmp (pToken, "Notification_Template")
		    ? ParseState_ExpectingNotificationSentinel
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
		    if (BaseClass::parseSessionTemplate (iSessionsFile, xType, pSession)) 
			BaseClass::insertSession (pSession);
		    xState = ParseState_ExpectingTag;
		}
		break;
	    case ParseState_ExpectingNotificationSentinel:
		if (strcmp (pToken, "Begin") != 0) {
		    log ("VsaDirectoryBuilder::ThisOrder::parseSessionsFile --> Error: found '%s' when expecting 'Begin' for Notification_Template", pToken);
		    xState = ParseState_Error;
		} else {
		    VTransient::transientServicesProvider ()->parseNotificationTemplate (iSessionsFile);
		    xState = ParseState_ExpectingTag; 
		}
		break;
	    case ParseState_ExpectingPoolSessionSentinel:
		if (strcmp (pToken, "Begin") != 0) {
		    log ("VsaDirectoryBuilder::ThisOrder::parseSessionsFile --> Error: found '%s' when expecting 'Begin' for PoolConnection_Template", pToken);
		    xState = ParseState_Error;
		} else {
		    Session::Reference pSession;
		    if (parsePoolSessionTemplate (iSessionsFile, xType, pSession)) {
			// Here supplyObjectSource_() is called and VSmartEvaluatorSource is newed
			BaseClass::insertSession (pSession);
		    }
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
	    case ParseState_ExpectingNotificationSentinel:
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

    bool bCompleted = false, bOnErrorAttemptsSet = false, bOnTimeoutAttemptsSet = false;

    Vca::U32 iMaxWorkers (100), iMinWorkers (1), iMinAvail (iMinWorkers - 1), iMaxAvail (iMinWorkers);
    Vca::U32 iEvalAttempts (0) , iEvalOnErrorAttempts (5) , iEvalTimeOutAttempts (0), iWorkerCreationFailureHardLmt (30), iWorkerCreationFailureSoftLmt (1);
    Vca::U32 iWorkersBeingCreated (UINT_MAX), iStopTimeOut (0), iShrinkTimeOut (0);
    Vca::U64 iEvaluationTimeout (900);
    Vca::U32 iBusynessThreshold(3);
    Vca::U32 iMaxBusynessChecks (UINT_MAX);

    VString iSpecialPoolName;
	SpecialPoolSession::Reference pSPSession;

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
	    : 0 == strcasecmp (pToken, "EvaluationOnErrorAttempts")
	    ? ParseState_ExpectingEvaluationOnErrorAttempts
	    : 0 == strcasecmp (pToken, "EvaluationTimeOutAttempts")
	    ? ParseState_ExpectingEvaluationTimeOutAttempts
	    : 0 == strcasecmp (pToken, "WorkerCreationFailureHardLimit")
	    ? ParseState_ExpectingWorkerCreationFailureHardLmt
	    : 0 == strcasecmp (pToken, "WorkerCreationFailureSoftLimit")
	    ? ParseState_ExpectingWorkerCreationFailureSoftLmt
	    : 0 == strcasecmp (pToken, "ShrinkTimeOut")
	    ? ParseState_ExpectingShrinkTimeOut
	    : 0 == strcasecmp (pToken, "StopTimeOut")
	    ? ParseState_ExpectingStopTimeOut
	    : 0 == strcasecmp (pToken, "BusynessThreshold")
	    ? ParseState_ExpectingBusynessThreshold 
	    : 0 == strcasecmp (pToken, "MaxBusynessChecks")
	    ? ParseState_ExpectingMaxBusynessChecks
	    : ParseState_Error;  
	  if (ParseState_Error == xState)
	      log ("VsaDirectoryBuilder::ThisOrder::parsePoolSessionTemplate --> Unknown tag '%s'", pToken);
	  break;
	case ParseState_ExpectingSessionSentinel:
	  if (strcmp (pToken, "Begin") == 0) {
	      if(!pSPSession) {
		  pSPSession.setTo (new SpecialPoolSession (iSpecialPoolName, xType));
	      }
	      VString templateString;
	      if(extractSessionTemplateString(rSessionsFile, templateString)) {
		  Vca::U32 busynessOverride;
		  bool useBusynessOverride = getBusynessFromTemplate(templateString, busynessOverride);
			
		  Session::Reference pSession;
		  bool bResult = BaseClass::parseSessionTemplate (templateString, xType, pSession);
		  if (bResult) {
		      pNewOrder->insertSession (pSession);
		      if(useBusynessOverride) {
			  pSPSession->addBusynessMapEntry(pSession->getName(), busynessOverride);
		      }
		  }
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
	  // if the config file is setting this older parameter
          iEvalAttempts = atoi (pToken);
	  // and the config file hasn't explicitly set the newer
	  // parameters, set the new parameters to this value.
	  if (!bOnErrorAttemptsSet)
	      iEvalOnErrorAttempts = iEvalAttempts;
	  if (!bOnTimeoutAttemptsSet)
	      iEvalTimeOutAttempts = iEvalAttempts;
          break;
        case ParseState_ExpectingEvaluationOnErrorAttempts:
	  bOnErrorAttemptsSet = true;
          iEvalOnErrorAttempts = atoi (pToken);
          break;
	case ParseState_ExpectingEvaluationTimeOutAttempts:
	  bOnTimeoutAttemptsSet = true;
          iEvalTimeOutAttempts = atoi (pToken);
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
	case ParseState_ExpectingBusynessThreshold:
	    iBusynessThreshold = atoi (pToken);
	    break;
	case ParseState_ExpectingMaxBusynessChecks:
	    iMaxBusynessChecks = atoi (pToken);
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
      iPoolDesc << "\r\nEVALUATION ATTEMPTS ON ERROR: " << iEvalOnErrorAttempts;
      iPoolDesc << "\r\nEVALUATION ATTEMPTS Time Out: " << iEvalTimeOutAttempts;
      iPoolDesc << "\r\nWORKER CREATION FAILURE SOFT LMT: " << iWorkerCreationFailureSoftLmt;
      iPoolDesc << "\r\nWORKER CREATION FAILURE HARD LMT: " << iWorkerCreationFailureHardLmt;
      iPoolDesc << "\r\nSHRINK TIMEOUT (mins): " << iShrinkTimeOut;
      iPoolDesc << "\r\nSTOP TIMEOUT (mins): " << iStopTimeOut << "\r\n";
      iPoolDesc << "\r\nPOOL_END: " << iSpecialPoolName << "\r\n";

      pSPSession->setSourceDirectory(pNewOrder->m_pDirectory);
      pSPSession->setPoolInfo (iPoolDesc);
      pSPSession->setWorkerMax (iMaxWorkers);
      pSPSession->setWorkerMin (iMinWorkers);
      pSPSession->setWorkerMinimumAval (iMinAvail);
      pSPSession->setWorkerMaximumAvail (iMaxAvail);
      pSPSession->setWorkersBeingCreated (iWorkersBeingCreated);
      pSPSession->setEvaluationTimeout (iEvaluationTimeoutMicroSecs);
      pSPSession->setEvaluationAttemptMax (iEvalAttempts);
      pSPSession->setEvaluationOnErrorAttemptMax (iEvalOnErrorAttempts);
      pSPSession->setEvaluationTimeOutAttemptMax (iEvalTimeOutAttempts);
      pSPSession->setWorkerCreationFailureHardLimit (iWorkerCreationFailureHardLmt);
      pSPSession->setWorkerCreationFailureSoftLimit (iWorkerCreationFailureSoftLmt);
      pSPSession->setShrinkTimeOut (iShrinkTimeOut);
      pSPSession->setStopTimeOut (iStopTimeOut);
      pSPSession->setBusynessThreshold (iBusynessThreshold);
      pSPSession->setMaxBusynessChecks (iMaxBusynessChecks);
      rpSession.setTo (pSPSession);
    }

    return bCompleted;
}

bool Vsa::VsaDirectoryBuilder::ThisOrder::getBusynessFromTemplate (VString &rSessionsString, Vca::U32 &rBusyness) const {
    VString pre, post;
    if(rSessionsString.getPrefix("BusynessOverride ",pre, post)) {
	VString newPre, newPost;
	post.getPrefix("\n", newPre, newPost);
	Vca::U32 u32;
	if (1 == sscanf (newPre, "%u", &u32))  {
	    rBusyness = u32;
	    return true;
	}
    }
    return false;
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
