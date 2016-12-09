#ifndef VPath_Interface
#define VPath_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VString.h"
#include "VkUUID.h"

/*************************
 *****  Definitions  *****
 *************************/

class VPath : public VReferenceable {
    DECLARE_CONCRETE_RTT (VPath, VReferenceable);

public:
    class VPathPeer : public VReferenceable {
        DECLARE_CONCRETE_RTT (VPathPeer, VReferenceable);

        friend class VPath;

    // Construction
    public:
        VPathPeer (uuid_t const &rOther) : m_iPeer (rOther) {
        }

        VPathPeer (uuid_t const &rOther1, uuid_t const &rOther2)
            :m_iPeer(rOther1) {
            m_pNextPeer.setTo(new VPathPeer(rOther2));
        }

        VPathPeer(VPathPeer const &rOther) :m_iPeer(rOther.uuid()){
            if(rOther.m_pNextPeer.isntNil()) 
                m_pNextPeer.setTo(new VPathPeer(*rOther.m_pNextPeer));
        }

    // Destruction
    private:
        ~VPathPeer() {
        }
    
    // Access
    public:
        VPathPeer *nextPeer() const {
            return m_pNextPeer;
        }

        VPathPeer *headPeer()  {
            return this;
        }

        VPathPeer *tailPeer() {
	    return m_pNextPeer.isNil() ? this : m_pNextPeer->tailPeer();
        }

        uuid_t const &uuid() const {
            return m_iPeer;
        }
    
        void display() const {
            VString string;
            VkUUIDHolder uuid(m_iPeer);
	    uuid.GetString(string);
        
            printf("%c",(string.content ())[0]);
            if(m_pNextPeer.isntNil())
                m_pNextPeer->display();
        }

    // State
    private:
        uuid_t const m_iPeer;
        VReference<VPathPeer> m_pNextPeer;
    };

public:
    class PeerPairHolder {
    // Construction
    public:
        PeerPairHolder(VReference<VPathPeer> pFirstPeer, VReference<VPathPeer> pSecondPeer)
            :m_pFirstPeer(pFirstPeer), m_pSecondPeer(pSecondPeer) {
        }
    
    // Destructor
    public:
        ~PeerPairHolder() {
        }

    // Assignment operator
    public:
        PeerPairHolder &operator= (PeerPairHolder const &rOther) {
            if( &rOther != this) {
                m_pFirstPeer.setTo(rOther.m_pFirstPeer);
                m_pSecondPeer.setTo(rOther.m_pSecondPeer);
            }
            return *this;
        }

    // State
    private:
        VReference<VPathPeer> m_pFirstPeer;
        VReference<VPathPeer> m_pSecondPeer;
    };

//typedef VkDynamicArrayOf<PeerPairHolder> PeerPairArray ;

// Construction
public:
    VPath () {
    }
     
    VPath (VPathPeer const &pSourcePeer, VPathPeer const &pDestPeer)
        :m_pSource(new VPathPeer(pSourcePeer)) ,
         m_pDestination(new VPathPeer(pDestPeer)) {
    }

    VPath (VPathPeer const &pHeadPeer) 
        :m_pHeadPeer(new VPathPeer(pHeadPeer)) {
    }

    VPath (VPath const &rOther) {
        if(rOther.m_pHeadPeer.isntNil()) {
            m_pHeadPeer.setTo(new VPathPeer(*rOther.m_pHeadPeer));
            if(rOther.m_pNextPath.isntNil())
                m_pNextPath.setTo(new VPath(*rOther.m_pNextPath));
            }
    }

// Destruction
private:
    ~VPath () {
    }

// Access
public:
    VPath *headPath () {
	return m_pHeadPeer.isNil() ? NilOf (VPath*) : this;
    }

    VPath *tailPath()  {
	return m_pHeadPeer.isNil() ? NilOf (VPath*) : m_pNextPath.isNil() ? this : m_pNextPath->tailPath();
    }

    VPathPeer *headPeer() const {
        return m_pHeadPeer.isNil() ? NilOf (VPathPeer*) : m_pHeadPeer.referent ();
    }

    VPathPeer *tailPeer() const {
        return m_pHeadPeer.isNil() ? NilOf (VPathPeer*) : m_pHeadPeer->tailPeer();
    }

    bool isEmpty() const {
        return m_pHeadPeer.isNil();
    }

    VPath *nextPath() const {
        return m_pNextPath;
    }

    void setSourcePeer(VPathPeer *pPeer) {
        m_pSource.setTo(pPeer);
    }

    void setDestinationPeer(VPathPeer *pPeer) {
        m_pDestination.setTo(pPeer);
    }

    // appending a path or path list before the current path list
    void appendPath(VPath *pPath) {
        if(pPath) {
            if(m_pHeadPeer.isNil()) {
                m_pHeadPeer.setTo(pPath->m_pHeadPeer);
                m_pNextPath.setTo(pPath->m_pNextPath);
            }
            else {
                // to avoid append a list to itself (will create a loop)
                if(pPath->tailPath()!=this) {
                    m_pHeadPeer.setTo(pPath->m_pHeadPeer);
                    m_pNextPath.setTo(pPath->m_pNextPath);
                    pPath->tailPath()->m_pNextPath.setTo(this);
                }
            }
        }
    }

    VPathPeer *sourcePeer() const {
        return m_pSource;
    }

    VPathPeer *destinationPeer() const {
        return m_pDestination;
    }
        
    void print() const {
        if(m_pHeadPeer.isNil())
             return;
        m_pHeadPeer->display();
        if(m_pNextPath.isntNil()) {
            printf(", ");
            m_pNextPath->print();
        }
            
    }
    void display() const {
        if(m_pHeadPeer.isNil())
            printf("(E)");
        else {
            printf("(");
            print();
            printf(")");
        }
    }

public :
    void concatenate(VPath *);
    void summation  (VPath *);

// State
private:
    VReference<VPathPeer> m_pHeadPeer;
    VReference<VPath>     m_pNextPath;
    
    VReference<VPathPeer> m_pSource;
    VReference<VPathPeer> m_pDestination;
};

#endif
