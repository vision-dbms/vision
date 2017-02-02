#ifndef VCodeTable_Interface
#define VCodeTable_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

class VCodeDescriptor;

/*************************
 *****  Definitions  *****
 *************************/

/********************
 *  VCodeTableBase  *
 ********************/

class VCodeTableBase : public VTransient {
//  Construction
protected:
    VCodeTableBase (
	char const* pTableName, unsigned int sNameSortedTable, unsigned int sCodeSortedTable
    )
    : m_pTableName		(pTableName)
    , m_sNameSortedTable	(sNameSortedTable)
    , m_sCodeSortedTable	(sCodeSortedTable)
    {
    }

//  Initialization
protected:
    void initialize (
	VCodeDescriptor const**pNameSortedTable,
	VCodeDescriptor const**pCodeSortedTable,
	VCodeDescriptor const*	pDefaultDescription
    );

//  Lookup
protected:
    unsigned int lookup (
	char const*			pName,
	VCodeDescriptor const* const*	pNameSortedTable
    ) const;

//  Access
public:
    char const* tableName () const {
	return m_pTableName;
    }

    unsigned int nameTableSize () const {
	return m_sNameSortedTable;
    }
    unsigned int codeTableSize () const {
	return m_sCodeSortedTable;
    }

//  State
protected:
    char const*		m_pTableName;
    unsigned int	m_sNameSortedTable;
    unsigned int	m_sCodeSortedTable;

//  Globals
protected:
    static int __cdecl NameSortPredicate (
	VCodeDescriptor const* const* ppDescription1,
	VCodeDescriptor const* const* ppDescription2
    );
};


/****************
 *  VCodeTable  *
 ****************/

template <
    class CodeEnumerationClass, class CodeDescriptionClass, size_t CodeTableSize
> class VCodeTable : public VCodeTableBase {
//  Construction
public:
    VCodeTable (
	char const*			pTableName,
	size_t				sDescriptionArray,
	CodeDescriptionClass const*	pDescriptionArray,
	CodeDescriptionClass const*	pDefaultDescription
    );

//  Access
public:
    CodeDescriptionClass const *entry (
	unsigned int xCode, CodeDescriptionClass const *pNotFoundResult
    ) const
    {
	return xCode < CodeTableSize ? m_pCodeSortedTable[xCode] : pNotFoundResult;
    }
    CodeDescriptionClass const *entry (
	char const* pName, CodeDescriptionClass const *pNotFoundResult
    ) const
    {
	return entry (
	    (CodeEnumerationClass)lookup (
		pName, (VCodeDescriptor const* const*)m_pNameSortedTable
	    ), pNotFoundResult
	);
    }

    CodeDescriptionClass const *operator[] (CodeEnumerationClass xCode) const {
	return entry (xCode, 0);
    }
    CodeDescriptionClass const *operator[] (char const* pName) const {
	return entry (pName, 0);
    }

//  State
protected:
    CodeDescriptionClass const *m_pNameSortedTable[CodeTableSize];
    CodeDescriptionClass const *m_pCodeSortedTable[CodeTableSize];
};


/*****************************************
 *****  Template Member Definitions  *****
 *****************************************/

#if defined(_AIX) && defined(__TEMPINC__)
#pragma implementation("VCodeTable.i")
#else
#include "VCodeTable.i"
#endif



#endif
