#ifndef VcaSerializer__Interface
#define VcaSerializer__Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VcaSerializer.h"

#include "Vca_VcaPeerCharacteristics.h"
#include "Vca_VcaRouteStatistics.h"

#include "VComponentReference.h"

#include "Vca_VTypeInfoHolder.h"

/**************************
 *****  Declarations  *****
 **************************/

namespace V {
    class VBlob;
}

namespace Vca {
    class VPeerData;
    class VPeerDataArray;
}

#include "Vca_VcaSite.h"
#include "Vca_VTrigger.h"


/*************************
 *****  Definitions  *****
 *************************/

#ifdef VCASERIALIZER_FAMILY_UNIT
#undef VCASERIALIZER_FAMILY_UNIT
#endif
#define VCASERIALIZER_FAMILY_UNIT VcaSerializer_

namespace Vca {
    class IBSConsumer;
    class IBSProducer;

    class VBSConsumer;
    class VBSProducer;

    /***************************************************
     *----  template <class Datum> VcaSerializer_  ----*
     ***************************************************/

    template <class Datum> class VcaSerializer_ : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (Datum);

    //  Construction
    public:
	VcaSerializer_(
	    VcaSerializer *pCaller, DataType &rDatum
	) : VcaSerializer (pCaller), m_rDatum (rDatum) {
	    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doData));
	}

    //  Destruction
    protected:
	~VcaSerializer_() {
	}

    //  Sequencer Actions
    protected:
	void doData (Sequencer *pSequencer) {
	    clearSequencer ();
	    transferData ();
	}

    //  Transport Callbacks
    protected:
	virtual void localizeData () OVERRIDE {
	    peer ()->localize (m_rDatum);
	}

	virtual void getData () OVERRIDE {
	    get (&m_rDatum, sizeof (Datum));
	}
	virtual void putData () OVERRIDE {
	    put (&m_rDatum, sizeof (Datum));
	}

    //  State
    protected:
	DataType &m_rDatum;
    };


    /******************************************
     *----  VcaSerializer_<VBSConsumer*>  ----*
     ******************************************/

    template <> class VcaSerializer_<VBSConsumer*> : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (VBSConsumer*);

    //  Construction
    public:
	VcaSerializer_(VcaSerializer *pCaller, DataType &rDatum);

    //  Destruction
    protected:
	~VcaSerializer_();

    protected:
	void doInterface (Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	virtual void wrapupIncomingSerialization () OVERRIDE;

    //  State
    protected:
	DataType &m_rDatum;
	VReference<IBSConsumer> m_pInterface;
    };


    /******************************************
     *----  VcaSerializer_<VBSProducer*>  ----*
     ******************************************/

    template <> class VcaSerializer_<VBSProducer*> : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (VBSProducer*);

    //  Construction
    public:
	VcaSerializer_(VcaSerializer *pCaller, DataType &rDatum);

    //  Destruction
    protected:
	~VcaSerializer_();

    //  Sequencer Actions
    protected:
	void doInterface (Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	virtual void wrapupIncomingSerialization () OVERRIDE;

    //  State
    protected:
	DataType &m_rDatum;
	VReference<IBSProducer> m_pInterface;
    };


    /**************************************
     *----  VcaSerializer_<VcaSite*>  ----*
     **************************************/

    template <> class VcaSerializer_<VcaSite*> : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (VcaSite*);

    //  Peer Kind
    public:
	enum PeerKind {
	    PeerKind_Unknown,
	    PeerKind_New,
	    PeerKind_Old
	};

    //  Construction
    public:
	VcaSerializer_(VcaSerializer *pCaller, DataType &rDatum);

    //  Destruction
    protected:
	~VcaSerializer_() {
	}

    // Query
    public:
	bool peerKindIsUnKnown () const {
	    return m_xPeerKind == PeerKind_Unknown;
	}

    //  Sequencer Actions
    protected:
	void doData			(Sequencer *pSequencer);
	void doCharacteristics		(Sequencer *pSequencer);
	void doRoutingInformation	(Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	virtual void localizeData () OVERRIDE;

	virtual void getData () OVERRIDE;
	virtual void putData () OVERRIDE;

	virtual void wrapupIncomingSerialization () OVERRIDE;

    //  State
    protected:
	DataType&		m_rDatum;
	VcaPeerCharacteristics	m_iCharacteristics;
	U08			m_xPeerKind;
	SSID			m_iSSID;
	VcaRouteStatistics	m_iBestRouteMetrics;
    };


    /************************************************
     *----  VcaSerializer_<VcaRouteStatistics>  ----*
     ************************************************/

    template <> class VcaSerializer_<VcaRouteStatistics> : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (VcaRouteStatistics);

    //  Construction
    public:
	VcaSerializer_(VcaSerializer *pCaller, DataType &rDatum);

    //  Destruction
    protected:
	~VcaSerializer_() {
	}

    //  Sequencer Actions
    protected:
	void doData (Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	virtual void localizeData () OVERRIDE;

	virtual void getData () OVERRIDE;
	virtual void putData () OVERRIDE;

	virtual void wrapupIncomingSerialization () OVERRIDE;

    //  State
    protected:
	DataType&	m_rDatum;
	unsigned int	m_iHopCount;
    };


    /**************************************
     *----  VcaSerializer_<V::VBlob>  ----*
     **************************************/

    template <> class VcaSerializer_<V::VBlob> : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (V::VBlob);

    //  Construction
    public:
	VcaSerializer_(VcaSerializer *pCaller, DataType &rDatum);

    //  Destruction
    protected:
	~VcaSerializer_() {
	}

    //  Query
    private:
	bool movingSize () const {
	    return IsNil (m_pData);
	}
	bool movingData () const {
	    return IsntNil (m_pData);
	}

    //  Sequencer Actions
    protected:
	void doData (Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	virtual void localizeData () OVERRIDE;

	virtual void getData () OVERRIDE;
	virtual void putData () OVERRIDE;

	virtual void wrapupIncomingSerialization () OVERRIDE;

    //  State
    protected:
	DataType&	m_rDatum;
	U32		m_sData;
	void*		m_pData;
    };


    /**************************************
     *----  VcaSerializer_<V::VTime>  ----*
     **************************************/

    template <> class VcaSerializer_<V::VTime> : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (V::VTime);

    //  Construction
    public:
	VcaSerializer_(VcaSerializer *pCaller, DataType &rDatum);

    //  Destruction
    protected:
	~VcaSerializer_() {
	}

    //  Sequencer Actions
    protected:
	void doNanosecondsSinceEpoch (Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	virtual void wrapupIncomingSerialization () OVERRIDE;

    //  State
    protected:
	DataType&	m_rDatum;
	U64		m_iNanosecondsSinceEpoch;
    };


    /**********************************************************
     *----  VcaSerializer_<Vca::VInterfaceMember const*>  ----*
     **********************************************************/

    template <> class VcaSerializer_<Vca::VInterfaceMember const*> : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (VInterfaceMember const *);

    //  Construction
    public:
	VcaSerializer_(VcaSerializer *pCaller, DataType &rDatum);

    //  Destruction
    protected:
	~VcaSerializer_();

    //  Sequencer Actions
    protected:
	void doData (Sequencer *pSequencer);
	void doType (Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	virtual void localizeData () OVERRIDE;

	virtual void getData () OVERRIDE;
	virtual void putData () OVERRIDE;

	virtual void wrapupIncomingSerialization () OVERRIDE;

    //  State
    protected:
	DataType&		m_rDatum;
	U32			m_xMember;
	VTypeInfo::Reference	m_pInterfaceType;
    };


    /****************************************
     *----  VcaSerializer_<VPeerData>   ----*
     ****************************************/

    template <> class VcaSerializer_<VPeerData> : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (VPeerData);

    public:
	typedef VkSetOf<VkUUIDHolder, uuid_t const &> ListenerDomains;
	typedef VkSetOf<VkUUIDHolder, uuid_t const &> ConnectorDomains; 
	typedef VkSetOf<VkUUIDHolder, uuid_t const &> ConnectedPeers;

    //  Construction
    public:
	VcaSerializer_(VcaSerializer *pCaller, DataType &rDatum);

    //  Destruction
    protected:
	~VcaSerializer_() {
	}

    //  Sequencer Actions
    protected:

	void doInFinalPath          (Sequencer *pSequencer);
	void doData                 (Sequencer *pSequencer);

	void doListenerDomains      (Sequencer *pSequencer);
	void doConnectorDomains     (Sequencer *pSequencer);
	void doConnectedPeers       (Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	virtual void localizeData () OVERRIDE;

	virtual void getData () OVERRIDE;
	virtual void putData () OVERRIDE;

	virtual void wrapupIncomingSerialization () OVERRIDE;

    //  State
    protected:
	DataType&   m_rDatum;
    };


    /******************************************
     *---  VcaSerializer_<VPeerDataArray>  ---*
     ******************************************/

    template <> class VcaSerializer_<VPeerDataArray> : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (VPeerDataArray);

    //  Construction
    public:
	VcaSerializer_(VcaSerializer *pCaller, DataType &rDatum);

    //  Destruction
    protected:
	~VcaSerializer_() {
	}

    //  Sequencer Actions
    protected:
	void doCount                (Sequencer *pSequencer);
	void doPeerDataSetup        (Sequencer *pSequencer);
	void doPeerData             (Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	virtual void localizeData () OVERRIDE;

	virtual void getData () OVERRIDE;
	virtual void putData () OVERRIDE;

	virtual void wrapupIncomingSerialization () OVERRIDE;

    // State
    private:
	 DataType&	m_rDatum;
	 U32		m_cElements;
	 U32		m_xElement;
    };


    /*************************************
     *----  VcaSerializer_<VString>  ----*
     *************************************/

    template <> class VcaSerializer_<VString> : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (VString);

    //  Construction
    public:
	VcaSerializer_(VcaSerializer *pCaller, DataType &rDatum);

    //  Destruction
    protected:
	~VcaSerializer_() {
	}

    //  Query
    private:
	bool movingData () const {
	    return IsntNil (m_pData);
	}
	bool movingSize () const {
	    return IsNil (m_pData);
	}

    //  Sequencer Actions
    protected:
	void doData (Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	virtual void localizeData () OVERRIDE;

	virtual void getData () OVERRIDE;
	virtual void putData () OVERRIDE;

    //  State
    protected:
	DataType&	m_rDatum;
	U32		m_sData;
	char*		m_pData;
    };


    /****************************************
     *----  VcaSerializer_<VTypeInfo*>  ----*
     ****************************************/

    template <> class VcaSerializer_<VTypeInfo*> : public VcaSerializer {
	DECLARE_VCASERIALIZER_FAMILY (VTypeInfo*);

    //  Parameter Signature Reference
    protected:
	typedef VTypeInfo::ParameterSignature ParameterSignature;
	typedef VComponentReference<ParameterSignature> ParameterSignatureReference;

    //  Type Kind
    public:
	enum TypeKind {
	    TypeKind_Unknown,

	    //  TypeKind_Nil{} ()
	    TypeKind_Nil,

	    //  TypeKind_Reference{this:SSID} ()
	    TypeKind_Reference,

	    //  TypeKind_Simple{this:SSID} (uuid_t iUUID)
	    TypeKind_Simple,

	    //  TypeKind_Subtype{this:SSID} (VTypeInfo *pSuper, uuid_t iUUID)
	    TypeKind_Subtype,

	    //  TypeKind_Template{this:SSID} (
	    //	U32		n,  // parameterCount
	    //	VTypeInfo	*pP1, ... *pPn,
	    //	VTypeInfo	*pSuper,
	    //	uuid_t		iUUID
	    //  )
	    TypeKind_Template,

	    //  TypeKind_General{this:SSID} (
	    //	VTypeInfo	*pScope,
	    //	U32		n,  //  parameterCount
	    //	VTypeInfo	*pP1, ... *pPn,
	    //	VTypeInfo	*pSuper,
	    //	uuid_t		iUUID
	    //  )
	    TypeKind_General
	};
	typedef U08 typekind_wire_t;

    //  Construction
    public:
	VcaSerializer_(VcaSerializer *pCaller, DataType &rDatum);

    //  Destruction
    protected:
	~VcaSerializer_() {
	}

    //  Query
    public:
	bool typeKindIsKnown () const {
	    return m_xTypeKind != TypeKind_Unknown;
	}

    //  Sequencer Actions
    protected:
	void doData		(Sequencer *pSequencer);
	void doParameterCount	(Sequencer *pSequencer);
	void doParameters	(Sequencer *pSequencer);
	void doScope		(Sequencer *pSequencer);
	void doSuper		(Sequencer *pSequencer);
	void doSymbolName	(Sequencer *pSequencer);
	void doSymbolUUID	(Sequencer *pSequencer);

    //  Transport Callbacks
    protected:
	virtual void localizeData () OVERRIDE;

	virtual void getData () OVERRIDE;
	virtual void putData () OVERRIDE;

	virtual void wrapupIncomingSerialization () OVERRIDE;

    //  State
    protected:
	DataType&			m_rDatum;
	VTypeInfo::Reference		m_pScope;
	VTypeInfo::Reference		m_pSuper;
	ParameterSignatureReference	m_pParameters;
	U32				m_cParameters;
	unsigned int			m_xParameter;
	typekind_wire_t			m_xTypeKind;
	SSID				m_iSSID;
	VString				m_iTypeName;
	uuid_t				m_iTypeUUID;
    };
}  // ... end of namespace Vca.


#endif
