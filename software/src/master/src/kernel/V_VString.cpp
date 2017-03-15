/*****  VString Implementation  *****/

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

#include "V_VString.h" 

/************************
 *****  Supporting  *****
 ************************/

#include "V_VArgList.h"

#include "VReferenceable.h"


/*********************
 *********************
 *****           *****
 *****  VString  *****
 *****           *****
 *********************
 *********************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

char VString::g_iEmptyBuffer[] = "";


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VString::VString (char const *pString, bool bCopy) : m_iStorage (
    pString ? const_cast<char*>(pString) : g_iEmptyBuffer,
    pString ? strlen (pString) + 1 : 1, pString && bCopy
) {
}

VString::VString () : m_iStorage (g_iEmptyBuffer, 1, false) {
}

VString::VString (size_t sString) : m_iStorage (g_iEmptyBuffer, 1, false) {
    guarantee (sString);
}


/**************************
 **************************
 *****  Access/Query  *****
 **************************
 **************************/

unsigned int VString::countOf (char const *pString, char iCharacter) {
    unsigned int nOccurrences = 0;

    while (char const *pNextOccurrence = strchr (pString, iCharacter)) {
	pString = pNextOccurrence + 1;
	nOccurrences++;
    }

    return nOccurrences;
}

unsigned int VString::countOf (char const *pString, char const *pSubstring) {
    unsigned int nOccurrences = 0;

    while (char const *pNextOccurrence = strstr (pString, pSubstring)) {
	pString = pNextOccurrence + 1;
	nOccurrences++;
    }

    return nOccurrences;
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

VString& VString::append (char const *pString, size_t sString) {
    if (pString) {
	size_t sOld = length ();
	size_t sNew = sOld + sString;
	pointer_t pStorage = storage (sNew);
	strncpy (pStorage + sOld, pString, sString);
	pStorage[sNew] = '\0';
    }
    return *this;
}

void VString::setTo (char const *pString, size_t sString) {
    if (pString && sString) {
	pointer_t pStorage = storage (sString);
	strncpy (pStorage, pString, sString);
	pStorage[sString] = '\0';
    }
    else if (m_iStorage.storageIsPrivate ())
	storage()[0] = '\0';
    else
	clear ();
}

void VString::setToMoved (VString &rOther) {
    if (this != &rOther) {
	setTo (rOther);
	rOther.clear ();
    }
}

void VString::setToStatic (char const *pString, size_t sString) {
    clear ();
    if (pString)
	m_iStorage.setTo (const_cast<char*>(pString), sString + 1);
}

void VString::setToStatic (char const *pString) {
    clear ();
    if (pString)
	m_iStorage.setTo (const_cast<char*>(pString), strlen (pString) + 1);
}


VString& VString::operator<< (char iValue) {
    char iWorkBuffer[2];
    sprintf (iWorkBuffer, "%c", iValue);
    return append (iWorkBuffer);
}

VString& VString::operator<< (unsigned char iValue) {
    char iWorkBuffer[2];
    sprintf (iWorkBuffer, "%c", iValue);
    return append (iWorkBuffer);
}

VString& VString::operator<< (double iValue) {
    char iWorkBuffer[32];
    sprintf (iWorkBuffer, "%g", iValue);
    return append (iWorkBuffer);
}

VString& VString::operator<< (int iValue) {
    char iWorkBuffer[32];
    sprintf (iWorkBuffer, "%d", iValue);
    return append (iWorkBuffer);
}

VString& VString::operator<< (unsigned int iValue) {
    char iWorkBuffer[32];
    sprintf (iWorkBuffer, "%u", iValue);
    return append (iWorkBuffer);
}

VString& VString::operator<< (__int64 iValue) {
    char iWorkBuffer[32];
    sprintf (iWorkBuffer, "%d", (int)iValue);
    return append (iWorkBuffer);
}

VString& VString::operator<< (unsigned __int64 iValue) {
    char iWorkBuffer[32];
    sprintf (iWorkBuffer, "%llu", iValue);
    return append (iWorkBuffer);
}

VString& VString::printf (char const *pFormat, ...) {
    V_VARGLIST (ap, pFormat);
    return vprintf (pFormat, ap);
}

VString& VString::vprintf (char const *pFormat, va_list ap) {

    FILE *pSink;
#if defined (_WIN32)
    pSink = fopen ("NUL:", "a");
#else
    pSink = fopen ("/dev/null", "a");
#endif

    unsigned int size = 2047; // guess
    if (pSink) {
	V::VArgList iArgList (ap);
	size = vfprintf (pSink, pFormat, iArgList.list ());
	fclose (pSink);
    }

    char *iWorkBuffer = (char*) allocate (size+1);

    V::VArgList iArgList (ap);
    vsprintf (iWorkBuffer, pFormat, iArgList);
    *this << iWorkBuffer;
    deallocate (iWorkBuffer);
    return *this;
}

VString& VString::vsafeprintf (char const *pFormat, va_list ap) {

    FILE *pSink;
#if defined (_WIN32)
    pSink = fopen ("NUL:", "a");
#else
    pSink = fopen ("/dev/null", "a");
#endif

    // There is no buffer size guess work involved in this safe print 
    if (pSink) {
	unsigned int size;
	{
	    V::VArgList iArgList (ap);
	    size = vfprintf (pSink, pFormat, iArgList.list ());
	    fclose (pSink);
	}

	char *iWorkBuffer = (char*) allocate (size+1);

	V::VArgList iArgList (ap);
	vsprintf (iWorkBuffer, pFormat, iArgList);
	*this << iWorkBuffer;
	deallocate (iWorkBuffer);
    } else {
	// Hope it will never reach here, but in case it does happen
	// at least it can give us some clues what it is trying to print.
	*this << "The null device cannot be opened so a complete formatted print is not possible,"
	      << "but here is the partial print : " << pFormat;
    }
    return *this;
}


/****************************
 *****  VString::Quote  *****
 ****************************/

VString& VString::quote (char const *pString) {
    static char const *const pSpecials = "\\\"";
    size_t sGuarantee = length () + 2;
    if (pString) {
	sGuarantee += strlen (pString);
	char const *pNextSpecial = pString + strcspn (pString, pSpecials);
	while (*pNextSpecial) {
	    sGuarantee++;
	    pNextSpecial++;
	    pNextSpecial += strcspn (pNextSpecial, pSpecials);
	}
    }

    pointer_t pStorage = storage (sGuarantee);
    strcat (pStorage, "\"");
    if (pString) {
	char const *pLastSpecial = pString;
	char const *pNextSpecial = pString + strcspn (pString, pSpecials);
	while (*pNextSpecial) {
	    strncat (
		pStorage, pLastSpecial, (size_t)(pNextSpecial - pLastSpecial)
	    );
	    strcat (pStorage, "\\");
	    pLastSpecial = pNextSpecial;
	    pNextSpecial++;
	    pNextSpecial += strcspn (pNextSpecial, pSpecials);
	}
	strcat (pStorage, pLastSpecial);
    }
    strcat (pStorage, "\"");

    return *this;
}

/*********************
 *********************
 *****  Utility  *****
 *********************
 *********************/


 /***************************************************************************
  * Routine: getPrefix
  * getPrefix routines (both char/char* delimiter versions) return the prefix 
  * before the delimiter and the suffix after the delimiter, via references
  * If the delimiter is not present they return the entire string.
  ***************************************************************************/

bool VString::getPrefix (char iDelimiter, VString &rPrefix, VString &rRemainingString) const {
  if (length () == 0)
    return false;

  char const *pStorage = storage ();
  char const *pLoc     = strchr (pStorage, iDelimiter);
  
  if (pLoc != NULL) {
    rPrefix.setTo (pStorage, pLoc-pStorage);
    rRemainingString.setTo (pLoc+1);  
    return true;
  }

  rPrefix.setTo (*this);
  rRemainingString.clear ();
  return false;
}

bool VString::getPrefix (char const *pDelimiter, VString &rPrefix, VString &rRemainingString) const {
    if (length () == 0)
	return false;

    char const *pStorage   = storage ();
    char const *pLoc       = strstr (pStorage, pDelimiter);

    if (pLoc != NULL) {
      rPrefix.setTo (pStorage, pLoc-pStorage);
      rRemainingString.setTo (pLoc+strlen(pDelimiter));
      return true;
    }

    rPrefix.setTo (*this);
    rRemainingString.clear ();
    return false;
}

/**
 * Summarizes strings in the following form: [prefix]...[suffix]
 * [suffix] length is determined automatically to be proportionate to cTotalSize. [prefix] length is determined automatically to fill the remainder of the string.
 * Tabs, newlines and carriage returns are converted to a single space if cTotalSize is less than 80. In this case, all whitespace at the beginning of the string will be automatically trimmed as well.
 * Whitespace at the end of the string is trimmed automatically regardless of size.
 * If cTotalSize is altogether too small, a simple '...' is returned.
 */
void VString::getSummary(VString &rSummary, unsigned int cTotalSize) const {
    const bool bShortString = cTotalSize <= 80;
    const unsigned int cNumDots = 3;
    unsigned int cTailSize = cTotalSize / 10;
    if (cTailSize < 5) cTailSize = 5;
    size_t cOrigLength = length ();
    const char* pOrigString = content ();

    // Catch boundary conditions.
    if (cTotalSize < cNumDots) {
        rSummary = "...";
        return;
    }

    // Detect trimming conditions as necessary.
    unsigned int xStart = 0;
    if (bShortString) {
        for (; xStart <= cTotalSize && (pOrigString[xStart] == '\n' || pOrigString[xStart] == '\r' || pOrigString[xStart] == '\t' || pOrigString[xStart] == ' '); xStart++);
    }
    for (; cOrigLength > xStart && (pOrigString[cOrigLength - 1] == '\n' || pOrigString[cOrigLength - 1] == '\r' || pOrigString[cOrigLength - 1] == '\t' || pOrigString[cOrigLength - 1] == ' '); cOrigLength--);

    if (cOrigLength < cTotalSize) {
        // The whole string can fit in our shortened length.
        rSummary.setTo (pOrigString + xStart, cOrigLength);
        cTotalSize = cOrigLength;
    } else {
        // The whole string won't fit. We need to shorten.
        if (cTotalSize <= cTailSize + cNumDots) {
            // We can't fit the whole tail and the dots; shorten the tail.
            cTailSize = cTotalSize - cNumDots;
        }
        rSummary.setTo (pOrigString + xStart, cTotalSize - cTailSize - cNumDots);
        for (int i = 0; i < cNumDots; i++) rSummary.append (".");
        rSummary.append (pOrigString + xStart + cOrigLength - cTailSize, cTailSize);
    }

    // Replace whitespace characters with a space.
    char* pSummary = rSummary.storage();
    if (bShortString) for (unsigned int i = 0; i <= cTotalSize; i++) switch (pSummary[i]) {
    case '\n':
    case '\r':
    case '\t':
        pSummary[i] = ' ';
        break;
    }
}

 /***************************************************************************
  * Routine: int index (char iChar, unsigned int cOccurence)
  * index method returns the index of the cOccurence'th occurence of the iChar
  * in the string. If it is not present, then a -1 is returned.
  ***************************************************************************/

int VString::index (char iChar, unsigned int cOccurence) const {
  int index = -1;
  char const *pStorage = storage ();
  char const *pNextOccurence = 0;
  while (cOccurence > 0 &&
	 pStorage &&
	 (pNextOccurence = strchr (pStorage, iChar))) {
    cOccurence--;
    if (cOccurence==0) 
      return pNextOccurence - storage ();

    pStorage = pNextOccurence + 1;
  }
  return index;
}

/**************************
 **************************
 *****  VStringIndex  *****
 **************************
 **************************/

class VStringIndex : public VReferenceable {
    DECLARE_CONCRETE_RTT (VStringIndex, VReferenceable);

//  Friend
    friend class VString;

//  Construction
public:
    VStringIndex (unsigned int xIndex) : m_xIndex (xIndex) {
    }

//  Destruction
private:
    ~VStringIndex () {
    }
	
//  Access
public:
    unsigned int getIndex () const {
	return m_xIndex;
    }

//  State
private:
    VReference<ThisClass>	m_pNext;
    VReference<ThisClass>       m_pPrev;
    unsigned int		m_xIndex;
};

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VStringIndex);


/*********************
 *********************
 *****  Replace  *****
 *********************
 *********************/

/***************************************************************************
  * Routine: substringReplace
  * This method replaces substrings present with a replacement
  * string. This is an in-place substring replacement method.
  **************************************************************************/

void VString::replaceSubstring (char const *pSubStr, char const *pReplacement) {

    // if substring or the replacement string is 0 then return
    if (!pReplacement || !pSubStr) 
	return;

    unsigned int nCount = 0, xIndex = 0, sLength, sSubStr, sReplacement;
    sLength      = length ();
    sSubStr      = strlen (pSubStr);
    sReplacement = strlen (pReplacement);

    //  count and store the matching substring indices... 

    VReference<VStringIndex> pHead, pTail;
    char const *pSubStrPtr=0; char const *pStorage=0;
    for (pStorage = content ();
	 *pStorage && (pSubStrPtr = strstr (pStorage, pSubStr)); 
	 nCount++
    ) {
      xIndex = pSubStrPtr - content ();
      
      VReference<VStringIndex> pEntry (new VStringIndex (xIndex));
      if (pTail) {
	pEntry->m_pPrev.setTo (pTail);
	pTail->m_pNext.setTo (pEntry);
      }
      else {
	pHead.setTo (pEntry);
      }
      pTail.setTo (pEntry);
      pStorage = pSubStrPtr + sSubStr;
    }
    
    //  search and replace 
    //  case 1: new string is bigger...move sub strings from back and copy new replacement string
    //  case 2: new string is shorter..move sub strings from front and copy new replacement string 

    if (nCount > 0) {  
	char *pPtr = storage ();

	//  case 1
	if (sReplacement >= sSubStr) {
 	  unsigned int sNewMemSize = sLength + 1+ (nCount * (sReplacement-sSubStr));

	  //  guarantee memory if new string is longer.....
	  pPtr = (sNewMemSize > sLength+1) ? storage (sNewMemSize) : pPtr;
	  
	  //  traverse the list from back 
	  VReference<VStringIndex> pEntry (pTail);
	  int xPrevIndex = -1;
	  char *pPrevTarget = pPtr+sNewMemSize-1;

	  while (pEntry) {
		xIndex = pEntry->getIndex ();

    		//  compute source pointer and its length
		char const *pSource = pPtr + xIndex+ sSubStr;
		unsigned int sSourceLength = (xPrevIndex==-1) 
	                               ? strlen (pSource)  
	                               : (xPrevIndex-xIndex-sSubStr);

		//  compute target pointer
		char *pTarget = pPrevTarget - sSourceLength;
	    
		//  move the string from source pointer to target pointer
		memmove (pTarget, pSource, sSourceLength);

		//  copy the replacement string
		strncpy (pTarget-sReplacement, pReplacement, sReplacement);

		//  set values for next iteration
		pEntry.setTo (pEntry->m_pPrev);
		xPrevIndex = xIndex;
		pPrevTarget = pTarget-sReplacement;
	    }
	    pPtr [sNewMemSize-1]='\0';
	}

	//  case 2
	else {

	    VReference <VStringIndex> pEntry (pHead);
  	    int xNextIndex =-1;
	    char *pNextTarget = pEntry ? pPtr+(pEntry->getIndex ()+sReplacement):0;

	    while (pEntry) {

		//  compute current index and next values
		xIndex = pEntry->getIndex ();

		if (pEntry->m_pNext)
    		    xNextIndex = pEntry->m_pNext->getIndex ();
		else
    		    xNextIndex = -1;
	      
		// compute source pointer and its length 
		char const *pSource = pPtr+xIndex+sSubStr; 
		unsigned int sSourceLength = (xNextIndex==-1) 
		                         ? strlen (pSource)
		                         : (xNextIndex-xIndex-sSubStr);
	      
		//  compute the target pointer
		char *pTarget = pNextTarget;
 
		//  move the string from source pointer to target pointer
		memmove (pTarget, pSource, sSourceLength);
	      
		//  copy the replacement string
		strncpy (pTarget-sReplacement, pReplacement, sReplacement);
	      
		//  set values for next iteration
		pEntry.setTo (pEntry->m_pNext);
		pNextTarget = pTarget+sReplacement+sSourceLength;
	    }
	    pPtr[sLength - (nCount* (sSubStr-sReplacement))]='\0';
	} 
    }
}


/***************************************************************************
  * Routine: substringReplace
  * This method replaces substrings present this string with a replacement
  * string and places the new output into the result string.
  * NOTE: This is not an in-place substring replacement method. 
  *       Output is written to a completely new string. 
 ***************************************************************************/

void VString::replaceSubstring (
    char const *pSubstring, char const *pRepstring, VString &rResult
) const {
    // if the substring is "" then return as it is
    if (strcmp (pSubstring, "") == 0) {
	rResult.setTo (*this);
	return;
    }

    // Make a first pass to see how big our resulting string will be.
    char const *pStorage = content ();
    size_t sSource = length ();
    size_t sResultOrig = rResult.length ();
    size_t sSubstring = strlen (pSubstring); // Size of our needle.
    size_t sRepstring = strlen (pRepstring); // Size of our replacement.
    size_t repIncrement = sRepstring - sSubstring; // Amount of extra room we need per replacement.
    size_t sResultNew = length () + sResultOrig; // Not including null character.
    for (pStorage = strstr (pStorage, pSubstring); pStorage; pStorage += sSubstring) {
      sResultNew += repIncrement;
    }
    pStorage = content (); // Reset pStorage.

    // Exit right here if we didn't find any occurrences.
    if (sResultNew == sResultOrig) return;

    // Make a second pass to actually build the resulting string.
    char *pResult = rResult.guarantee (sResultNew + 1) + sResultOrig; // Points to our current position in the result.
    const char *occurrence = strstr ( pStorage, pSubstring); // Points to our current position in the source.
    while (occurrence) {
      // Copy.
      memcpy (pResult, pStorage, occurrence - pStorage); // Copy original string up to needle.
      pResult += occurrence - pStorage;
      memcpy (pResult, pRepstring, sRepstring);
      pResult += sRepstring;

      // Increment.
      pStorage = occurrence + sSubstring; // Skip over the occurrence of our needle in the source.
      occurrence = strstr (pStorage, pSubstring);
    }
    // Done finding occurrences. Copy the remainder of the source string now.
    memcpy (pResult, pStorage, sSource - (pStorage - content ()) + 1); // include the null character.
}

/**
 * Replaces CR with CRLF and stores the result in dest, obliterating anything that was already there.
 */
void VString::convertLFtoCRLF (VString &dest) const {
  int sSource = length (), sDest = 0;
  const char *pSource = content ();
  
  // Make a first pass to find out how big we need our resultant string to be.
  for (int i = 0; i < sSource; i++, sDest++) {
    if (pSource[i] == '\n') sDest++;
  }

  // Make a second pass to actually build the destination string.
  char *pDest = dest.guarantee (sDest + 1); // Don't forget the null character.
  for (int i = 0, j = 0; i <= sSource; i++, j++) {
    if (pSource[i] == '\n') pDest[j++] = '\r';
    pDest[j] = pSource[i];
  }
}
