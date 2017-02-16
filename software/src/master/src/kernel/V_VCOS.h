#ifndef VCOS_Interface
#define VCOS_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VCOS : public VTransient {
    //  Family
	DECLARE_FAMILY_MEMBERS (VCOS, VTransient);

    //  Ownership Token
    public:
	class V_API OwnershipToken : public VReferenceable {
	//  Run Time Type
	    DECLARE_CONCRETE_RTT(OwnershipToken, VReferenceable);

	//  Construction
	public:
	    OwnershipToken () {
	    }

	//  Destruction
	protected:
	    ~OwnershipToken () {
	    }
	};
	typedef VReference<OwnershipToken> TokenReference;

    //  Storage Management
    private:
	void deallocateStorage ();

    public:
	void guarantee (size_t sStorage, bool bExact = false);
	void trim (size_t sStorage);

    //  Construction
    public:
	VCOS (void *pStorage, size_t sStorage, bool bCopy = true);
	VCOS (size_t sStorage);
	VCOS (VCOS const &rOther);
	VCOS ();

    //  Destruction
    public:
	~VCOS () {
	    deallocateStorage ();
	}

    //  Access
    public:
	pointer_t storage (size_t sStorage) {
	    guarantee (sStorage);
	    return m_pStorage;
	}
	pointer_t storage () {
	    guarantee (m_sStorage);
	    return m_pStorage;
	}
	char const *storage () const {
	    return m_pStorage;
	}

	char const *storageLimit (size_t sStorage) {
	    guarantee (sStorage);
	    return m_pStorage + m_sStorage;
	}
	char const *storageLimit () {
	    guarantee (m_sStorage);
	    return m_pStorage + m_sStorage;
	}
	char const *storageLimit () const {
	    return m_pStorage + m_sStorage;
	}

	size_t storageSize () const {
	    return m_sStorage;
	}

    //  Query
    public:
	bool storageIsPrivate () const {
	    return m_pStorageOwnershipToken.ownsReferent ();
	}
	bool storageIsShared () const {
	    return m_pStorageOwnershipToken.sharesReferent ();
	}

    //  Update
    public:
	void setTo (VCOS const &rOther);
	void setTo (void *pStorage, size_t sStorage, bool bCopy = false);

	ThisClass &operator= (VCOS const &rOther) {
	    setTo (rOther);
	    return *this;
	}

    //  State
    protected:
	TokenReference	m_pStorageOwnershipToken;
	pointer_t		m_pStorage;
	size_t		m_sStorage;
    };
}


#endif
