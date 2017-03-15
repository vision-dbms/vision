#ifndef VBlob_Interface
#define VBlob_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"
#include "V_VCOS.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VBlob : public VTransient {
    //  Family
	DECLARE_FAMILY_MEMBERS (VBlob, VTransient);

    //  Construction
    public:
	VBlob (void const *pData, size_t sData, bool bCopy = true);
	VBlob (void       *pVoid, size_t sVoid, bool bCopy = true);

	VBlob (size_t sStorage);

	VBlob (VBlob const &rOther);

	VBlob ();

    //  Destruction
    public:
	~VBlob () {
	}

    //  Access
    public:
	pointer_t contentArea () {
	    return m_iStorage.storage () + m_xLowerBound;
	}
	char const *contentArea () const {
	    return m_iStorage.storage () + m_xLowerBound;
	}
	size_t contentSize () const {
	    return m_xUpperBound - m_xLowerBound;
	}

	pointer_t preContentArea () {
	    return m_iStorage.storage ();
	}
	char const *preContentArea () const {
	    return m_iStorage.storage ();
	}
	size_t preContentSize () const {
	    return m_xLowerBound;
	}

	pointer_t postContentArea () {
	    return m_iStorage.storage () + m_xUpperBound;
	}
	char const *postContentArea () const {
	    return m_iStorage.storage () + m_xUpperBound;
	}
	size_t postContentSize () const {
	    return m_iStorage.storageSize () - m_xUpperBound;
	}

	pointer_t storage (size_t sContent) {
	    return m_iStorage.storage (sContent);
	}
	pointer_t storage () {
	    return m_iStorage.storage ();
	}
	char const *storage () const {
	    return m_iStorage.storage ();
	}

	char const *storageLimit (size_t sContent) {
	    return m_iStorage.storageLimit (sContent);
	}
	char const *storageLimit () {
	    return m_iStorage.storageLimit ();
	}
	char const *storageLimit () const {
	    return m_iStorage.storageLimit ();
	}

	size_t storageSize () const {
	    return m_iStorage.storageSize ();
	}

    //  Query
    public:
	bool storageIsPrivate () const {
	    return m_iStorage.storageIsPrivate ();
	}
	bool storageIsShared () const {
	    return m_iStorage.storageIsShared ();
	}

    //  Update
    public:
	void clear ();

	void append (void const *pContent, size_t sContent);
	void append (VBlob const &rOther) {
	    append (rOther.contentArea (), rOther.contentSize ());
	}

	//  The following return the size of the actual adjustment, ...
	size_t incrementContentLowerBoundBy (size_t sAdjustment);
	size_t incrementContentUpperBoundBy (size_t sAdjustment);

	void setTo (void *pStorage, size_t sStorage, bool bCopy = false);
	void setTo (size_t sStorage, bool bExact = true);
	void setTo (VBlob const &rOther);

	void setContentBoundsTo (size_t xLowerBound, size_t xUpperBound);
	void setContentLowerBoundTo (size_t xBound);
	void setContentUpperBoundTo (size_t xBound);
	void setContentSizeTo (size_t sContent) {
	    setContentUpperBoundTo (m_xLowerBound + sContent);
	}

	ThisClass &operator= (VBlob const &rOther) {
	    setTo (rOther);
	    return *this;
	}

    //  State
    protected:
	VCOS	m_iStorage;
	size_t	m_xLowerBound;
	size_t	m_xUpperBound;
    };
}


#endif
