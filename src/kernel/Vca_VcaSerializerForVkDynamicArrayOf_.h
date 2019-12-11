#ifndef Vca_VcaSerializerForVkDynamicArrayOf__Interface
#define Vca_VcaSerializerForVkDynamicArrayOf__Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VcaSerializer.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <typename T> class VTrigger;

    /*********************************************************
     *----  VcaSerializerForVkDynamicArrayOfGenericType  ----*
     *********************************************************/

    template <typename ElementType> class VcaSerializerForVkDynamicArrayOfGenericType : public VcaSerializer {
	DECLARE_ABSTRACT_RTTLITE (VcaSerializerForVkDynamicArrayOfGenericType<ElementType>,VcaSerializer);

    //  Data Type
    public:
	typedef VkDynamicArrayOf<ElementType> DataType;

    //  Sequencer
    public:
	typedef VTrigger<ThisClass> Sequencer;

    //  Construction
    protected:
	VcaSerializerForVkDynamicArrayOfGenericType (
	    VcaSerializer *pCaller, DataType &rDatum
	) : VcaSerializer (pCaller), m_rDatum (rDatum), m_cElements (rDatum.elementCount ()), m_xElement (0) {
	    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doCount));
	}

    //  Destruction
    protected:
	~VcaSerializerForVkDynamicArrayOfGenericType () {
	}

    //  Sequencer Actions
    protected:
	void doCount (Sequencer *pSequencer) {
	    pSequencer->setActionTo (isIncoming () ? &ThisClass::doSetup : &ThisClass::doElement);
	    start (this, New_VcaSerializer_(this, m_cElements));
	}

	void doSetup (Sequencer *pSequencer) {
	    m_rDatum.Append (m_cElements);
	    pSequencer->setActionTo (&ThisClass::doElement);
	}

	void doElement (Sequencer *pSequencer) {
	    if (m_xElement < m_cElements)
		start (this, New_VcaSerializer_(this, m_rDatum[m_xElement++]));
	    else
		clearSequencer ();
	}

    //  State
    protected:
	DataType &m_rDatum; 

	U32 m_cElements;	// count of the elements moved
	U32 m_xElement;	// index of the current element being moved
    };


    /*****************************************************
     *----  VcaSerializerForVkDynamicArrayOfPODType  ----*
     *****************************************************/

    template <typename ElementType> class VcaSerializerForVkDynamicArrayOfPODType : public VcaSerializer {
	DECLARE_ABSTRACT_RTTLITE (VcaSerializerForVkDynamicArrayOfPODType<ElementType>, VcaSerializer);

    //  Data Type
    public:
	typedef VkDynamicArrayOf<ElementType> DataType;

    //  Sequencer
    public:
	typedef VTrigger<ThisClass> Sequencer;

    //  Construction
    public:
	VcaSerializerForVkDynamicArrayOfPODType (
	    VcaSerializer *pCaller, DataType &rDatum
	) : VcaSerializer (pCaller), m_rDatum (rDatum), m_cElements (rDatum.elementCount ()) {
	    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doCount));
	}

    ///  Destruction
    protected:
	~VcaSerializerForVkDynamicArrayOfPODType () {
	}

    //  Sequencer Actions
    protected:
	void doCount (Sequencer *pSequencer) {
	    pSequencer->setActionTo (&ThisClass::doElement);
	    start (this, New_VcaSerializer_(this, m_cElements));
	}

	void doElement (Sequencer *pSequencer) {
	    if (isIncoming ())
		m_rDatum.Append (m_cElements);
	    clearSequencer ();
	    transferData ();
	}

    //  Transport Callbacks
    protected:
	void localizeData () {
	    peer ()->localize (m_rDatum);
	}

	void getData () {
	    get (m_rDatum.elementArray (), m_cElements*sizeof (ElementType));
	}
	void putData () {
	//********************************
        //  In some cases, a VkDynamicArrayOf_<T> can share a single copy of its
	//  element array with other VkDynamicArrayOf_<T> instances.  The sharing
	//  is broken and a private copy of the element array is made if a
	//  writeable pointer to any of the elements is requested.  Accessing
	//  the element array from a non-const instance is one of the ways to
	//  request a writeable pointer.  Since 'put' only wants a pointer to an
	//  array of 'const' elements, make sure we don't break the sharing by
	//  asking for the element array from a 'const' dynarray...
	//********************************
	    put (const_cast<DataType const&>(m_rDatum).elementArray (), m_cElements*sizeof (ElementType));
	}

    //  State
    protected:
	DataType &m_rDatum;
	U32 m_cElements;
    };


    /********************************************************
     *----  VcaSerializerForVkDynamicArrayOfStringType  ----*
     ********************************************************/

    template <typename ElementType> class VcaSerializerForVkDynamicArrayOfStringType : public VcaSerializer {
	DECLARE_CONCRETE_RTTLITE (VcaSerializerForVkDynamicArrayOfStringType<ElementType>,VcaSerializer);

    //  Data Type
    public:
	typedef VkDynamicArrayOf<ElementType> DataType;

    //  Sequencer
    public:
	typedef VTrigger<ThisClass> Sequencer;

    //  Construction
    public:
	VcaSerializerForVkDynamicArrayOfStringType (
	    VcaSerializer *pCaller, DataType &rDatum
	) : VcaSerializer (pCaller), m_rDatum (rDatum), m_cElements (rDatum.elementCount ()) {
	    m_pSequencer.setTo (new Sequencer (this, &ThisClass::doCount));
	}

    //  Destruction
    protected:
	~VcaSerializerForVkDynamicArrayOfStringType () {
	}

    //  Sequencer Actions
    protected:
	void doCount (Sequencer *pSequencer) {
	    pSequencer->setActionTo (&ThisClass::doSize);
	    start (this, New_VcaSerializer_(this, m_cElements));
	}

	void doSize (Sequencer *pSequencer) {       
	    if (isOutgoing ()) {  // put data
		m_iElementSizes.Append (m_cElements);    
		for (unsigned int xElement = 0; xElement < m_cElements; xElement++) {
		    m_iElementSizes[xElement] = m_rDatum[xElement].storageSize () - 1;  // excluding null
		}
            }            
	    pSequencer->setActionTo (&ThisClass::doData);
	    start (this, New_VcaSerializer_(this, m_iElementSizes));
	}

	void doData (Sequencer *pSequencer) {
	    if (isOutgoing ()) {  // put data
		unsigned int iLength = 0;
		for (unsigned int xEle = 0; xEle < m_cElements; xEle++) 
		    iLength += m_iElementSizes[xEle];
                V::pointer_t pPos = m_iConcatenatedData.guarantee (iLength + 1);   // including null
		*pPos = '\0';
		for (unsigned int xElement = 0; xElement < m_cElements; xElement++) {
		    strncat (pPos, m_rDatum[xElement].storage (), m_iElementSizes[xElement] + 1);  // including null
		    pPos += m_iElementSizes[xElement];
		}
	    }
	    pSequencer->setActionTo (&ThisClass::doArray);
	    start (this, New_VcaSerializer_(this, m_iConcatenatedData));
	}

	void doArray (Sequencer *pSequencer) {
	    if (isIncoming ()) { // get data
		m_rDatum.Append (m_cElements);
		V::pointer_t pPos = m_iConcatenatedData.storage ();   
		for (unsigned int xElement = 0; xElement < m_cElements; xElement++) {
		    m_rDatum[xElement].setTo (pPos, m_iElementSizes[xElement]); 
		    pPos += m_iElementSizes[xElement]; 
		}  
	    }
	    clearSequencer ();
	}

    //  State
    protected:
	DataType	       &m_rDatum; 
        VkDynamicArrayOf<U32>	m_iElementSizes;
	ElementType		m_iConcatenatedData;
	U32			m_cElements;	
    };


    /************************************
     *----  Serialization Policies  ----*
     ************************************/

    namespace SerializationPolicy {
	template <typename ElementType> struct VkDynamicArrayOf {
	    typedef VcaSerializerForVkDynamicArrayOfGenericType<ElementType> Implementation;
	};

	template <> struct VkDynamicArrayOf<bool> {
	    typedef VcaSerializerForVkDynamicArrayOfPODType<bool> Implementation;
	};

	template <> struct VkDynamicArrayOf<S08> {
	    typedef VcaSerializerForVkDynamicArrayOfPODType<S08> Implementation;
	};
	template <> struct VkDynamicArrayOf<U08> {
	    typedef VcaSerializerForVkDynamicArrayOfPODType<U08> Implementation;
	};

	template <> struct VkDynamicArrayOf<S16> {
	    typedef VcaSerializerForVkDynamicArrayOfPODType<S16> Implementation;
	};
	template <> struct VkDynamicArrayOf<U16> {
	    typedef VcaSerializerForVkDynamicArrayOfPODType<U16> Implementation;
	};

	template <> struct VkDynamicArrayOf<S32> {
	    typedef VcaSerializerForVkDynamicArrayOfPODType<S32> Implementation;
	};
	template <> struct VkDynamicArrayOf<U32> {
	    typedef VcaSerializerForVkDynamicArrayOfPODType<U32> Implementation;
	};

	template <> struct VkDynamicArrayOf<S64> {
	    typedef VcaSerializerForVkDynamicArrayOfPODType<S64> Implementation;
	};
	template <> struct VkDynamicArrayOf<U64> {
	    typedef VcaSerializerForVkDynamicArrayOfPODType<U64> Implementation;
	};

	template <> struct VkDynamicArrayOf<F32> {
	    typedef VcaSerializerForVkDynamicArrayOfPODType<F32> Implementation;
	};
	template <> struct VkDynamicArrayOf<F64> {
	    typedef VcaSerializerForVkDynamicArrayOfPODType<F64> Implementation;
	};

	template <> struct VkDynamicArrayOf<VString> {
	    typedef VcaSerializerForVkDynamicArrayOfStringType<VString> Implementation;
	};
    } //  namespace SerializationPolicy


    /***********************************************
     *----  VcaSerializerForVkDynamicArrayOf_  ----*
     ***********************************************/

    template <typename ElementType>
    class VcaSerializerForVkDynamicArrayOf_ : public SerializationPolicy::VkDynamicArrayOf<ElementType>::Implementation {
	DECLARE_CONCRETE_RTTLITE (
	    VcaSerializerForVkDynamicArrayOf_<ElementType>, typename SerializationPolicy::VkDynamicArrayOf<ElementType>::Implementation
	);

    //  Construction
    public:
	VcaSerializerForVkDynamicArrayOf_(VcaSerializer *pCaller, typename BaseClass::DataType &rDatum) : BaseClass (pCaller, rDatum) {
	}

    //  Destruction
    protected:
	~VcaSerializerForVkDynamicArrayOf_() {
	}
    };
}  //  namespace Vca


#endif
