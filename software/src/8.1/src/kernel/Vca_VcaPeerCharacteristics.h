#ifndef Vca_VcaPeerCharacteristics_Interface
#define Vca_VcaPeerCharacteristics_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"
#include "VkUUID.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VcaPeerCharacteristics {
    //  Architecture
    public:
	enum Architecture {
	    Architecture_Unknown,
	    Architecture_Big,
	    Architecture_Little,
	    Architecture_FirstUnreasonable,	//  MUST FOLLOW LAST VALID ARCHITECTURE CODE

#   if Vk_DataFormatNative == Vk_DataFormatLEndian
	    Architecture_Here = Architecture_Little
# elif Vk_DataFormatNative == Vk_DataFormatBEndian
	    Architecture_Here = Architecture_Big
#endif
	};

    //  Feature Level
    public:
	enum FeatureLevel {
	    FeatureLevel_Primary = 1,
	    FeatureLevel_Weak,

	    FeatureLevel_Minimum = FeatureLevel_Primary,
	    FeatureLevel_Maximum = FeatureLevel_Weak
	};

    //  Version Range
    public:
	class Vca_API VersionRange {

	//  Version
	public:
	    class Vca_API Version {
	    //  Construction
	    public:
		Version (Version const &rOther);
		Version (FeatureLevel xFeatureLevel);

	    // Assignment
	    public:
		Version &operator= (Version const &rOther);

	    //  Access
	    public:
		operator FeatureLevel () const {
		    return featureLevel ();
		}
		FeatureLevel featureLevel () const {
		    return static_cast<FeatureLevel>(m_xFeatureLevel);
		}
		U16 unused () const {
		    return m_iUnused;
		}

	    //  Query
	    public:
		bool supports (FeatureLevel xFeatureLevel) const {
		    return m_xFeatureLevel >= xFeatureLevel;
		}

	    //  Byte Order Reversal
	    public:
		void reverseByteOrder ();

	    //  State
	    protected:
		U16 m_xFeatureLevel;
		U16 m_iUnused;
	    };

	//  Construction
	public:
	    VersionRange (VersionRange const &rOther);
	    VersionRange ();

	//  Assignment
	public:
	    VersionRange &operator= (VersionRange const &rOther);

	//  Access
	public:
	    Version const &maximum () const {
		return m_iMaximum;
	    }
	    Version const &minimum () const {
		return m_iMinimum;
	    }

	//  Query
	public:
	    bool supports (FeatureLevel xFeatureLevel) const {
		return m_iMaximum.supports (xFeatureLevel);
	    }

	//  Byte Order Reversal
	public:
	    void reverseByteOrder ();

	//  State
	protected:
	    Version m_iMaximum;
	    Version m_iMinimum;
	};

    //  Construction
    public:
	VcaPeerCharacteristics (VcaPeerCharacteristics const &rOther);
	VcaPeerCharacteristics ();

    //  Access
    public:
	Architecture architecture () const {
	    return (Architecture)m_xArchitecture;
	}
	unsigned int unused1 () const {
	    return m_iUnused1;
	}
	VersionRange versionRange () const {
	    return m_iVersionRange;
	}
	uuid_t const &uuid () const {
	    return m_iUUID;
	}

	operator uuid_t const &() const {
	    return m_iUUID;
	}

    //  Query
    public:
	bool represents (uuid_t const &rUUID) const;

	bool supportsWeak () const {
	    return supports (FeatureLevel_Weak);
	}
	bool supports (FeatureLevel xFeatureLevel) const {
	    return m_iVersionRange.supports (xFeatureLevel);
	}

    // Assignment operator
    public:
	VcaPeerCharacteristics &operator= (VcaPeerCharacteristics const &rOther) {
	    if(&rOther != this) {
		m_iUUID         = rOther.uuid ();
		m_xArchitecture = rOther.architecture ();
		m_iUnused1      = rOther.unused1 ();
		m_iVersionRange = rOther.versionRange ();
	    }   
	    return *this;
	}

    //  Comparison (used by VkSetOfFactoryReferences template)
    public:
	bool operator <  (uuid_t const &rUUID) const {
	    return VkUUID::lt (m_iUUID, rUUID);
	}
	bool operator <= (uuid_t const &rUUID) const {
	    return VkUUID::le (m_iUUID, rUUID);
	}
	bool operator == (uuid_t const &rUUID) const {
	    return VkUUID::eq (m_iUUID, rUUID);
	}
	bool operator != (uuid_t const &rUUID) const {
	    return VkUUID::ne (m_iUUID, rUUID);
	}
	bool operator >= (uuid_t const &rUUID) const {
	    return VkUUID::ge (m_iUUID, rUUID);
	}
	bool operator >  (uuid_t const &rUUID) const {
	    return VkUUID::gt (m_iUUID, rUUID);
	}

	bool operator < (VcaPeerCharacteristics const &rOther) const {
	    return rOther > m_iUUID;
	}
	bool operator <= (VcaPeerCharacteristics const &rOther) const {
	    return rOther >= m_iUUID;
	}
	bool operator == (VcaPeerCharacteristics const &rOther) const {
	    return rOther == m_iUUID;
	}
	bool operator != (VcaPeerCharacteristics const &rOther) const {
	    return rOther != m_iUUID;
	}
	bool operator >= (VcaPeerCharacteristics const &rOther) const {
	    return rOther <= m_iUUID;
	}
	bool operator > (VcaPeerCharacteristics const &rOther) const {
	    return rOther < m_iUUID;
	}

    //  Localization
    public:
	bool architectureIsKnown () const {
	    return architecture () != Architecture_Unknown;
	}
	bool architectureIsLocal () const {
	    return architecture () == Architecture_Here;
	}
	bool architectureIsAntiLocal () const {
	    return architectureIsKnown () && architecture () != Architecture_Here;
	}
	bool architectureIsUnreasonable () const {
	    return m_xArchitecture >= (unsigned int)Architecture_FirstUnreasonable;
	}

	void localize (void *pByteArray, size_t sElement, size_t cElements) const;

	void localize (bool &rValue, size_t cElements) const {
	}

	void localize (S08 &rValue, size_t cElements) const {
	}
	void localize (U08 &rValue, size_t cElements) const {
	}

	void localize (S16 &rValue, size_t cElements) const {
	    localize (&rValue, sizeof (rValue), cElements);
	}
	void localize (U16 &rValue, size_t cElements) const {
	    localize (&rValue, sizeof (rValue), cElements);
	}

	void localize (S32 &rValue, size_t cElements) const {
	    localize (&rValue, sizeof (rValue), cElements);
	}
	void localize (U32 &rValue, size_t cElements) const {
	    localize (&rValue, sizeof (rValue), cElements);
	}

	void localize (S64 &rValue, size_t cElements) const {
	    localize (&rValue, sizeof (rValue), cElements);
	}
	void localize (U64 &rValue, size_t cElements) const {
	    localize (&rValue, sizeof (rValue), cElements);
	}

	void localize (F32 &rValue, size_t cElements) const {
	    localize (&rValue, sizeof (rValue), cElements);
	}
	void localize (F64 &rValue, size_t cElements) const {
	    localize (&rValue, sizeof (rValue), cElements);
	}

	void localize (VkDynamicArrayOf<bool> &rValue) const {
	}
    
	void localize (VkDynamicArrayOf<S08> &rValue) const {
	}
	void localize (VkDynamicArrayOf<U08> &rValue) const {
	}

	void localize (VkDynamicArrayOf<S16> &rValue) const {
	    localize (rValue[0], rValue.elementCount ());
	}
	void localize (VkDynamicArrayOf<U16> &rValue) const {
	    localize (rValue[0], rValue.elementCount ());
	}

	void localize (VkDynamicArrayOf<S32> &rValue) const {
	    localize (rValue[0], rValue.elementCount ());
	}
	void localize (VkDynamicArrayOf<U32> &rValue) const {
	    localize (rValue[0], rValue.elementCount ());
	}

	void localize (VkDynamicArrayOf<S64> &rValue) const {
	    localize (rValue[0], rValue.elementCount ());
	}
	void localize (VkDynamicArrayOf<U64> &rValue) const {
	    localize (rValue[0], rValue.elementCount ());
	}

	void localize (VkDynamicArrayOf<F32> &rValue) const {
	    localize (rValue[0], rValue.elementCount ());
	}
	void localize (VkDynamicArrayOf<F64> &rValue) const {
	    localize (rValue[0], rValue.elementCount ());
	}

	void localize (VcaPeerCharacteristics &rValue) const;
	void localize (SSID &rValue, size_t cElements) const {
	    localize (&rValue, sizeof (rValue), cElements);
	}
	void localize (uuid_t &rValue, size_t cElements) const;

	void localize () {
	    localize (*this);
	}

	void reverseByteOrder ();

    //  State
    protected:
	unsigned int	m_xArchitecture	: 8;
	unsigned int	m_iUnused1	: 24;
	VersionRange	m_iVersionRange;
	uuid_t		m_iUUID;
    };
}


#endif 
