#ifndef VkRadixListSorter_Interface
#define VkRadixListSorter_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

/*---------------------------------------------------------------------------
 *****  Routines to generate a set of ascending sort indices for an array of
 *****  'unsigned int's using a radix list sort.
 *
 *  Arguments:
 *	pSort			- the address of an array which will receive
 *				  the sort indices generated.
 *	pElements		- the address of the array of 'unsigned int's
 *				  to be sorted.
 *	cElements		- the number of elements in 'm_pElements'.
 *
 *  Return Value:
 *	true if a sort was necessary, false if the array was already in order.
 *
 *****/

class VkRadixListSorter : public VTransient {
//  Counters
public:
    static unsigned int	g_cTrivialCalls;
    static unsigned int	g_cSortedCalls;
    static unsigned int	g_cSortedElements;
    static unsigned int	g_cUnsortedCalls;
    static unsigned int	g_cUnsortedElements;

//  Construction
public:
    VkRadixListSorter () : m_cElements (0), m_fElementsAlreadySorted (true) {
    }

//  Destruction
public:
    ~VkRadixListSorter () {
    }

//  Sorting
public:
    bool SetElements (unsigned int const *pElements, unsigned int cElements);

    bool GetSort (unsigned int *pSort);

    static bool GetSort (
	unsigned int const *pElements, unsigned int cElements, unsigned int *pSort
    ) {
	VkRadixListSorter iSorter;
	iSorter.SetElements (pElements, cElements);
	return iSorter.GetSort (pSort);
    }

//  State
protected:
    unsigned int const	*m_pElements;
    size_t		 m_cElements;
    bool		 m_fElementsAlreadySorted;
    union element_t {
        unsigned int	    asInt;
	unsigned char	    asByteArray[4];
    }			 m_iCurrentElement;
    unsigned int	 m_iLargestElement;
    unsigned int	 m_iCounts[256];
};

#define UTIL_RadixListSort(array,indices,n) VkRadixListSorter::GetSort((array),(n),(indices))


#endif
