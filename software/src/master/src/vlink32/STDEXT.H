#ifndef STDEXT_H
#define STDEXT_H

/////////////////////////
//  CString Extensions
/////////////////////////

const CString& operator+=(CString &dstString, short		shortInt);
const CString& operator+=(CString &dstString, long		longInt);
const CString& operator+=(CString &dstString, WORD		iWORD);
const CString& operator+=(CString &dstString, DWORD		iDWORD);
const CString& operator+=(CString &dstString, LPVOID	iPtr);

#endif
