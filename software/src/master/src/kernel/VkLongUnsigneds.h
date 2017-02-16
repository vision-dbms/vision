/*****  Long Unsigneds Interface  *****/
#ifndef VkLongUnsigneds_Interface
#define VkLongUnsigneds_Interface

/*********************
*****  Library  *****
*********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VFamilyValues.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
  /*******************************
   *----  VLongUnsignedBase  ----*
   *******************************/
  class V_API VLongUnsignedBase {
  public:
    typedef unsigned int grain_t;
    typedef unsigned int index_t;
  protected:
    static grain_t m_iDummy;
  };
}

/*******************************************************************
 *---- template <unsigned int sDatum> class VkUnsignedTemplate ----*
 *******************************************************************/
template <unsigned int sDatum> class VkUnsignedTemplate : public V::VLongUnsignedBase {
    DECLARE_FAMILY_MEMBERS (VkUnsignedTemplate<sDatum>, V::VLongUnsignedBase);

protected:
    grain_t m_i[sDatum];

public:
    static ThisClass const &Zero ();

    void SetToZero ();

    grain_t operator[] (index_t xElement) const {
	return xElement < sDatum
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
	    ? m_i[xElement]
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
	    ? m_i[sDatum - xElement - 1]
#endif
	    : 0;
    }

    grain_t& operator[] (index_t xElement) {
	return xElement < sDatum
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
	    ? m_i[xElement]
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
	    ? m_i[sDatum - xElement - 1]
#endif
	    : m_iDummy;
    }

    bool operator <  (ThisClass const& rOther) const;
    bool operator <= (ThisClass const& rOther) const;
    bool operator == (ThisClass const& rOther) const;
    bool operator != (ThisClass const& rOther) const;
    bool operator >= (ThisClass const& rOther) const;
    bool operator >  (ThisClass const& rOther) const;
};


/***********************************
 *****  Member Implementation  *****
 ***********************************/

template <unsigned int sDatum> VkUnsignedTemplate<sDatum> const &VkUnsignedTemplate<sDatum>::Zero () {
    static bool g_bNotInitialized = true;
    static ThisClass g_iZero;
    if (g_bNotInitialized) {
	g_iZero.SetToZero ();
	g_bNotInitialized = false;
    }
    return g_iZero;
}

template <unsigned int sDatum> void VkUnsignedTemplate<sDatum>::SetToZero () {
    for (index_t j=0; j<sDatum; j++)
	m_i[j] = 0;
}

template <unsigned int sDatum> bool VkUnsignedTemplate<sDatum>::operator < (
    VkUnsignedTemplate<sDatum> const& rOther
) const {
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
    for (index_t j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (index_t j=sDatum; j-- > 0;)
#endif
    {
	if (m_i[j] < rOther.m_i[j])
	    return true;
	else if (m_i[j] > rOther.m_i[j])
	    return false;
    }
    return false;
}

template <unsigned int sDatum> bool VkUnsignedTemplate<sDatum>::operator <= (
    VkUnsignedTemplate<sDatum> const& rOther
) const {
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
    for (index_t j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (index_t j=sDatum; j-- > 0;)
#endif
    {
	if (m_i[j] < rOther.m_i[j])
	    return true;
	else if (m_i[j] > rOther.m_i[j])
	    return false;
    }
    return true;
}

template <unsigned int sDatum> bool VkUnsignedTemplate<sDatum>::operator == (
    VkUnsignedTemplate<sDatum> const& rOther
) const {
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
    for (index_t j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (index_t j=sDatum; j-- > 0;)
#endif
	if (m_i[j] != rOther.m_i[j])
	    return false;
    return true;
}

template <unsigned int sDatum> bool VkUnsignedTemplate<sDatum>::operator != (
    VkUnsignedTemplate<sDatum> const& rOther
) const {
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
    for (index_t j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (index_t j=sDatum; j-- > 0;)
#endif
	if (m_i[j] != rOther.m_i[j])
	    return true;
    return false;
}

template <unsigned int sDatum> bool VkUnsignedTemplate<sDatum>::operator >= (
    VkUnsignedTemplate<sDatum> const& rOther
) const {
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
    for (index_t j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (index_t j=sDatum; j-- > 0;)
#endif
    {
	if (m_i[j] > rOther.m_i[j])
	    return true;
	else if (m_i[j] < rOther.m_i[j])
	    return false;
    }
    return true;
}

template <unsigned int sDatum> bool VkUnsignedTemplate<sDatum>::operator >  (
    VkUnsignedTemplate<sDatum> const& rOther
) const {
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
    for (index_t j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (index_t j=sDatum; j-- > 0;)
#endif
    {
	if (m_i[j] > rOther.m_i[j])
	    return true;
	else if (m_i[j] < rOther.m_i[j])
	    return false;
    }
    return false;
}

typedef VkUnsignedTemplate<(unsigned int)2> VkUnsigned64;
typedef VkUnsignedTemplate<(unsigned int)3> VkUnsigned96;
typedef VkUnsignedTemplate<(unsigned int)4> VkUnsigned128;


/****************************
 *----  Instantiations  ----*
 ****************************/

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(V_VLongUnsigneds)

#ifndef V_VLongUnsigneds
#define V_VLongUnsigneds extern
#endif

V_VLongUnsigneds template class V_API VkUnsignedTemplate<(unsigned int)2>;
V_VLongUnsigneds template class V_API VkUnsignedTemplate<(unsigned int)3>;
V_VLongUnsigneds template class V_API VkUnsignedTemplate<(unsigned int)4>;

#endif

#endif
