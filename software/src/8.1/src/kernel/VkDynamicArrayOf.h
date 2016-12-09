#ifndef VkDynamicArrayOf_Interface
#define VkDynamicArrayOf_Interface

/************************
 *****  Components  *****
 ************************/

#include "VkArrayOf.h"

#include "V_VCOS.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

/*-------------------------------*
 *----  Allocation Policies  ----*
 *-------------------------------*/
namespace V {
    namespace Policy {

    /*<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>*
     *>>>>  Storage Policy Base  <<<<*
     *<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>*/

	struct StoragePolicyBase {
	    DECLARE_FAMILY_MEMBERS (StoragePolicyBase,void);

	//  Aliases
	public:
	    typedef VkArray::element_count_t element_count_t;
	    typedef VkArray::element_index_t element_index_t;
	};


    /*<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>><*
     *>>>>  Default Storage Policy  <<<<*
     *<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>><*/

	template <typename T> struct StoragePolicy : public StoragePolicyBase {
	    DECLARE_FAMILY_MEMBERS (StoragePolicy,StoragePolicyBase);

	//  Element Storage
	public:
	    class ElementStorage {
		DECLARE_FAMILY_MEMBERS (ElementStorage,void);

	    //  Aliases
	    public:
		typedef T element_t;

	    //  Memory Management
	    private:
		static T *allocateStorage (element_count_t cElements) {
		    return cElements > 1 ? new T[cElements] : cElements > 0 ? new T : 0;
		}
		INLINE_TMEMBER T *duplicateStorage (element_count_t cElements) const;
		INLINE_TMEMBER void deallocateStorage (element_count_t &rcElements);

	    //  Construction
	    public:
		ElementStorage (
		    element_count_t cElements, ThisClass const &rOther
		) : m_pStorage (rOther.duplicateStorage (cElements)) {
		}
		ElementStorage (element_count_t cElements) : m_pStorage (allocateStorage (cElements)) {
		}

	    //  Destruction
	    public:
		~ElementStorage () {
		}
		void destroy (element_count_t cElements) {
		    deallocateStorage (cElements);
		}

	    //  Access
	    public:
		element_t const *elementArray () const {
		    return m_pStorage;
		}
		element_t const *element (element_count_t cElements, element_index_t xElement) const {
		    return xElement < cElements ? elementArray () + xElement : 0;
		}

		element_t *elementArray () {
		    return m_pStorage;
		}
		element_t *element (element_count_t cElements, element_index_t xElement) {
		    return xElement < cElements ? elementArray () + xElement : 0;
		}

	    //  Update
	    public:
		INLINE_TMEMBER void insert (element_count_t &rcElements, element_index_t xEdit, element_count_t sEdit);
		INLINE_TMEMBER void remove (element_count_t &rcElements, element_index_t xEdit, element_count_t sEdit);

		void removeAll (element_count_t &rcElements) {
		    deallocateStorage (rcElements);
		}

	    //  State
	    private:
		T *m_pStorage;
	    };  // class ElementStorage
	}; // template <typename T> class StoragePolicy<T>


	/*****************************************************************/

	template <typename T> T *StoragePolicy<T>::ElementStorage::duplicateStorage (element_count_t cElements) const {
	    T *const pNewStorage = allocateStorage (cElements);
	    if (pNewStorage && m_pStorage) {
		for (element_index_t xElement = 0; xElement < cElements; xElement++)
		    pNewStorage[xElement] = m_pStorage[xElement];
	    }
	    return pNewStorage;
	}

	template <typename T> void StoragePolicy<T>::ElementStorage::deallocateStorage (element_count_t &rcElements) {
	    if (m_pStorage) {
		if (rcElements > 1)
		    delete[] m_pStorage;
		else
		    delete m_pStorage;
		m_pStorage = 0;
	    }
	    rcElements = 0;
	}

	/*****************************************************************/

	template <typename T> void StoragePolicy<T>::ElementStorage::insert (
	    element_count_t &rcElements, element_index_t xEdit, element_count_t sEdit
	) {
	//  Adjust the parameters if the insertion origin is past the current end of the array...
	    if (xEdit > rcElements) {
		sEdit += xEdit - rcElements;
		xEdit = rcElements;
	    }

	//  ... and reallocate and install the new element array if necessary:
	    if (sEdit > 0) {
		element_t* pNewStorage = allocateStorage (rcElements + sEdit);

		element_index_t xTargetElement = 0;
		element_index_t xSourceElement = 0;

		while (xSourceElement < xEdit)
		    pNewStorage[xTargetElement++] = m_pStorage[xSourceElement++];

		xTargetElement += sEdit;

		while (xSourceElement < rcElements)
		    pNewStorage[xTargetElement++] = m_pStorage[xSourceElement++];

		deallocateStorage (rcElements);
		m_pStorage = pNewStorage;
		rcElements = xTargetElement;
	    }
	}

	/*****************************************************************/

	template <typename T> void StoragePolicy<T>::ElementStorage::remove (
	    element_count_t &rcElements, element_index_t xEdit, element_count_t sEdit
	) {
	//  Adjust the parameters if the deletion region extends past the end of the array...
	    if (xEdit >= rcElements)
		return;

	    if (xEdit + sEdit > rcElements)
		sEdit -= xEdit + sEdit - rcElements;

	//  ... and reallocate and install the new element array if there is work to do:
	    if (sEdit > 0) {
		element_t* pNewStorage = allocateStorage (rcElements - sEdit);

		element_index_t xTargetElement = 0;
		element_index_t xSourceElement = 0;

		while (xSourceElement < xEdit)
		    pNewStorage[xTargetElement++] = m_pStorage[xSourceElement++];

		xSourceElement += sEdit;

		while (xSourceElement < rcElements)
		    pNewStorage[xTargetElement++] = m_pStorage[xSourceElement++];

		deallocateStorage (rcElements);
		m_pStorage = pNewStorage;

		rcElements = xTargetElement;
	    }
	}


    /*<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>*
     *>>>>  POD Storage Policy  <<<<*
     *<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>*/

	template <typename T> struct PODStoragePolicy : public StoragePolicyBase {
	    DECLARE_FAMILY_MEMBERS (PODStoragePolicy,StoragePolicyBase);

	//  Element Storage
	public:
	    class ElementStorage {
		DECLARE_FAMILY_MEMBERS (ElementStorage,void);

	    //  Aliases
	    public:
		typedef T element_t;

	    //  Construction
	    public:
		ElementStorage (
		    element_count_t cElements, ThisClass const &rOther
		) : m_iStorage (rOther.m_iStorage) {
		}
		ElementStorage (element_count_t cElements) : m_iStorage (cElements * sizeof (element_t)) {
		}

	    //  Destruction
	    public:
		~ElementStorage () {
		}
		void destroy (element_count_t cElements) {
		//  No need to do anything because V::VCOS::~VCOS does it all for less !!!
		}

	    //  Access
	    public:
		element_t const *elementArray () const {
		    return reinterpret_cast<element_t const*>(m_iStorage.storage ());
		}
		element_t const *element (element_count_t cElements, element_index_t xElement) const {
		    return xElement < cElements ? elementArray () + xElement : 0;
		}

		element_t *elementArray () {
		    return reinterpret_cast<element_t*>(m_iStorage.storage ());
		}
		element_t *element (element_count_t cElements, element_index_t xElement) {
		    return xElement < cElements ? elementArray () + xElement : 0;
		}

	    //  Update
	    private:
		element_t *storage (element_count_t cElements) {
		    return reinterpret_cast<element_t*>(m_iStorage.storage (cElements * sizeof (element_t)));
		}
	    public:
		INLINE_TMEMBER void insert (element_count_t &rcElements, element_index_t xEdit, element_count_t cElements);
		INLINE_TMEMBER void remove (element_count_t &rcElements, element_index_t xEdit, element_count_t cElements);

		void removeAll (element_count_t &rcElements) {
		    m_iStorage.trim (rcElements = 0);
		}

	    //  State
	    private:
		V::VCOS m_iStorage;
	    };  // class ElementStorage
	}; // template <typename T> class PODStoragePolicy


	/*****************************************************************/

	template <typename T> void PODStoragePolicy<T>::ElementStorage::insert (
	    element_count_t &rcElements, element_index_t xEdit, element_count_t sEdit
	) {
	//  Adjust the parameters if the insertion origin is past the current end of the array...
	    if (xEdit > rcElements) {
		sEdit += xEdit - rcElements;
		xEdit = rcElements;
	    }

	//  ... and insert the requested elements if necessary:
	    if (sEdit > 0) {
		T *const pStorage = storage (rcElements + sEdit);
		memmove (
		    pStorage + xEdit + sEdit,				// move destination
		    pStorage + xEdit,					// move source
		    (rcElements - xEdit) * sizeof (element_t)		// move size in bytes
		);
		rcElements += sEdit;
	    }
	}

	/*****************************************************************/


	template <typename T> void PODStoragePolicy<T>::ElementStorage::remove (
	    element_count_t &rcElements, element_index_t xEdit, element_count_t sEdit
	) {
	//  Adjust the parameters if the deletion region extends past the end of the array...
	    if (xEdit >= rcElements)
	    return;

	    if (xEdit + sEdit > rcElements)
		sEdit -= xEdit + sEdit - rcElements;

	//  ... and remove the requested elements if necessary:
	    if (sEdit > 0) {
		T *const pStorage = storage (rcElements - sEdit);
		memmove (
		    pStorage + xEdit,					// move destination
		    pStorage + xEdit + sEdit,				// move source
		    (rcElements - xEdit - sEdit) * sizeof (element_t)	// move size in bytes
		);
		rcElements -= sEdit;
	    }
	}


    /*<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>*
     *>>>>  Storage Policy Specializations  <<<<*
     *<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>*/

	template <> struct StoragePolicy<bool> : public PODStoragePolicy<bool> {
	};
	template <> struct StoragePolicy<char> : public PODStoragePolicy<char> {
	};
	template <> struct StoragePolicy<unsigned char> : public PODStoragePolicy<unsigned char> {
	};
	template <> struct StoragePolicy<short> : public PODStoragePolicy<short> {
	};
	template <> struct StoragePolicy<unsigned short> : public PODStoragePolicy<unsigned short> {
	};
	template <> struct StoragePolicy<int> : public PODStoragePolicy<int> {
	};
	template <> struct StoragePolicy<unsigned int> : public PODStoragePolicy<unsigned int> {
	};
	template <> struct StoragePolicy<__int64> : public PODStoragePolicy<__int64> {
	};
	template <> struct StoragePolicy<unsigned __int64> : public PODStoragePolicy<unsigned __int64> {
	};
	template <> struct StoragePolicy<float> : public PODStoragePolicy<float> {
	};
	template <> struct StoragePolicy<double> : public PODStoragePolicy<double> {
	};

    } // namespace Policy
} // namespace V


/*----------------------------*
 *----  VkDynamicArrayOf  ----*
 *----------------------------*/

template <class ElementType> class VkDynamicArrayOf : public V::VkArrayOf<typename V::Policy::StoragePolicy<ElementType>::ElementStorage> {
    DECLARE_FAMILY_MEMBERS (VkDynamicArrayOf<ElementType>, V::VkArrayOf<typename V::Policy::StoragePolicy<ElementType>::ElementStorage>);

//  Aliases
public:
    typedef typename BaseClass::element_count_t element_count_t;
    typedef typename BaseClass::element_index_t element_index_t;

//  Construction
public:
    VkDynamicArrayOf (ThisClass const &rOther) : BaseClass (rOther) {
    }
    VkDynamicArrayOf (element_count_t cElements = 0) : BaseClass (cElements) {
    }

//  Destruction
public:
    ~VkDynamicArrayOf () {
	m_iStorage.destroy (m_cElements);  //  ... the inexpensive alternative to 'removeAll'
    }

//  Element Insertion and Deletion
public:
    void Insert (element_index_t xEdit, element_count_t sEdit) {
	m_iStorage.insert (m_cElements, xEdit, sEdit);
    }

    void Append (element_count_t sEdit) {
	Insert (m_cElements, sEdit);
    }
    void Prepend (element_count_t sEdit) {
	Insert (0, sEdit);
    }

    void Guarantee (element_count_t cElements) {
	if (cElements > m_cElements)
	    Append (cElements - m_cElements);
    }

    void Delete (element_index_t xEdit, element_count_t sEdit = 1) {
	m_iStorage.remove (m_cElements, xEdit, sEdit);
    }

    void DeleteAll () {
	m_iStorage.removeAll (m_cElements);
    }

//  State
protected:
    using BaseClass::m_iStorage;
    using BaseClass::m_cElements;
};


#endif
