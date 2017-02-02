#ifndef V_VLinkedQueue__Interface
#define V_VLinkedQueue__Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "V_VCount.h"
#include "V_VSpinlock.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
/*------------------------------------------------------------------*
 *---- MSVC6 won't accept the link member as a template parameter,
 *---- so it needs to be a const member of the class instead.
 *------------------------------------------------------------------*/
    /**
     * Thread-safe FIFO queue.
     */
    template <class E, class M = VSpinlock> class VLinkedQueue_ : public VTransient {
    public:
	typedef VLinkedQueue_<E,M> ThisClass;
	typedef VTransient BaseClass;

    //  Aliases
    public:
	typedef unsigned_int32_count_t	counter_t;
	typedef counter_t::value_t	count_t;

        /** Our queue element type. */
	typedef E			Element;
        /** Reference type for our queue elements. */
	typedef typename E::Reference	ElementReference;
        /** The element's link member, used to link elements together in this queue. */
	typedef ElementReference E::*	ElementLink;
        /** Mutex type that we'll use for thread safety. */
	typedef M			Mutex;
        /** Mutex claim type that we'll use for thread safety. */
	typedef typename M::Claim	MutexClaim;

    //  Construction
    public:
        /**
         * Constructor expects a pointer to a member belonging to E; said member must be of type E::Reference and will be used to link elements in this queue together.
         *
         * @param pLink pointer to the member used to link queue elements together.
         */
	VLinkedQueue_(ElementLink pLink) : m_pLink (pLink) {
	}

    //  Destruction
    public:
	~VLinkedQueue_() {
	}

    //  Access
    public:
        /** Number of elements enqueued in this queue over its lifetime. */
	count_t enqueueCount () const {
	    return m_cEnqueued;
	}
        /** Number of elements dequeued from this queue over its lifetime. If this is greater than enqueueCount(), check your wallet and pockets. */
	count_t dequeueCount () const {
	    return m_cDequeued;
	}
        /** The front of this queue. */
	ElementReference const &head () const {
	    return m_pHead;
	}

    //  Query
    public:
        /**
         * Used to determine if there are any elements in this queue.
         *
         * @return true if this queue has zero elements, false otherwise.
         */
	bool isEmpty () const {
	    return m_pHead.isNil ();
	}
        /**
         * Used to determine if there are any elements in this queue.
         *
         * @return true if this queue has elements, false otherwise.
         */
	bool isntEmpty () const {
	    return m_pHead.isntNil ();
	}

    //  Use
    public:
        /**
         * Adds an element to this queue.
         *
         * @param pElement the element to be enqueued.
         * @return true if the given element was inserted as the first element in the queue.
         */
	bool enqueue (Element *pElement) {	// returns true if the queue was empty.
	    bool bFirstElement = false;
	    if (pElement) {
		MutexClaim iLock (m_iMutex);

		m_cEnqueued.increment ();
		if (m_pTail)
		    (m_pTail.operator E*()->*m_pLink).setTo (pElement);
		else {
		    m_pHead.setTo (pElement);
		    bFirstElement = true;
		}
		m_pTail.setTo (pElement);
	    }
	    return bFirstElement;
	}

        /**
         * Used to dequeue the head of this queue.
         *
         * Both parameters and the return value are computed atomically so that the following usage is thread-safe as long as there aren't multiple queue consumers. The common design pattern used here is that the thread that is responsible for placing the first element in the queue should also be responsible for consuming the queue to completion while other threads may place elements in the queue as much as they like provided they never consume from it.
         *
         * @code
	 * if (m_iQueue.enqueue (pElement)) do { // first element
	 *     m_iQueue.head ()->doSomething ();
	 * } while (m_iQueue.dequeueAndTest ());
         * @endcode
         *
         * @param[out] rpElement the reference by which the dequeued element is returned.
         * @param[out] rbMore set to true if there are more elements in the queue after dequeueing has succeeded, false otherwise. Only meaningful if the return value from this method was true.
         * @return true if an object was successfully dequeued, false otherwise.
         */
	bool dequeue (ElementReference &rpElement, bool &rbMore) {
	    bool bElementValid = false;
	    MutexClaim iLock (m_iMutex);
	    if (m_pHead.isNil ())
		rpElement.clear ();
	    else {
		m_cDequeued.increment ();
		bElementValid = true;
		rpElement.setTo (m_pHead);
		m_pHead.claim (rpElement.operator E*()->*m_pLink);
		if (m_pHead.isntNil ())
		    rbMore = true;
		else {
		    rbMore = false;
		    m_pTail.clear ();
		}
	    }
	    return bElementValid;
	}
        /**
         * Overload for dequeue(ElementReference&, bool&) for when the consumer doesn't care about the second parameter.
         * @see dequeue(ElementReference&, bool&)
         */
	bool dequeue (ElementReference &rpElement) {
	    bool bMoreElements;
	    return dequeue (rpElement, bMoreElements);
	}
        /**
         * Cover method for dequeue(ElementReference&, bool&) that presumes that the first element in this queue has been processed as necessary and can be discarded.
         */
	bool dequeueAndTest () {	// returns true if more elements are in the queue.
	    ElementReference pElement; bool bMoreElements;
	    return dequeue (pElement, bMoreElements) && bMoreElements;
	}

    //  State
    private:
        /** The link member used to link elements in this queue. */
	ElementLink		m_pLink;
        /** The mutex used for thread safety. */
	Mutex			m_iMutex;
        /** The front of this queue. */
	ElementReference	m_pHead;
        /** The back of this queue. */
	ElementReference	m_pTail;
        /** Lifetime statistic for number of enqueued elements. */
	counter_t		m_cEnqueued;
        /** Lifetime statistic for number of dequeued elements. */
	counter_t		m_cDequeued;
    };
}


#endif
