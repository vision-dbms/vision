/*****  Long Unsigneds Interface  *****/
#ifndef VkLongUnsigneds_Interface
#define VkLongUnsigneds_Interface

#include "Vk.h"

PublicVarDecl unsigned int VkLongUnsigneds_DummyElement;

template <unsigned int sDatum> class VkUnsignedTemplate {
public:
    typedef VkUnsignedTemplate<sDatum> ThisClass;

protected:
    unsigned int m_i[sDatum];

public:
    static ThisClass const &Zero ();

    void SetToZero ();

    unsigned int operator[] (unsigned int xElement) const {
	return xElement < sDatum
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
	    ? m_i[xElement]
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
	    ? m_i[sDatum - xElement - 1]
#endif
	    : 0;
    }

    unsigned int& operator[] (unsigned int xElement) {
	return xElement < sDatum
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
	    ? m_i[xElement]
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
	    ? m_i[sDatum - xElement - 1]
#endif
	    : VkLongUnsigneds_DummyElement;
    }

    bool operator <  (ThisClass const& rOther) const;
    bool operator <= (ThisClass const& rOther) const;
    bool operator == (ThisClass const& rOther) const;
    bool operator != (ThisClass const& rOther) const;
    bool operator >= (ThisClass const& rOther) const;
    bool operator >  (ThisClass const& rOther) const;
};

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
    for (unsigned int j=0; j<sDatum; j++)
	m_i[j] = 0;
}

template <unsigned int sDatum> bool VkUnsignedTemplate<sDatum>::operator < (
    VkUnsignedTemplate<sDatum> const& rOther
) const {
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
    for (unsigned int j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (unsigned int j=sDatum; j-- > 0;)
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
    for (unsigned int j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (unsigned int j=sDatum; j-- > 0;)
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
    for (unsigned int j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (unsigned int j=sDatum; j-- > 0;)
#endif
	if (m_i[j] != rOther.m_i[j])
	    return false;
    return true;
}

template <unsigned int sDatum> bool VkUnsignedTemplate<sDatum>::operator != (
    VkUnsignedTemplate<sDatum> const& rOther
) const {
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
    for (unsigned int j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (unsigned int j=sDatum; j-- > 0;)
#endif
	if (m_i[j] != rOther.m_i[j])
	    return true;
    return false;
}

template <unsigned int sDatum> bool VkUnsignedTemplate<sDatum>::operator >= (
    VkUnsignedTemplate<sDatum> const& rOther
) const {
#if   Vk_DataFormatNative == Vk_DataFormatBEndian
    for (unsigned int j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (unsigned int j=sDatum; j-- > 0;)
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
    for (unsigned int j=0; j<sDatum; j++)
#elif Vk_DataFormatNative == Vk_DataFormatLEndian
    for (unsigned int j=sDatum; j-- > 0;)
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


#endif
