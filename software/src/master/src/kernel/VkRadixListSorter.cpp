/*****  VkRadixListSorter Implementation  *****/

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

#include "VkRadixListSorter.h"

/************************
 *****  Supporting  *****
 ************************/

#include "M_CPD.h"


/**********************
 **********************
 *****  Counters  *****
 **********************
 **********************/

unsigned int VkRadixListSorter::g_cTrivialCalls;
unsigned int VkRadixListSorter::g_cSortedCalls;
unsigned int VkRadixListSorter::g_cSortedElements;
unsigned int VkRadixListSorter::g_cUnsortedCalls;
unsigned int VkRadixListSorter::g_cUnsortedElements;


/*********************
 *********************
 *****  Sorting  *****
 *********************
 *********************/

/***********************
 *****  Utilities  *****
 ***********************/

#define SetupValuePointersForRLSortPass(xPass)\
    pDByte = m_iCurrentElement.asByteArray;\
    pDByte += OffsetOfNthByte (unsigned int, xPass);\
    pCByte = m_iCurrentElement.asByteArray;\
    pCByte += OffsetOfNthByte (unsigned int, xPass + 1)

#define SetupRLSortDestinationPointers(destination)\
    for (i = 0, destinationPointer = destination; i < 256; i++) {\
        pointers[i] = destinationPointer;\
	destinationPointer += m_iCounts[i];\
	m_iCounts[i] = 0;\
    }

#define SequentialRLSortIndices(i) i
#define IndicesResidentRLSortIndices(i) pSort[i]
#define TempResidentRLSortIndices(i) temp[i]

#define ComputeNewRLSortCounts m_iCounts[*pCByte]++
#define NoNewRLSortCounts

#define RLSortDistribute(indexAccessMacro, countComputationMacro)\
    for (i = 0; i < m_cElements; i++) {\
	unsigned int index = indexAccessMacro (i);\
	m_iCurrentElement.asInt = m_pElements[index];\
	*pointers[*pDByte]++ = index;\
	countComputationMacro;\
    }


/**********************
 *****  Routines  *****
 **********************/

bool VkRadixListSorter::SetElements (
    unsigned int const *pElements, unsigned int cElements
) {
    m_pElements = pElements;
    m_cElements = cElements;
    m_fElementsAlreadySorted = true;

    if (cElements > 1) {
	unsigned char const *pCByte, *pDByte;
	SetupValuePointersForRLSortPass (0);

	memset (m_iCounts, 0, sizeof (m_iCounts));

	m_iLargestElement = 0;
	for (unsigned int i = 0; i < cElements; i++) {
	    m_iCurrentElement.asInt = pElements[i];
	    m_iCounts [*pCByte]++;

	    if (m_iLargestElement < m_iCurrentElement.asInt)
		m_iLargestElement = m_iCurrentElement.asInt;
	    else if (m_fElementsAlreadySorted && m_iLargestElement > m_iCurrentElement.asInt)
		m_fElementsAlreadySorted = false;
	}
    }

    return !m_fElementsAlreadySorted;
}

bool VkRadixListSorter::GetSort (unsigned int *pSort) {
//  Check for trivial sorts, ...
    if (m_cElements < 2) {
	g_cTrivialCalls++;

	if (m_cElements)
	    pSort[0] = 0;

	return false;
    }

/*****  Common Initialization  *****/
    if (m_fElementsAlreadySorted) {
	g_cSortedCalls++;
	g_cSortedElements += m_cElements;

	for (unsigned int i = 0; i < m_cElements; i++)
	    pSort[i] = i;

	return false;
    }

    g_cUnsortedCalls++;
    g_cUnsortedElements += m_cElements;

    unsigned char const *pCByte, *pDByte;
    unsigned int i, *destinationPointer, *pointers[256];

    if (m_iLargestElement < (1 << 8)) {
/*****  Perform a one-pass sort if all values are less than 2^8  *****/
/*----  Pass 1 of 1  ----*/
	SetupValuePointersForRLSortPass (1);
	SetupRLSortDestinationPointers (pSort);
	RLSortDistribute (SequentialRLSortIndices, NoNewRLSortCounts);
	return true;
    }

/*****  All multi-pass sorts require a temporary array  *****/
    unsigned int *temp = (unsigned int *)allocate (m_cElements * sizeof (unsigned int));

    if (m_iLargestElement < (1 << 16)) {
/*****  Perform a two-pass sort if all values are less than 2^16  *****/
/*----  Pass 1 of 2  ----*/
	SetupValuePointersForRLSortPass (1);
	SetupRLSortDestinationPointers (temp);
	RLSortDistribute (SequentialRLSortIndices, ComputeNewRLSortCounts);

/*----  Pass 1 of 2  ----*/
	SetupValuePointersForRLSortPass (2);
	SetupRLSortDestinationPointers (pSort);
	RLSortDistribute (TempResidentRLSortIndices, NoNewRLSortCounts);
    }

    else if (m_iLargestElement < (1 << 24)) {
/*****  Perform a three-pass sort if all values are less than 2^24  *****/
/*----  Pass 1 of 3  ----*/
	SetupValuePointersForRLSortPass (1);
	SetupRLSortDestinationPointers (pSort);
	RLSortDistribute (SequentialRLSortIndices, ComputeNewRLSortCounts);

/*----  Pass 2 of 3  ----*/
	SetupValuePointersForRLSortPass (2);
	SetupRLSortDestinationPointers (temp);
	RLSortDistribute (IndicesResidentRLSortIndices, ComputeNewRLSortCounts);

/*----  Pass 3 of 3  ----*/
	SetupValuePointersForRLSortPass (3);
	SetupRLSortDestinationPointers (pSort);
	RLSortDistribute (TempResidentRLSortIndices, NoNewRLSortCounts);
    }

/*****  Perform a four-pass sort otherwise  *****/
    else {
/*----  Pass 1 of 4  ----*/
	SetupValuePointersForRLSortPass (1);
	SetupRLSortDestinationPointers (temp);
	RLSortDistribute (SequentialRLSortIndices, ComputeNewRLSortCounts);

/*----  Pass 2 of 4  ----*/
	SetupValuePointersForRLSortPass (2);
	SetupRLSortDestinationPointers (pSort);
	RLSortDistribute (TempResidentRLSortIndices, ComputeNewRLSortCounts);

/*----  Pass 3 of 4  ----*/
	SetupValuePointersForRLSortPass (3);
	SetupRLSortDestinationPointers (temp);
	RLSortDistribute (IndicesResidentRLSortIndices, ComputeNewRLSortCounts);

/*----  Pass 4 of 4  ----*/
	SetupValuePointersForRLSortPass (4);
	SetupRLSortDestinationPointers (pSort);
	RLSortDistribute (TempResidentRLSortIndices, NoNewRLSortCounts);
    }

/*****  Multi-Pass Sort Cleanup  *****/
    deallocate (temp);

    return true;
}
