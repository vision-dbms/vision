#ifndef VEvaluatorPump_Interface
#define VEvaluatorPump_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vsa_VEvaluatorPump.h"
#include "Vsa_VGenericEvaluation.h"

#include "Vca_VcaGofer.h"

#include "Vxa_ICollection.h"
#include "Vxa_ISingleton.h"

/*************************
 *****  Definitions  *****
 *************************/

class VEvaluatorPump : public Vsa::VEvaluatorPump {
    DECLARE_CONCRETE_RTTLITE (VEvaluatorPump, Vsa::VEvaluatorPump);

//  Aliases
public:
    typedef Vsa::IEvaluatorClient IEvaluatorClient;
    typedef Vsa::VEvaluation VEvaluation;

/****************************************
 *----  VEvaluatorPump::Evaluation  ----*
 ****************************************/
public:
    class Evaluation : public Vsa::VGenericEvaluation {
	DECLARE_CONCRETE_RTTLITE (Evaluation, VGenericEvaluation);

    //  Aliases
    public:
	typedef Vxa::ICollection ICollection;
	typedef Vxa::ISingleton  ISingleton;

	typedef Vca::IDataSource<ICollection*> ICollectionSource;
	typedef Vca::IDataSource<ISingleton*>  ISingletonSource;

	typedef Vca::VGoferInterface<ICollection> icollection_gofer_t;
	typedef Vca::VGoferInterface<ISingleton>  isingleton_gofer_t;
	
    //  Construction
    public:
	Evaluation (
	    VEvaluator *pEvaluator, IEvaluatorClient *pClient, VString const &rPath, VString const &rExpression
	);
	Evaluation (
	    VEvaluator *pEvaluator, IEvaluatorClient *pClient, VString const &rPath, VString const &rQuery, VString const &rEnvironment
	);

    //  Destruction
    private:
	~Evaluation ();

    //  Gofer Order Fulfillment
    public:
	template <typename gofer_reference_t> bool fulfill (gofer_reference_t &rpGofer) {
	    return createClientObjectGofer (rpGofer);
	}
	bool fulfill (icollection_gofer_t::Reference &rpGofer);
	bool fulfill (isingleton_gofer_t::Reference &rpGofer);
    private:
	template <typename gofer_reference_t> bool cachedClientObjectGofer (
	    gofer_reference_t &rpResultGofer, gofer_reference_t &rpCachedGofer
	) {
	    if (rpCachedGofer.isNil ())
		createClientObjectGofer (rpCachedGofer);
	    rpResultGofer.setTo (rpCachedGofer);
	    return rpResultGofer.isntNil ();
	}

	template <typename gofer_reference_t> bool createClientObjectGofer (gofer_reference_t &rpGofer) {
	    return BaseClass::newClientObjectGofer (rpGofer);
	}
	bool createClientObjectGofer (icollection_gofer_t::Reference &rpGofer);
	bool createClientObjectGofer (isingleton_gofer_t::Reference &rpGofer);

	template <typename gofer_reference_t> bool createClientRootObjectGofer (gofer_reference_t &rpGofer) {
            typedef typename gofer_reference_t::ReferencedClass::interface_t interface_t;
	    if (!BaseClass::newClientObjectGofer (rpGofer))
		rpGofer.clear ();
	    else {
#if 0
                typename Vca::VGoferInterface<Vca::IDataSource<interface_t*> >::Reference pSourceGofer;
                if (BaseClass::newClientObjectGofer (pSourceGofer)) {
		    gofer_reference_t pObjectGofer (new Vca::Gofer::Sourced<interface_t> (pSourceGofer));
                    pObjectGofer->setFallbackTo (rpGofer);
                    pObjectGofer->setFallbackOnNull ();
                    rpGofer.claim (pObjectGofer);
		}
#endif
		gofer_reference_t pObjectGofer (new Vca::Gofer::Identity<interface_t> (rpGofer));
		pObjectGofer->setFallbackTo (rpGofer);
		pObjectGofer->setFallbackOnNull ();
		rpGofer.claim (pObjectGofer);
	    }
	    return rpGofer.isntNil ();
	}

    //  State
    private:
	icollection_gofer_t::Reference	m_pICollectionGofer;
	isingleton_gofer_t::Reference	m_pISingletonGofer;
    };

/*****************************************************************************************************************
 *----  template <typename gofer_reference_t> VEvaluatorPump::Evaluation::MyKindOfOrder_<gofer_reference_t>  ----*
 *****************************************************************************************************************/
public:
    template <typename gofer_reference_t> class MyKindOfOrder_ : public Evaluation::GoferOrder_<gofer_reference_t> {
	DECLARE_FAMILY_MEMBERS (MyKindOfOrder_<gofer_reference_t>, Evaluation::GoferOrder_<gofer_reference_t>);

    //  Construction
    public:
	MyKindOfOrder_(gofer_reference_t &rpResult) : BaseClass (rpResult) {
	}

    //  Destruction
    public:
	~MyKindOfOrder_() {
	};

    //  Use
    protected:
	bool fulfillUsing_(VEvaluation *pEvaluation) const {
	    Evaluation::Reference const pMyKindOfEvaluation (dynamic_cast<Evaluation*>(pEvaluation));
	    return pMyKindOfEvaluation ? pMyKindOfEvaluation->fulfill (m_rpResult) : BaseClass::fulfillUsing_(pEvaluation);
	}

    //  State
    protected:
	using BaseClass::m_rpResult;
    };

/****************************************************************/
//  Construction
public:
    VEvaluatorPump (
        PumpSettings *pSettings, Vca::VBSConsumer *pPipeToPeer, Vca::VBSProducer *pPipeToHere
    );

//  Destruction
private:
    ~VEvaluatorPump ();

//  Access
public:
    template <typename gofer_reference_t> bool clientObjectGofer (gofer_reference_t &rpGofer) const {
	rpGofer.clear ();
	MyKindOfOrder_<gofer_reference_t> iMyKindOfOrder (rpGofer);
	return incomingClient() && incomingClient()->fulfill (iMyKindOfOrder);
    }

//  Evaluation
protected:
    virtual /*override*/ VEvaluation* createEvaluation (
	IEvaluatorClient *pClient, VString const &rPath, VString const &rExpression
    );
    virtual /*override*/ VEvaluation* createEvaluation (
	IEvaluatorClient* pClient, VString const &rPath, VString const &rQuery, VString const &rEnvironment
    );

//  Cancellation
protected:
    void cancelCurrent ();
};

#endif
