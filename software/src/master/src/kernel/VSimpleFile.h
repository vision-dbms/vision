#ifndef VSimpleFile_Interface
#define VSimpleFile_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VSimpleFile : public VTransient {
    //  Globals
    public:
	static char const * const g_pTextReadMode;
	static char const * const g_pTextWriteMode;
	static char const * const g_pTextAppendMode;

    //  Construction
    public:
	VSimpleFile () : m_pStream (0), m_bAutoFlushEnabled (false) {
	}

    //  Destruction
    public:
	~VSimpleFile () {
	    if (m_pStream)
		fclose (m_pStream);
	}

    //  Use
    public:
	bool Open (char const *pFileName, char const *pOpenMode);

	bool OpenForTextRead (char const *pFileName) {
	    return Open (pFileName, g_pTextReadMode);
	}
	bool OpenForTextWrite (char const *pFileName) {
	    return Open (pFileName, g_pTextWriteMode);
	}
	bool OpenForTextAppend (char const *pFileName) {
	    return Open (pFileName, g_pTextAppendMode);
	}

	bool OpenUniqueTemp (VString& rFileName, char const *pPrefix);

	bool GetLine (VString &rLine) const;
	bool GetLine (VString &rLine, char const *pFileName) {
	    return OpenForTextRead (pFileName) && GetLine (rLine);
	}

	bool GetContents (VString &rContents) const;
	bool GetContents (VString &rContents, char const *pFileName) {
	    return OpenForTextRead (pFileName) && GetContents (rContents);
	}

	bool PutLine (char const *pLine) const;
	bool PutLine (char const *pLine, char const *pFileName) {
	    return OpenForTextWrite (pFileName) && PutLine (pLine);
	}

	bool PutString (char const *pString) const;
	bool PutString (char const *pString, char const *pFileName) {
	    return OpenForTextWrite (pFileName) && PutString (pString);
	}

	void flush () const;

	void close ();

	int dup2 (int filedes2) const;

	int __cdecl printf (char const *pFormat, ...) const;
	int vprintf (char const *pFormat, va_list pArguments) const;

	void enableAutoFlush () {
	    m_bAutoFlushEnabled = true;
	}
	void disableAutoFlush () {
	    m_bAutoFlushEnabled = false;
	}

    //  Query
    public:
	bool autoFlushEnabled () const {
	    return m_bAutoFlushEnabled;
	}

	bool isOpen () const {
	    return IsntNil (m_pStream);
	}
	bool isntOpen () const {
	    return IsNil (m_pStream);
	}
	unsigned int Size () const;


    //  State
    private:
	FILE *m_pStream;
	bool m_bAutoFlushEnabled;
    };
}
using V::VSimpleFile;


#endif
