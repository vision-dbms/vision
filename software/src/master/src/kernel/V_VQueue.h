#ifndef V_VQueue_Interface
#define V_VQueue_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

#include "V_VSpinlock.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

namespace V {
//  'typename Request' requires operator= and copy and default constructors.
    template<typename Request> class VQueue : public VTransient {
	DECLARE_FAMILY_MEMBERS (VQueue<Request>, VTransient);

    //  Aliases
    public:
	typedef VSpinlock	Mutex;
        typedef Mutex::Claim	MutexClaim;

    //  Entry
    public:
        class Iterator;
	class Entry : public VReferenceable {
	    DECLARE_CONCRETE_RTTLITE (Entry, VReferenceable);

	    friend class VQueue<Request>;
  	    friend class Iterator;

	//  Construction
	private:
	    Entry (Request const &rRequest) : m_iRequest (rRequest) {
	    }

	//  Destruction
	private:
	    ~Entry () {
	    }

	//  Access
	public:
	    Entry *pop (Request &rRequest) const {
		rRequest = m_iRequest;
		return m_pSuccessor;
	    }

	//  State
	private:
	    Request const	m_iRequest;
	    VReference<Entry>	m_pSuccessor;
	};
	friend class Entry;
      
    //  Iterator  
    public:
      class Iterator {

      //  Construction
      public:
	Iterator (VQueue<Request> const &rQueue) : m_rQueue (rQueue), m_xElement (0) {
	}

      //  Destruction
      public:
	~Iterator () {
	}

      //  Query
      public:
	bool isNotAtEnd () const {
	  return m_xElement < m_rQueue.size ();
	}
	bool isAtEnd () const {
	  return !isNotAtEnd ();
	}
	bool current (Request &rRequest) {
	  bool bRetVal = false;

	  if (isNotAtEnd ()) {
	    if (m_xElement == 0) {
	      rRequest = m_rQueue.m_iActiveRequest;
	      bRetVal = true;
	    }
	    else {
	      unsigned int xIndex=1;
	      VReference<Entry> pEntry (m_rQueue.m_pHead);
	      while (pEntry && xIndex++ < m_xElement)
		pEntry.setTo (pEntry->m_pSuccessor);
	      if (pEntry) {
		pEntry->pop (rRequest);
		bRetVal = true;
	      }
	    }
	  }
	  return bRetVal;
	}

      //  Access
      public:
	unsigned int index () const {
	  return m_xElement;
	}

	Iterator& operator++ () {
	  if (m_xElement < m_rQueue.size ())
	    m_xElement++;
	  return *this;
	}

	
      protected:
	VQueue<Request>  const &m_rQueue;
	unsigned int            m_xElement;
      };
      friend class Iterator;

    //  Construction
    public:
	VQueue () : m_bActiveRequest (false), m_sQueue (0) {
	}


    //  Destruction
    public:
	~VQueue () {
	}

    //  Access/Query
    public:
	bool active () const {
	    return m_bActiveRequest;
	}
	Request const &activeRequest () const {
	    return m_iActiveRequest;
	}
	Request &activeRequest () {
	    return m_iActiveRequest;
	}
        size_t size () const {
	  return m_sQueue;
        }

    //  Queue Management
    private:
	void enqueue (Entry *pEntry) {
	    if (m_pTail)
		m_pTail->m_pSuccessor.setTo (pEntry);
	    else
		m_pHead.setTo (pEntry);
	    m_pTail.setTo (pEntry);
	}
    public:
	bool enqueue (Request const &rRequest) {
	    MutexClaim iMutexClaim (m_iMutex);
	    bool bFirst = false;
	    if (m_bActiveRequest) {
		enqueue (new Entry (rRequest));
	    }
	    else {
		m_iActiveRequest = rRequest;		m_bActiveRequest = true;
		bFirst = true;
	    }
	    m_sQueue++;
	    return bFirst;		//  ... true if first, false if active
	}

	bool dequeue (Request &rRequest) {
	    MutexClaim iMutexClaim (m_iMutex);
	    bool bValidRequest = m_bActiveRequest;
	    if (bValidRequest) {
		rRequest = m_iActiveRequest;
		m_bActiveRequest = m_pHead.isntNil ();
		if (!m_bActiveRequest)
		    m_iActiveRequest.clear ();
		else {
		    m_pHead.setTo (m_pHead->pop (m_iActiveRequest));
		    if (m_pHead.isNil ())
			m_pTail.clear ();
		}
		m_sQueue--;
	    }
	    return bValidRequest;	//  ... true if a request was dequeued
	}

	bool dequeue () {
	    MutexClaim iMutexClaim (m_iMutex);
	    if (m_bActiveRequest) {
		m_bActiveRequest = m_pHead.isntNil ();
		if (!m_bActiveRequest)
		    m_iActiveRequest.clear ();
		else {
		    m_pHead.setTo (m_pHead->pop (m_iActiveRequest));
		    if (m_pHead.isNil ())
			m_pTail.clear ();
		}
		m_sQueue--;
	    }
	    return m_bActiveRequest;	//  ... true if a request remains active
	}

    //  State
    private:
	Mutex			m_iMutex;
	VReference<Entry>	m_pHead;
	VReference<Entry>	m_pTail;
	Request			m_iActiveRequest;
	bool			m_bActiveRequest;
        size_t                  m_sQueue;     
    };
}


#endif
