///////////////////////////
//  Standard Extensions
///////////////////////////

#include "stdafx.h"
#include "stdext.h"

///////////////////////////
//  CString Extensions
///////////////////////////

const CString& operator+=(CString &dstString, short shortInt)
{
    char buffer[16];
    sprintf (buffer, "%d", shortInt);
    return dstString += (LPCSTR)buffer;
}

const CString& operator+=(CString &dstString, long longInt)
{
    char buffer[16];
    sprintf (buffer, "%ld", longInt);
    return dstString += (LPCSTR)buffer;
}

const CString& operator+=(CString &dstString, WORD iWORD)
{
    char buffer[16];
    sprintf (buffer, "%u", iWORD);
    return dstString += (LPCSTR)buffer;
}

const CString& operator+=(CString &dstString, DWORD iDWORD)
{
    char buffer[16];
    sprintf (buffer, "%lu", iDWORD);
    return dstString += (LPCSTR)buffer;
}

const CString& operator+=(CString &dstString, LPVOID iPtr)
{
    char buffer[16];
    sprintf (buffer, "%lp", iPtr);
    return dstString += (LPCSTR)buffer;
}
