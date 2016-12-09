/*****  V_VBlob Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "V_VBlob.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************
 *********************
 *****           *****
 *****  V_VBlob  *****
 *****           *****
 *********************
 *********************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VBlob::VBlob (void const *pData, size_t sData, bool bCopy)
: m_iStorage (const_cast<void*>(pData), sData, bCopy), m_xLowerBound (0), m_xUpperBound (sData)
{
}

V::VBlob::VBlob (void *pVoid, size_t sVoid, bool bCopy)
: m_iStorage (pVoid, sVoid, bCopy), m_xLowerBound (0), m_xUpperBound (0)
{
}

V::VBlob::VBlob (VBlob const& rOther)
: m_iStorage	(rOther.m_iStorage)
, m_xLowerBound	(rOther.m_xLowerBound)
, m_xUpperBound	(rOther.m_xUpperBound)
{
}

V::VBlob::VBlob (size_t sStorage)
: m_iStorage (sStorage), m_xLowerBound (0), m_xUpperBound (0)
{
}

V::VBlob::VBlob () : m_xLowerBound (0), m_xUpperBound (0) {
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void V::VBlob::append (void const *pContent, size_t sContent) {
    size_t xUpperBound = m_xUpperBound + sContent;
    memcpy (storage (xUpperBound) + m_xUpperBound, pContent, sContent);
    m_xUpperBound = xUpperBound;
}

void V::VBlob::clear () {
    m_iStorage.setTo (0, 0, false);
    m_xLowerBound = m_xUpperBound = 0;
}

size_t V::VBlob::incrementContentLowerBoundBy (size_t sAdjustment) {
    if (sAdjustment > contentSize ())
	sAdjustment = contentSize ();
    m_xLowerBound += sAdjustment;
    return sAdjustment;
}

size_t V::VBlob::incrementContentUpperBoundBy (size_t sAdjustment) {
    if (sAdjustment > postContentSize ())
	sAdjustment = postContentSize ();
    m_xUpperBound += sAdjustment;
    return sAdjustment;
}

void V::VBlob::setTo (void *pStorage, size_t sStorage, bool bCopy) {
    m_iStorage.setTo (pStorage, sStorage, bCopy);
    m_xLowerBound = m_xUpperBound = 0;
}

void V::VBlob::setTo (size_t sStorage, bool bExact) {
    m_iStorage.guarantee (sStorage, bExact);
    m_xLowerBound = m_xUpperBound = 0;
}

void V::VBlob::setTo (VBlob const &rOther) {
    if (this != &rOther) {
	m_iStorage.setTo (rOther.m_iStorage);
	m_xLowerBound = rOther.m_xLowerBound;
	m_xUpperBound = rOther.m_xUpperBound;
    }
}

void V::VBlob::setContentBoundsTo (size_t xLowerBound, size_t xUpperBound) {
    if (xUpperBound > storageSize ())
	xUpperBound = storageSize ();
    if (xLowerBound > xUpperBound)
	xLowerBound = xUpperBound;
    m_xLowerBound = xLowerBound;
    m_xUpperBound = xUpperBound;
}

void V::VBlob::setContentLowerBoundTo (size_t xBound) {
    if (xBound > m_xUpperBound)
	xBound = m_xUpperBound;
    m_xLowerBound = xBound;
}

void V::VBlob::setContentUpperBoundTo (size_t xBound) {
    if (xBound < m_xLowerBound)
	xBound = m_xLowerBound;
    else if (xBound > storageSize ())
	xBound = storageSize ();
    m_xUpperBound = xBound;
}
