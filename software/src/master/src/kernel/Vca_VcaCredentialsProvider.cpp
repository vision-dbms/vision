/*****  Vca_VcaCredentialsProvider Implementation *****/

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

#include "Vca_VcaCredentialsProvider.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#if defined(_WIN32)		// extra strength compiler happy pill needed here
#include "Vca_VDeviceFactory.h"	// omit:Linux omit:SunOS
#endif


/*****************************************************
 *****************************************************
 *****                                           *****
 *****  Vca::VcaCredentialsProvider::Credentials *****
 *****                                           *****
 *****************************************************
 *****************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaCredentialsProvider::Credentials::Credentials (
   VString const &rHost, VString const &rLogin, VString const &rPassword
) : m_iHost (rHost), m_iLogin (rLogin), m_iPassword (rPassword) {
}



/*****************************************
 *****************************************
 *****                               *****
 *****  Vca::VcaCredentialsProvider  *****
 *****                               *****
 *****************************************
 *****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VcaCredentialsProvider::VcaCredentialsProvider () {
  init ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VcaCredentialsProvider::~VcaCredentialsProvider () {
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

/*****
 * Method: init ()
 *   Builds Credentials list out of the following files 
 *   1. Global credentials file pointed by VcaGlobalCredentialsFile env variable
 *   2. Local credentials file pointed by VcaCredentialsFile env variable.
 *   3. $HOME/credentials.txt 
 *   4. ./credentials.txt
 *****/

void Vca::VcaCredentialsProvider::init () {

    //  Global Credentials File
    parseCredentialsFile (getenv ("VcaGlobalCredentialsFile"));

    //  Local Credentials File
    parseCredentialsFile (getenv ("VcaCredentialsFile"));

    //  $HOME/credentials.txt 
    struct stat iHomeStats;
    char const *pFilePath=0;
    bool bHomeStats = false;
    pFilePath = getenv ("HOME");
    if (pFilePath) {
        VString iFilePath (pFilePath, false);
#ifdef __VMS
        iFilePath << "credentials.txt";
#else
        iFilePath << "/credentials.txt";
#endif
        bHomeStats = stat (iFilePath, &iHomeStats) == 0;
        if (bHomeStats)
	    parseCredentialsFile (iFilePath);
    }

    // Process ./credentials.txt (only once if same as $HOME/credentials.txt)
    pFilePath = "credentials.txt";
    struct stat iCurrentStats;
    bool bProcessingCurrentFile = !bHomeStats || stat (pFilePath, &iCurrentStats) == 0 && (
	    iHomeStats.st_dev != iCurrentStats.st_dev || iHomeStats.st_ino != iCurrentStats.st_ino
    );
    if (bProcessingCurrentFile)
        parseCredentialsFile (pFilePath);
}


/*****
 * Method: parseCredentialsFile
 *   Parses the credentials file and appends a credential entries to the linked
 *   list of credentials to be used for building the directory under construction.
 *****/

void Vca::VcaCredentialsProvider::parseCredentialsFile (char const *pFileName) {
  
  if (!pFileName)
    return;
  
  FILE *pFile = fopen (pFileName, "r");
  if (!pFile) 
    return;

  VString iHostName, iUserName, iPassword;

  if (pFile) {
    char iBuffer [4096];
    while (fgets (iBuffer, sizeof (iBuffer), pFile)) {
      char const* const pWhitespaceBreakSet = " \t\r\n";
      char const* const pRestOfLineBreakSet = "\r\n";
      enum {
	ParseState_Error,
	ParseState_ExpectingTag,
	ParseState_ExpectingPasswordTag,
	ParseState_ExpectingHostName,
	ParseState_ExpectingUserName,
	ParseState_ExpectingPassword
      } xState = ParseState_ExpectingTag;
      
      char const *pBreakSet = pWhitespaceBreakSet;
      for (
	   char *pToken = strtok (iBuffer, pBreakSet);
	   pToken && ParseState_Error != xState;
	   pToken = strtok (0, pBreakSet)
      ) {
	switch (xState) {
	case ParseState_ExpectingTag:
	  xState = 0 == strcasecmp (pToken, "Credentials")
	    ? ParseState_ExpectingPasswordTag
	    : 0 == strcasecmp (pToken, "HostName")
	    ? ParseState_ExpectingHostName
            : 0 == strcasecmp (pToken, "UserName")
            ? ParseState_ExpectingUserName
            : 0 == strcasecmp (pToken, "Password")
            ? ParseState_ExpectingPassword
	    : ParseState_Error;  
	  break;
	case ParseState_ExpectingPasswordTag:
	  if (strcmp (pToken, "Begin") == 0) {
	      iHostName.clear ();
	      iUserName.clear ();
	      iPassword.clear ();
	  }
	  else if (strcmp (pToken,"End") == 0) { 
            VReference<Credentials> pCredentials (new Credentials (iHostName, iUserName, iPassword));
	    appendCredentials (pCredentials);
          }
 	  break;
	case ParseState_ExpectingHostName:
	  iHostName.setTo (pToken);
	  break;
	case ParseState_ExpectingUserName:
	  iUserName.setTo (pToken);
	  break;
	case ParseState_ExpectingPassword:
	  iPassword.setTo (pToken);
	  break;
	default:
	  xState = ParseState_Error;
	  break;
	}
        switch (xState) {
	   case ParseState_ExpectingHostName:
	   case ParseState_ExpectingUserName:
	   case ParseState_ExpectingPassword:
	      pBreakSet = pRestOfLineBreakSet;
	      break;
	   default:
	      pBreakSet = pWhitespaceBreakSet;
	      break;
        }
      }
    }
    fclose (pFile);
  }
}

void Vca::VcaCredentialsProvider::appendCredentials (Credentials *pCredentials) {
  if (pCredentials) {
    pCredentials->m_pSuccessor.setTo (m_pCredentialsList);
    m_pCredentialsList.setTo (pCredentials);
  }
}

bool Vca::VcaCredentialsProvider::getPassword (
  VString const &rHostName, VString const &rUserName, VString &rPassword
) const {

  VReference<Credentials> pCredentials (m_pCredentialsList);

  while (pCredentials.isntNil ()) {
    // get credentials 
    VString iHost, iLogin, iPassword;
    pCredentials->getHost(iHost);
    pCredentials->getLogin (iLogin);
    
    if (strcasecmp (rHostName.content (), iHost.content ()) == 0 && 
	strcasecmp (rUserName.content (), iLogin.content ()) == 0) {
      pCredentials->getPassword (rPassword);
      return true;
    }
    pCredentials.setTo (pCredentials->m_pSuccessor);
  }
  return false;
}

