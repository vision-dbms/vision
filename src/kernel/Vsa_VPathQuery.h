#ifndef Vsa_VPathQuery_Interface
#define Vsa_VPathQuery_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "VkMapOf.h"

#include "V_VString.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    class Vsa_API VPathQuery : public VReferenceable {
	DECLARE_CONCRETE_RTTLITE (VPathQuery, VReferenceable);

    //  Environment
    public:
	class Vsa_API Environment {
	//  Map
	public:
	    typedef VkMapOf<VString, char const*, char const*, VString> Map;

	//  Construction
	public:
	    Environment (VString const &rString) : m_iString (rString), m_bNotMapped (true) {
	    }
	    Environment () : m_bNotMapped (false) {
	    }

	//  Destruction
	public:
	    ~Environment () {
	    }

	//  Access
	public:
	    bool getValue (char const *pKey, VString &rResult);

	    VString const &string () const {
		return m_iString;
	    }

	//  State
	private:
	    VString const	m_iString;
	    Map			m_iMap;
	    bool		m_bNotMapped;
	};

    //  Formatter
    public:
	class Vsa_API Formatter {
	//  Construction
	public:
	    Formatter (
		char const *pClass, bool bClassCopy, char const *pTemplate, bool bTemplateCopy
	    );
 	    Formatter (VString const &rClass, VString const &rTemplate);
	    Formatter (Formatter const &rOther);

	//  Destruction
	public:
	    ~Formatter () {
	    }

	//  Access
	public:
	    VString const &getClass () const {
		return m_iClass;
	    }
	    VString const &getTemplate () const {
		return m_iTemplate;
	    }

	//  Update
	public:
	    void setClassTo (char const *pValue) {
		m_iClass.setTo (pValue);
	    }
	    void setClassTo (VString const &rValue) {
		m_iClass.setTo (rValue);
	    }
	    void setTemplateTo (char const *pValue) {
		m_iClass.setTo (pValue);
	    }
	    void setTemplateTo (VString const &rValue) {
		m_iTemplate.setTo (rValue);
	    }

	//  State
	private:
	    VString m_iClass;
	    VString m_iTemplate;
	};

    //  Construction
    public:
	VPathQuery (VString const &rPath, VString const &rQuery, VString const &rEnvironment);
	VPathQuery (VString const &rPath, VString const &rQuery);

    //  Destruction
    protected:
      ~VPathQuery ();

    //  Access
    public:
	VString const &environmentString () const {
	    return m_iEnvironment.string ();
	}
	VString const &pathString () const {
	    return m_iPath;
	}
	VString const &queryString () const {
	    return m_iQuery;
	}

	bool getValue (char const *pKey, VString &rResult) {
	    return m_iEnvironment.getValue (pKey, rResult);
	}

	void getVisionExpression (Formatter const &rFormatter, VString &rResult);


    //  State
    private:
	VString	const	m_iPath;
	VString	const	m_iQuery;
	Environment	m_iEnvironment;
        VReference<ThisClass> m_pSuccessor;
        VReference<ThisClass> m_pPredecessor;
    };
}


#endif
