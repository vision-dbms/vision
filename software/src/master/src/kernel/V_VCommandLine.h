#ifndef V_VCommandLine_Interface
#define V_VCommandLine_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

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
    class V_API VCommandLine : public VTransient {
	DECLARE_FAMILY_MEMBERS (VCommandLine, VTransient);

    //  Cursor
    public:
	class V_API Cursor : public VTransient {
	    DECLARE_FAMILY_MEMBERS (Cursor, VTransient);

	//  Construction
	public:
	    Cursor (Cursor const &rOther)
		: m_pCommandLine (rOther.m_pCommandLine)
		, m_xCommandWord (rOther.m_xCommandWord)
	    {
	    }
	    Cursor (VCommandLine const *pCommandLine)
		: m_pCommandLine (pCommandLine), m_xCommandWord (0)
	    {
	    }

	//  Destruction
	public:
	    ~Cursor () {
	    }

	//  Update
	public:
	    void setTo (Cursor const &rOther) {
		m_pCommandLine = rOther.m_pCommandLine;
		m_xCommandWord = rOther.m_xCommandWord;
	    }
	    void setTo (VCommandLine const *pCommandLine) {
		m_pCommandLine = pCommandLine;
		m_xCommandWord = 0;
	    }
	    Cursor &operator= (Cursor const &rOther) {
		setTo (rOther);
		return *this;
	    }
	    Cursor &operator= (VCommandLine *pCommandLine) {
		setTo (pCommandLine);
		return *this;
	    }

	//  Use
	public:
	    char const *firstString (			// -xyz=StringValue
		char const *pTag, char const *pVariable = 0, char const *pDefaultValue = 0
	    ) {
		reset ();
		return nextString (pTag, pVariable, pDefaultValue);
	    }
	    bool firstSwitch (				// -xyz
		char const *pTag, char const *pVariable = 0
	    ) {
		reset ();
		return nextSwitch (pTag, pVariable);
	    }

	    char const *firstTag () {			// -xyz(etc.)
		reset ();
		return nextTag ();
	    }
	    char const *firstToken () {			// xyz BUT NOT -xyz(etc.)
		reset ();
		return nextToken ();
	    }
	    char const *firstWord () {			// xyz OR -xyz(etc.)
		reset ();
		return nextWord ();
	    }

	    char const *nextString (			// -xyz=StringValue
		char const *pTag, char const *pVariable = 0, char const *pDefaultValue = 0
	    );
	    bool nextSwitch (				// -xyz
		char const *pTag, char const *pVariable = 0
	    );

	    char const *nextTag ();			// -xyz(etc.)
	    char const *nextToken ();			// xyz BUT NOT -xyz(etc.)
	    char const *nextWord () {			// xyz OR -xyz(etc.)
		return m_pCommandLine->word (++m_xCommandWord);
	    }

	    void reset () {
		m_xCommandWord = 0;
	    }

	private:
	    static char const *consumeTag (char const *pWord);

	//  State
	private:
	    VCommandLine const *m_pCommandLine;
	    unsigned int	m_xCommandWord;
	};
	friend class Cursor;

    //  Construction
    public:
	VCommandLine (int cArgs, argv_t pArgs) : m_cArgs (cArgs), m_pArgs (pArgs) {
	}

    //  Destruction
    public:
	~VCommandLine () {
	}

    //  Access
    public:
	char const *arg0 () const {
	    return m_pArgs[0];
	}
	unsigned int argc () const {
	    return m_cArgs;
	}
	argv_t argv () const {
	    return m_pArgs;
	}

	bool getValue (					// -xyz=unsigned#
	    size_t &rResult, char const *pTag, char const *pVariable = 0
	) const;

	char const *stringValue (			// -xyz=StringValue
	    char const *pTag, char const *pVariable = 0, char const *pDefaultValue = 0
	) const;
	bool switchValue (				// -xyz
	    char const *pTag, char const *pVariable = 0
	) const;

    private:
	char const *word (unsigned int xWord) const {
	    return xWord < m_cArgs ? m_pArgs[xWord] : 0;
	}

    //  State
    protected:
	unsigned int const m_cArgs;
	argv_t const m_pArgs;
    };
}


#endif
