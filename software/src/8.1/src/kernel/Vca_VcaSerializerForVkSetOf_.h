#ifndef VcaSerializerForVkSetOf__Interface
#define VcaSerializerForVkSetOf__Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VcaSerializer.h"


/*************************
 *****  Definitions  *****
 *************************/

/***************************************************************
 *----  template <class SetType> VcaSerializerForVkSetOf_  ----*
 ***************************************************************/

#ifdef VCASERIALIZER_FAMILY_UNIT
#undef VCASERIALIZER_FAMILY_UNIT
#endif
#define VCASERIALIZER_FAMILY_UNIT VcaSerializerForVkSetOf_

namespace Vca {
    template<class T> class VTrigger;

    template <class SetType> class VcaSerializerForVkSetOf_ : public VcaSerializer {
    public:
	DECLARE_VCASERIALIZER_FAMILY (SetType);

    //  Storage Type
    public:
	typedef typename VTypeTraits<typename SetType::EVT>::StorageType StorageType;

    //  Construction
    public:
	VcaSerializerForVkSetOf_(
	    VcaSerializer *pCaller, DataType &rDatum
	) : VcaSerializer (pCaller), m_rDatum (rDatum), m_cElements (UINT_MAX), m_xElement (0) {
	    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doData));
	}

    //  Destruction
    protected:
	~VcaSerializerForVkSetOf_() {
	}

    //  Sequencer Actions
    protected:
	void doData (Sequencer *pSequencer) {
	    pSequencer->setActionTo (&ThisClass::doElementSetup);
	    transferData ();
	}

	/** 
	 This function does setup functionality before actually sending or receiving the element
	    1. If the transport is inbound, then inserts the element obtained in the previous 
	       iteration into the datum
	    2. If the transport is outbound, then copy the element to be sent 
	**/
	void doElementSetup (Sequencer *pSequencer) {
	    if (m_cElements == 0 || m_xElement == m_cElements)  // exit condition
		clearSequencer ();

	    else {
		if (isOutgoing ())
		    m_iElement = m_rDatum [m_xElement];
		else if (m_xElement!=0)
		    m_rDatum.Insert (m_iElement); 

	        pSequencer->setActionTo (&ThisClass::doElement);
	    }
	}

	void doElement (Sequencer *pSequencer) {
	    start (this, New_VcaSerializer_(this, m_iElement));
	    m_xElement++;
	    pSequencer->setActionTo (&ThisClass::doElementSetup);
	}

    //  Transport Callbacks
    protected:
	void localizeData () {
	    peer ()->localize (m_cElements);
	}

	void getData () {
	    get (&m_cElements, sizeof (m_cElements));
	}

	/** 
	    This function determines the cardinality of the set, and sends it over the wire.
	    Also initializes the buffer storage with the first element to be sent
	**/
	void putData () {
	    m_cElements = m_rDatum.cardinality();
	    put (&m_cElements, sizeof (m_cElements)); 
	}

	// This function inserts the last element obtained into the datum 
	void wrapupIncomingSerialization () {
	    if (m_cElements !=0)
		m_rDatum.Insert (m_iElement);
	}       

    //  State
    protected:
	DataType &m_rDatum; 

	U32		m_cElements;	// count of the elements moved
	U32		m_xElement;	// index of the current element being moved
	StorageType	m_iElement;      
    };

    /******************************************
     *----  New_VcaSerializerForVkSetOf_  ----*
     ******************************************/

    template<class SetType> inline VcaSerializer *New_VcaSerializerForVkSetOf_(
	VcaSerializer *pCaller, SetType &rT
    ) {
	return new Vca::VcaSerializerForVkSetOf_<SetType> (pCaller, rT);
    }
}  //  ...  end of namespace Vca.


#endif
