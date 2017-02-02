#ifndef OSDUMP_H
#define OSDUMP_H

/*********************************
 *****  Constants and Types  *****
 *********************************/

/*****  Constants    *****/
#define MAXPATHLENGTH	1024
#define MAXUSEGMENTS	10

/*****  Record Type Definitions  *****/
#define HEADER		100

#define VECTOR		1
#define USEGMENT	2
#define LSTORE		3

struct osDUMP_USegType {
    int rtype;
    int uvectorIndex;
    int ptokenIndex;
};

struct osDUMP_VectorType {
    int recordType;
    char item[256];
    int vectorIndex;
    int ptokenIndex;
    int usegCount;
    osDUMP_USegType usegEntry [1];
};

#define osDUMP_RecordType(v)	    ((v)->recordType)
#define osDUMP_Item(v)		    ((v)->item)
#define osDUMP_VectorIndex(v)	    ((v)->vectorIndex)
#define osDUMP_PtokenIndex(v)	    ((v)->ptokenIndex)
#define osDUMP_UsegCount(v)	    ((v)->usegCount)
#define osDUMP_UvectorIndex(v, i)   ((v)->usegEntry[i].uvectorIndex)
#define osDUMP_UPtokenIndex(v, i)   ((v)->usegEntry[i].ptokenIndex)
#define osDUMP_RType(v, i)	    ((v)->usegEntry[i].rtype)


#define osDUMP_SizeofVectorType(n) (\
    sizeof (osDUMP_VectorType) + ((n) - 1) * sizeof (osDUMP_USegType)\
)

#define osDUMP_GetVectorPtr(vector, item, size) osDUMP_GetColumnPtr (\
    vector, item, (size), VECTOR, false\
)

#define osDUMP_GetDumpEntry(vector, item) osDUMP_GetColumnPtr (\
    vector, item, NilOf (size_t *), VECTOR, true\
)

#define osDUMP_GetLStorePtr(lstore, item) osDUMP_GetColumnPtr (\
    lstore, item, NilOf (size_t *), LSTORE, false\
)


/*******************
 *****  State  *****
 *******************/

PublicVarDecl M_CPreamble*	osDUMP_ContainerTablePreamble;
PublicVarDecl PS_CT*		osDUMP_ContainerTable;
PublicVarDecl char		osDUMP_PathName[MAXPATHLENGTH];
PublicVarDecl char		osDUMP_NDFPathName[MAXPATHLENGTH];
PublicVarDecl long		osDUMP_ContSegment,
				osDUMP_CTIndex,
				osDUMP_TableIndex,
				osDUMP_SpaceName,
				osDUMP_Replacing;


/************************
 *****  Operations  *****
 ************************/

PublicFnDecl int osDUMP_ReadDump (
    char *			dumpname
);

PublicFnDecl void osDUMP_CleanupDump (
    void
);

PublicFnDecl int osDUMP_ReadContTable (
    void
);

PublicFnDecl long osDUMP_GetNewCTEntry (
    void
);

PublicFnDecl int osDUMP_SizeOfContainerTable (
    void
);

PublicFnDecl void osDUMP_WriteContainerTable (
    char *			fileptr,
    char **			endPtr,
    long *			filesize,
    long			segment
);

PublicFnDecl void osDUMP_unmapSegments (
    void
);

PublicFnDecl M_CPreamble* osDUMP_GetColumnPtr(
    osDUMP_VectorType **	dbVector,
    char const *		item,
    size_t *			size,
    int				type,
    int				entryOnly
);

PublicFnDecl M_CPreamble* osDUMP_GetTablePtr ();

PublicFnDecl M_CPreamble* osDUMP_GetPtokenPtr (
    M_CPreamble*		table
);

PublicFnDecl M_CPreamble* osDUMP_GetUvectorPtr (
    osDUMP_VectorType *		dbVector,
    int				i
);

PublicFnDecl M_CPreamble* osDUMP_GetVectorPtokenPtr (
    osDUMP_VectorType *		dbVector
);

PublicFnDecl M_CPreamble* osDUMP_GetUsegPtokenPtr (
    osDUMP_VectorType *		dbVector,
    int				i
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  OSdump.h 1 replace /users/ees/dbutil
  861030 12:02:00 ees OSpace dump routine decl's

 ************************************************************************/
/************************************************************************
  OSdump.h 2 replace /users/jck/updates
  870129 13:26:37 jck First release as a module of the incorporator

 ************************************************************************/
/************************************************************************
  OSdump.h 3 replace /users/jck/updates
  870629 14:06:51 jck Implemented processing of multiple prefixes; reimplemented
allocation scheme for final segment; integrated avl search routines

 ************************************************************************/
/************************************************************************
  OSdump.h 4 replace /users/m2/dbup
  880426 11:54:13 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  OSdump.h 5 replace /users/jck/updates
  880615 14:22:44 jck Hardened some soft spots in the incorporator

 ************************************************************************/
/************************************************************************
  OSdump.h 6 replace /users/jck/updates
  880628 08:58:06 jck Plugged another gap in the incorporator

 ************************************************************************/
