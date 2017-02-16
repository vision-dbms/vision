#ifndef V_VFifoLite_Interface
#define V_VFifoLite_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"


/**************************
 *----  V::VFifoLite  ----*
 **************************/

namespace V {
    class V_API VFifoLite : public VTransient {
    //  Construction
    public:
	VFifoLite (VFifoLite const &rOther)
	    : m_pOrigin (rOther.m_pOrigin)
	    , m_pCPoint (rOther.m_pCPoint)
	    , m_pPPoint (rOther.m_pPPoint)
	    , m_pExtent (rOther.m_pExtent)
	{
	}
	VFifoLite () : m_pOrigin (0), m_pCPoint (0), m_pPPoint (0), m_pExtent (0) {
	}

    //  Destruction
    public:
	~VFifoLite () {
	}

    //  Initialization
    public:
	void clear () {
	    m_pOrigin = m_pExtent = m_pCPoint = m_pPPoint = 0;
	}
	void setTo (VFifoLite const &rOther) {
	    m_pOrigin = rOther.m_pOrigin;
	    m_pCPoint = rOther.m_pCPoint;
	    m_pPPoint = rOther.m_pPPoint;
	    m_pExtent = rOther.m_pExtent;
	}
	void setTo (void const *pData, size_t sData) {
	    m_pCPoint = m_pOrigin = static_cast<char const*>(pData);
	    m_pExtent = m_pOrigin + sData;

	    m_pPPoint = const_cast<char*>(m_pExtent);
	}
	void setTo (void *pVoid, size_t sVoid) {
	    m_pCPoint = m_pOrigin = static_cast<char const*>(pVoid);
	    m_pExtent = m_pOrigin + sVoid;

	    m_pPPoint = const_cast<char*>(m_pOrigin);
	}

	void setToData (void const *pData, size_t sData) {
	    setTo (pData, sData);
	}
	void setToVoid (void *pVoid, size_t sVoid) {
	    setTo (pVoid, sVoid);
	}

    //  Access
    public:

    //  Query
    public:
	bool isValid () const {
	    return IsntNil (m_pOrigin);
	}
	bool isntValid () const {
	    return IsNil (m_pOrigin);
	}

    //  Area Access/Query
    public:
	char const *consumedArea () const {
	    return m_pOrigin;
	}
	size_t consumedAreaSize () const {
	    return m_pCPoint - m_pOrigin;
	}
	bool consumedAreaIsEmpty () const {
	    return m_pCPoint <= m_pOrigin;
	}
	bool consumedAreaIsntEmpty () const {
	    return m_pCPoint > m_pOrigin;
	}

	char const *consumerArea () const {
	    return m_pCPoint;
	}
	size_t consumerAreaSize () const {
	    return m_pPPoint - m_pCPoint;
	}
	bool consumerAreaIsEmpty () const {
	    return m_pPPoint <= m_pCPoint;
	}
	bool consumerAreaIsntEmpty () const {
	    return m_pPPoint > m_pCPoint;
	}

	char *producerArea () const {
	    return m_pPPoint;
	}
	size_t producerAreaSize () const {
	    return m_pExtent - m_pPPoint;
	}
	bool producerAreaIsEmpty () const {
	    return m_pExtent <= m_pPPoint;
	}
	bool producerAreaIsntEmpty () const {
	    return m_pExtent > m_pPPoint;
	}

    //  Area Management and Data Movement
    public:
	size_t consume (size_t sBlock) {
	    if (sBlock > consumerAreaSize ())
		sBlock = consumerAreaSize ();
	    m_pCPoint += sBlock;
	    return sBlock;
	}
	size_t produce (size_t sBlock) {
	    if (sBlock > producerAreaSize ())
		sBlock = producerAreaSize ();
	    m_pPPoint += sBlock;
	    return sBlock;
	}

    private:
	static size_t move (char *&rpDataDst, char const *&rpDataSrc, size_t sData);

    public:
	size_t moveDataFrom (VFifoLite &rProducer);
	size_t moveDataTo   (VFifoLite &rConsumer);

    //  State
    protected:
	// (*) m_pOrigin <= m_pCPoint <= m_pPPoint <= m_pExtent
	char const *m_pOrigin;	// Consumed origin (*)
	char const *m_pCPoint;	// Consumer origin (*)
	char       *m_pPPoint;	// Producer origin (*)
	char const *m_pExtent;	// Producer limit  (*)
    };
}


#endif
