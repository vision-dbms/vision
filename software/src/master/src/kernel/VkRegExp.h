/*****  Vision Kernel Regular Expression Interface  *****/
#ifndef VkRegExp_Interface
#define VkRegExp_Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VCOS.h"
#include "V_VString.h"

/*************************
 *****  Definitions  *****
 *************************/

class VkRegExp {
public:
    class V_API Query {
	friend class VkRegExp;

    //  Constants
    public:
	static unsigned int const NSUBEXP = 10;

    //  Construction
    public:
	Query (VString const& rString);

    //  Destruction
    public:
	~Query ();

    //  Access
    public:
	VString const& string () const {
	    return m_iString;
	}
	bool getMatch (unsigned int& rxStart, unsigned int& rxEnd, unsigned int xMatch) const;
	bool getMatch (char const*& rpStart, char const*& rpEnd, unsigned int xMatch) const;
	bool getMatch (VString& rResult, unsigned int xMatch) const;

    //  State
    private:
	VString const m_iString;
	char const* m_startp[NSUBEXP];
	char const* m_endp[NSUBEXP];
	char const *reginput;	/* String-input pointer. */
    };

//  Construction
public:
    V_API VkRegExp (char const*);

//  Destruction
public:
    V_API ~VkRegExp ();

//  Access/Query
public:
    VString const& info () const {
	return m_vsInfo;
    }
    bool isValid () const {
	return m_bValid;
    }
    
//  Use
public:
    V_API bool Execute (VString const& rString) const;
    V_API bool Execute (Query& rQuery) const;

//  Implementation
private:
    char *reg (int paren, int *flagp);
    char *regatom (int *);
    char *regbranch (int *);
    char *regpiece (int *);
    char *regnode (char);
    void regc (char);
    void reginsert (char, char *);
    void regtail (char *, char *);
    void regoptail (char *, char *);
    char *regnext (REGISTER char*) const;
    bool regtry (Query& rQuery, char const*) const;
    bool regmatch (Query& rQuery, char*) const;
    int regrepeat (Query& rQuery, char*) const;

//  State
//  ... from VkRegExp::VkRegExp:
private:
    char const *regparse;	/* Input-scan pointer. */
    int regnpar;		/* () count. */
    char regdummy;
    char *regcode;		/* Code-emit pointer; &regdummy = don't. */
    long regsize;		/* Code size. */
private:
    char regstart;		/* Internal use only. */
    char reganch;		/* Internal use only. */
    char const *regmust;	/* Internal use only. */
    int regmlen;		/* Internal use only. */
    V::VCOS programStorage;	/* Internal use only. */
    char *program;		/* Internal use only. */
// construction status
private:
    VString m_vsInfo;
    bool m_bValid;
};


#endif
