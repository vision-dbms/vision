#ifndef Vxa_VMapMaker_Interface
#define Vxa_VMapMaker_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VRolePlayer.h"

#include "Vxa_VCardinalityHints.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCallType2Exporter;
    class VMonotypeMapMaker;
    class VSet;

    class Vxa_API VMapMaker {
	DECLARE_NUCLEAR_FAMILY (VMapMaker);

    //  Implementation
    public:
	class Vxa_API Implementation : public VRolePlayer {
	    DECLARE_ABSTRACT_RTTLITE (Implementation, VRolePlayer);

	    friend class VMapMaker;

	//  Construction
	protected:
	    Implementation (VCardinalityHints *pTailHints, cardinality_t xCommitLimit = 0);

	//  Destruction
	protected:
	    ~Implementation ();

	//  Access
	public:
	    VCardinalityHints *tailHints () const {
		return m_pTailHints;
	    }

	    cardinality_t commitLimit () const {
		return m_xCommitLimit;
	    }
	    cardinality_t updateLimit () const {
		return m_xUpdateLimit;
	    }

	//  Query
	public:
	    bool isEmpty () const {
		return m_xCommitLimit == 0;
	    }
	    bool isntEmpty () const {
		return m_xCommitLimit > 0;
	    }

	    bool isPending () const {
		return m_xUpdateLimit > m_xCommitLimit;
	    }
	    bool isntPending () const {
		return m_xUpdateLimit == m_xCommitLimit;
	    }

	//  Transmission
	private:
	    virtual bool transmitUsing (VCallType2Exporter *pExporter, object_reference_array_t const &rInjection);
	    virtual bool transmitUsing (VCallType2Exporter *pExporter);

	//  Update
	protected:
	    virtual bool commitThrough (cardinality_t xElement);

	    bool commitUpdate () {
		return commitThrough (m_xUpdateLimit);
	    }
	    void noteUpdate () {
		m_xUpdateLimit = m_xCommitLimit + 1;
	    }
	    void retractUpdate () {
		m_xUpdateLimit = m_xCommitLimit;
	    }
	public:
	    virtual VMapMaker &getMapMakerFor (VMapMaker &rParent, cardinality_t xElement, VSet *pCodomain) = 0;

	//  State
	private:
	    VCardinalityHints::Reference const m_pTailHints;
	    cardinality_t                      m_xCommitLimit;
	    cardinality_t                      m_xUpdateLimit;
	};

    //  Construction
    public:
	VMapMaker ();

    //  Destruction
    public:
	~VMapMaker ();

    //  Access
    public:
	cardinality_t commitLimit () const {
	    return m_pImplementation ? m_pImplementation->commitLimit () : 0;
	}
	cardinality_t updateLimit () const {
	    return m_pImplementation ? m_pImplementation->updateLimit () : 0;
	}

    //  Query
    public:
	bool isEmpty () const {
	    return isntImplemented () || m_pImplementation->isEmpty ();
	}
	bool isntEmpty () const {
	    return isImplemented () && m_pImplementation->isntEmpty ();
	}

	bool isImplemented () const {
	    return m_pImplementation.isntNil ();
	}
	bool isntImplemented () const {
	    return m_pImplementation.isNil ();
	}

	bool isPending () const {
	    return isImplemented () && m_pImplementation->isPending ();
	}
	bool isntPending () const {
	    return isImplemented () && m_pImplementation->isntPending ();
	}

    //  Update
    public:
	bool commitUpdate  () const;
	void retractUpdate () const;

    public:
	ThisClass &getMapMakerFor (cardinality_t xElement, VSet *pCodomain, VMonotypeMapMaker *pChild);
	ThisClass &getMapMakerFor (cardinality_t xElement, VSet *pCodomain, VCardinalityHints *pTailHints);

    //  Implementation
    public:
	template <typename T> bool getImplementation (T &rpImplementation) const {
	    rpImplementation.setTo (
		dynamic_cast<typename T::ReferencedClass*>(m_pImplementation.referent ())
	    );
	    return rpImplementation.isntNil ();
	}
	void setImplementationTo (Implementation *pImplementation);

    //  Transmission
    public:
	bool transmitUsing (VCallType2Exporter *pExporter, object_reference_array_t const &rInjection) const;
	bool transmitUsing (VCallType2Exporter *pExporter) const;

    //  State
    private:
	Implementation::Reference m_pImplementation;
    };
}


#endif
