/*****  VCodeTable Implementation  *****/

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

#include "VCodeTable.h"

/************************
 *****  Supporting  *****
 ************************/

#include "verrdef.h"

#include "VCodeDescriptor.h"


/**********************************
 **********************************
 *****  Table Initialization  *****
 **********************************
 **********************************/

void VCodeTableBase::initialize (
    VCodeDescriptor const**	pNameSortedTable,
    VCodeDescriptor const**	pCodeSortedTable,
    VCodeDescriptor const*	pDefaultDescription
)
{
    unsigned int xCode;

    //  Fill the code table...
    for (xCode = 0; xCode < m_sCodeSortedTable; xCode++)
	pCodeSortedTable[xCode] = pDefaultDescription;

    for (xCode = 0; xCode < m_sNameSortedTable; xCode++)
    {
	VCodeDescriptor const* pCodeDescription = pNameSortedTable[xCode];
	if (pCodeSortedTable[pCodeDescription->code ()] == pDefaultDescription)
	    pCodeSortedTable[pCodeDescription->code ()]  = pCodeDescription;
	else raiseException (
	    EC__InternalInconsistency,
	    "Duplicate Code %u <%s> In %s Code Table",
	    pCodeDescription->code (),
	    pCodeDescription->name (),
	    m_pTableName
	);
    }

    //  ... sort the name sorted table, ...
    qsort (
	pNameSortedTable, m_sNameSortedTable, sizeof (VCodeDescriptor*), (VkComparator)NameSortPredicate
    );

    //  ... and check the name sort for duplicate entries:
    for (xCode = 1; xCode < m_sNameSortedTable; xCode++)
    {
	VCodeDescriptor const* pCodeDescription = pNameSortedTable[xCode];
	if (0 == NameSortPredicate (
		pNameSortedTable + xCode, pNameSortedTable + xCode - 1
	    )
	) raiseException (
	    EC__InternalInconsistency,
	    "Duplicate Name %s <%u> In %s Code Table",
	    pCodeDescription->name (),
	    pCodeDescription->code (),
	    m_pTableName
	);
    }
}


/*************************
 *************************
 *****  Name Lookup  *****
 *************************
 *************************/

unsigned int VCodeTableBase::lookup (
    char const*				pName,
    VCodeDescriptor const* const*	pNameSortedTable
) const
{
    VCodeDescriptor  iSearchKey (0, pName);
    VCodeDescriptor* pSearchKey = &iSearchKey;
    VCodeDescriptor const* const* pSearchResult = (VCodeDescriptor const* const*)bsearch (
	&pSearchKey, pNameSortedTable, m_sNameSortedTable, sizeof (VCodeDescriptor*), (VkComparator)NameSortPredicate
    );
    return pSearchResult ? (*pSearchResult)->code () : m_sCodeSortedTable;
}


/*********************************
 *********************************
 *****  Name Sort Predicate  *****
 *********************************
 *********************************/

int __cdecl VCodeTableBase::NameSortPredicate (
    VCodeDescriptor const* const* ppDescription1,
    VCodeDescriptor const* const* ppDescription2
)
{
    return strcmp ((*ppDescription1)->name (), (*ppDescription2)->name ());
}
