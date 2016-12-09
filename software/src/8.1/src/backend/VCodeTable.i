#ifndef VCodeTable_Implementation
#define VCodeTable_Implementation

template <
    class CodeEnumerationClass, class CodeDescriptionClass, size_t CodeTableSize
> VCodeTable<
    CodeEnumerationClass, CodeDescriptionClass, CodeTableSize
>::VCodeTable (
    char const*			pTableName,
    size_t			sDescriptionArray,
    CodeDescriptionClass const *pDescriptionArray,
    CodeDescriptionClass const *pDefaultDescription
) : VCodeTableBase (pTableName, sDescriptionArray, CodeTableSize)
{
    for (
	unsigned int xCode = 0; xCode < sDescriptionArray; xCode++
    ) m_pNameSortedTable[xCode] = &pDescriptionArray[xCode];

    initialize (
	(VCodeDescriptor const**)m_pNameSortedTable,
	(VCodeDescriptor const**)m_pCodeSortedTable,
	pDefaultDescription
    );
}


#endif
