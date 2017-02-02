/*****  Miscellaneous Utilities Header File  *****/
#ifndef VUTIL_H
#define VUTIL_H

/*****  Shared Declarations  *****/
#include "VkLongUnsigneds.h"

#include "V_VAllocator.h"

/************************************************
 ************************************************
 *****  Expression and Size Display Macros  *****
 ************************************************
 ************************************************/

#define UTIL_DisplayExpression(format, expression) IO_printf (\
    "%-40s = "#format"\n", #expression, expression\
)

#define UTIL_DisplaySizeofType(type) IO_printf (\
    "%-40s = %u\n", "sizeof ("#type")", sizeof (type)\
)

/******************************************
 ******************************************
 *****  Free List Management Globals  *****
 ******************************************
 ******************************************/

#define UTIL_FreeListTotal		V::VAllocatorFreeList::g_sFreeListTotal
#define UTIL_FreePoolTotal		V::VAllocatorFreeList::g_sFreePoolTotal
#define UTIL_FreePoolWaste		V::VAllocatorFreeList::g_sFreePoolWaste
#define UTIL_FreePoolAdaptations	V::VAllocatorFreeList::g_cFreePoolAdaptations
#define UTIL_FreeListOversizes		V::VAllocatorFreeList::g_cFreeListOverflows

/**************************************************
 **************************************************
 *****  Message Buffer Services Declarations  *****
 **************************************************
 **************************************************/

#define UTIL_FormatMessage VTransient::string

/***************************************
 ***************************************
 *****  Memory Allocator Services  *****
 ***************************************
 ***************************************/

#define UTIL_Malloc(cBytes) VTransient::allocate (cBytes)
#define UTIL_Realloc(pMemory,cBytes) VTransient::reallocate ((pMemory), (cBytes))
#define UTIL_Free(pMemory) VTransient::deallocate (pMemory)


/*********************
 *********************
 *****  Sorters  *****
 *********************
 *********************/

PublicFnDecl void UTIL_CharQSort (
    char const *array, unsigned int *indices, size_t n, bool descending
);

PublicFnDecl void UTIL_DoubleQSort (
    double const *array, unsigned int *indices, size_t n, bool descending
);

PublicFnDecl void UTIL_FloatQSort (
    float const *array, unsigned int *indices, size_t n, bool descending
);

PublicFnDecl void UTIL_IntQSort (
    int const *array, unsigned int *indices, size_t n, bool descending
);

PublicFnDecl void UTIL_Unsigned64QSort (
    VkUnsigned64 const *array, unsigned int *indices, size_t n, bool descending
);

PublicFnDecl void UTIL_Unsigned96QSort (
    VkUnsigned96 const *array, unsigned int *indices, size_t n, bool descending
);

PublicFnDecl void UTIL_Unsigned128QSort (
    VkUnsigned128 const *array, unsigned int *indices, size_t n, bool descending
);

/***************************************
 ***************************************
 *****  Number Formating Services  *****
 ***************************************
 ***************************************/

PublicFnDecl int UTIL_FormatNumberWithCommas (
    char *			buffer,
    size_t			maxSize,
    int				overflowChar,
    int				fieldSpecification,
    int				precision,
    double			number
);

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  util.h 1 replace /users/mjc/system
  860226 16:26:17 mjc create

 ************************************************************************/
/************************************************************************
  util.h 2 replace /users/jad/system
  860422 15:16:35 jad use new io module

 ************************************************************************/
/************************************************************************
  util.h 3 replace /users/mjc/system
  860430 20:00:35 mjc Added free list manager and radix list sorter

 ************************************************************************/
/************************************************************************
  util.h 4 replace /users/mjc/system
  861014 22:54:52 mjc Implemented Q-Sorters and 'ExpandFreeList'

 ************************************************************************/
/************************************************************************
  util.h 5 replace /users/mjc/translation
  870524 09:42:36 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  util.h 6 replace /users/jad/system
  880805 13:44:10 jad Improve the performance on the PrintOf and SPrint primitives

 ************************************************************************/
