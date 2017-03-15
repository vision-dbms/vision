/*****  VSorter Implementation  *****/

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

#include "VSorter.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VCollectionOfOrderables.h"


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VSorter::VSorter (VCollectionOfOrderables const& rOrderableCollection)
    : m_rOrderableCollection (rOrderableCollection)
    , m_pElementBuffer (0)
    , m_sElementBuffer (0)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VSorter::~VSorter () {
    if (m_pElementBuffer)
	deallocate (m_pElementBuffer);
}


/***********************
 ***********************
 *****  Heap Sort  *****
 ***********************
 ***********************/

void VSorter::hsort (char *base, size_t nel, size_t sel) {
    unsigned int i;

    buildheap (base, nel, sel);
    for (i = nel; i >= 2; i--) {
	interchange (base, 1, i, sel);
	heapify (base, 1, i - 1, sel);
    }
}


void VSorter::buildheap (char *base, size_t nel, size_t sel) {
    int i;

    for (i = nel / 2; i >= 1; i--) heapify (base, i, nel, sel);
}

void VSorter::heapify (char *base, unsigned int i, unsigned int j, size_t sel) {
    unsigned int k;

    for (;;) {
	k = i * 2;
	if (k > j) break;	/*  No Leaves  */
	
	if (k < j && m_rOrderableCollection.compare (base + (k - 1) * sel, base + k * sel) < 0)
	    k++;

	if (m_rOrderableCollection.compare (base + (i - 1) * sel, base + (k - 1) * sel) >= 0)
	    break;

	interchange (base, i, k, sel);
	i = k;
    }
}


/*********************************
 *********************************
 *****  Algorithm Utilities  *****
 *********************************
 *********************************/

void VSorter::interchange (
    char *base, unsigned int el1, unsigned int el2, size_t sel
)
{
    union element_t {
	char	c;
	short	s;
	int	i;
	double	d;
    } el;

    el1--;
    el2--;
    switch (sel) {
    case 0:
	break;
    case 1:
	el.c			= *(base + el1);
	*(base + el1)		= *(base + el2);
	*(base + el2)		= el.c;
        break;
    case 2:
	el.s			= *((short *)base + el1);
	*((short *)base + el1)	= *((short *)base + el2);
	*((short *)base + el2)	= el.s;
        break;
    case 4:
	el.i			= *((int *)base + el1);
	*((int *)base + el1)	= *((int *)base + el2);
	*((int *)base + el2)	= el.i;
        break;
    case 8:
	el.d			= *((double *)base + el1);
	*((double *)base + el1)	= *((double *)base + el2);
	*((double *)base + el2) = el.d;
        break;
    default:
	if (m_sElementBuffer != sel) {
	    m_sElementBuffer = 0;
	    deallocate (m_pElementBuffer);
	    m_pElementBuffer = allocate (sel);
	    m_sElementBuffer = sel;
	}
	memcpy (m_pElementBuffer, base + el1 * sel, sel);
	memcpy (base + el1 * sel, base + el2 * sel, sel);
	memcpy (base + el2 * sel, m_pElementBuffer, sel);
        break;
    }
}
