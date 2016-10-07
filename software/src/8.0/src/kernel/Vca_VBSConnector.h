#ifndef Vca_VBSConnector_Interface
#define Vca_VBSConnector_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/**************************
 *----  VBSConnector  ----*
 **************************/

namespace Vca {
    class VBSProducer;
    class VBSConsumer;

    class Vca_API VBSConnector : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VBSConnector, VRolePlayer);

    //  BSClients
    public:
	class BSClient;
	class BSProducerClient;
	class BSConsumerClient;

	friend class BSClient;

    //  Buffer
    public:
	class Buffer;

    //  BufferQueue
    public:
	class BufferQueue {
	//  Construction
	public:
	    BufferQueue ();

	//  Destruction
	public:
	    ~BufferQueue ();

	//  Query
	public:
	    bool isEmpty () const {
		return m_pHead.isNil ();
	    }
	    bool isntEmpty () const {
		return m_pHead.isntNil ();
	    }

	//  Use
	public:
	    void enqueue (VReference<Buffer>&rpBuffer);
	    bool dequeue (VReference<Buffer>&rpBuffer);

	//  State
	private:
	    VReference<Buffer> m_pHead;
	    VReference<Buffer> m_pTail;
	};

    //  Construction
    public:
	VBSConnector (VBSProducer *pProducer, VBSConsumer *pConsumer);

    //  Destruction
    private:
	~VBSConnector ();

    //  Buffer Management
    protected:
	bool acquireBuffer (VReference<Buffer>&rpBuffer);
	bool releaseBuffer (VReference<Buffer>&rpBuffer);

    //  State
    private:
	BufferQueue m_iBufferFreeList;
	size_t m_sBuffer;
    };
}


#endif
