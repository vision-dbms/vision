#ifndef Vca_VcaSerializer_Interface
#define Vca_VcaSerializer_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "V_VSchedulable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTypeInfoHolder.h"

/*************************
 *****  Definitions  *****
 *************************/

#define DECLARE_VCASERIALIZER_FAMILY(dataType)\
    DECLARE_CONCRETE_RTTLITE (VCASERIALIZER_FAMILY_UNIT<dataType>, VcaSerializer);\
public:\
    typedef dataType DataType;\
    typedef VTrigger<ThisClass> Sequencer


namespace Vca {
    class ITrigger;

    class VBSConsumer;
    class VBSProducer;

    class VcaConnection;
    class VcaSite;
    class VcaTransport;
    class VcaTransportInbound;
    class VcaTransportOutbound;

    class Vca_API VcaSerializer : public VReferenceable, protected V::VSchedulable {
	DECLARE_VIRTUAL_RTTLITE (VcaSerializer, VReferenceable);

    //  Friends
	friend class VcaConnection;
	friend class VcaTransport;
	friend class VcaTransportInbound;
	friend class VcaTransportOutbound;

    //  Sequencer
    public:
	typedef ITrigger Sequencer;

    //  Construction
    protected:
	VcaSerializer (VcaSerializer *pCaller);
	VcaSerializer (VcaTransport *pTransport);

    //  Destruction
    protected:
	~VcaSerializer ();

    //  Access
    public:
	VcaSerializer *caller () const {
	    return m_pCaller;
	}

	VcaSite *peer () const;

	VcaTransport *transport () const;
	VcaTransportInbound *transportIfInbound () const;
	VcaTransportOutbound *transportIfOutbound () const;

    //  Query
    public:
	bool isIncoming () const;
	bool isOutgoing () const;

	bool aborted () const {
	    return m_bAborted;
	}

    //  Data Transfer
    protected:
	void get (void *pVoid, size_t sVoid);
	void put (void const *pData, size_t sData);

	void transferData ();

    //  Scheduling
    private:
	virtual void schedule_();
	virtual void run_();

    public:
	void abort ();

    protected:
	static void start (VcaSerializer *pParent, VcaSerializer *pChild);

    //  Sequencing
    protected:
	void clearSequencer ();
    
    //  Transport Callbacks
    protected:
	virtual void localizeData ();

	virtual void getData ();
	virtual void putData ();

	virtual void wrapupIncomingSerialization ();
	virtual void wrapupOutgoingSerialization ();

    //  State
    protected:
	Reference		const	m_pCaller;
	VReference<VcaTransport>const	m_pTransport;
	VReference<Sequencer>		m_pSequencer;
	bool				m_bAborted;
    };


    /********************************
     *----  New_VcaSerializer_  ----*
     ********************************/

    template<class T> inline VcaSerializer *New_VcaSerializer_(VcaSerializer *pCaller, T &rT) {
	return VTypeInfoHolder<T>::serializer (pCaller, rT);
    }

    template<class T> inline VcaSerializer *New_VcaSerializer_(VcaSerializer* pCaller, VReference<T>& rT) {
	return New_VcaSerializer_(pCaller, rT._rpReferent_());
    }
}


#endif
