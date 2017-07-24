#ifndef V_VString_Interface
#define V_VString_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"
#include "V_VCOS.h"

/*************************
 *****  Definitions  *****
 *************************/

class V_API VString : public VTransient {
//  Family
    DECLARE_FAMILY_MEMBERS (VString, VTransient);

//  Globals
private:
    static char g_iEmptyBuffer[];

//  Construction
public:
    VString (VString const& rString) : m_iStorage (rString.m_iStorage) {
    }
    
    VString (char const *pString, bool copyRequired = true);

    explicit VString (size_t sString);

    VString ();

//  Destruction
public:
    ~VString () {
    }

//  Access
public:
    char const *content () const {
	return storage ();
    }
    size_t length () const {
	return strlen (*this);
    }

    pointer_t storage (size_t sString) {
	return m_iStorage.storage (sString + 1);
    }
    pointer_t storage () {
	return m_iStorage.storage ();
    }
    char const *storage () const {
	return m_iStorage.storage ();
    }

    char const *storageLimit () {
	return m_iStorage.storageLimit ();
    }
    char const *storageLimit () const {
	return m_iStorage.storageLimit ();
    }

    size_t storageSize () const {
	return m_iStorage.storageSize ();
    }

    operator char const*() const {
	return storage ();
    }
    pointer_t guarantee (size_t sSize) {
	return m_iStorage.storage (sSize);
    }

//  Query
public:
    bool isEmpty () const {
	return storage () == g_iEmptyBuffer;
    }
    bool isntEmpty () const {
	return storage () != g_iEmptyBuffer;
    }
    bool operator<  (char const *pOther) const {
	return strcmp (*this, pOther) <  0;
    }
    bool operator<= (char const *pOther) const {
	return strcmp (*this, pOther) <= 0;
    }
    bool operator== (char const *pOther) const {
	return strcmp (*this, pOther) == 0;
    }
    bool operator!= (char const *pOther) const {
	return strcmp (*this, pOther) != 0;
    }
    bool operator>= (char const *pOther) const {
	return strcmp (*this, pOther) >= 0;
    }
    bool operator>  (char const *pOther) const {
	return strcmp (*this, pOther) >  0;
    }
    bool equals (VString const &rOther) const {
        return *this == rOther;
    }
    bool equalsIgnoreCase (VString const &rOther) const {
      return strcasecmp (*this, rOther) == 0;
    }
    bool equalsIgnoreCase (char const* pOther) const {
      return strcasecmp (*this, pOther) == 0;
    }

    static unsigned int countOf (char const *pString, char iCharacter);
    static unsigned int countOf (char const *pString, char const *pSubstring);

    static unsigned int lineCount (char const *pString) {
	return countOf (pString, '\n');
    }
    unsigned int lineCount () const {
	return lineCount (*this);
    }

//  Update
public:
    void clear () {
	m_iStorage.setTo (g_iEmptyBuffer, 1);
    }

    void trim () {
	m_iStorage.trim (length () + 1);
    }

    VString& append (char const *pString, size_t sString);
    VString& append (char const *pString) {
	return append (pString, strlen (pString));
    }

    VString& operator<< (char const *pString) {
	return append (pString);
    }

    VString& operator<< (char iValue);
    VString& operator<< (unsigned char iValue);
    VString& operator<< (double iValue);
    VString& operator<< (int iValue);
    VString& operator<< (unsigned int iValue);
    VString& operator<< (__int64 iValue);
    VString& operator<< (unsigned __int64 iValue);

    VString& printf  (char const *pFormat, ...);
    VString& vprintf (char const *pFormat, va_list ap);
    VString& vsafeprintf (char const *pFormat, va_list ap);

    VString& quote (char const *pString);

    void setTo (VString const &rOther) {
	m_iStorage.setTo (rOther.m_iStorage);
    }

    void setTo (char const *pString, size_t sString);

    void setTo (char const *pString) {
	setTo (pString, pString ? strlen (pString) : 0);
    }
    void setTo (char iValue) {
	setTo (&iValue, 1);
    }
    void setTo (double iValue) {
	clear (); *this << iValue;
    }
    void setTo (int iValue) {
	clear (); *this << iValue;
    }
    void setTo (unsigned int iValue) {
	clear (); *this << iValue;
    }

    void setToMoved (VString &rOther);

    void setToStatic (char const *pString, size_t sString);
    void setToStatic (char const *pString);

    ThisClass &operator= (VString const &rOther) {
	setTo (rOther);
	return *this;
    }
    ThisClass &operator= (char const *pOther) {
	setTo (pOther);
	return *this;
    }

//  Index
 public:
    int index (char iChar, unsigned int cOccurence) const;

//  Extraction
public:
    bool getPrefix (char iDelimiter,        VString &rPrefix, VString &rRemainingString) const;
    bool getPrefix (char const *pDelimiter, VString &rPrefix, VString &rRemainingString) const;

    /**
     * Retrieves the summary of this string.
     *
     * @param[out] rSummary the returned summary.
     * @param[in] cTotalSize the desired summary length.
     */
    void getSummary (VString &rSummary, unsigned int cTotalSize) const;

//  Search and Replace 
public:
    void replaceSubstring (char const *pSubstring, char const *pReplacement); //in-place 
    void replaceSubstring (
	char const *pSubstring, char const *pReplacementString, VString &rResult
    ) const;
    void convertLFtoCRLF (VString &dest) const;

//  State
protected:
    V::VCOS m_iStorage;
};


#endif
