/*****  Data Base Update Utilities Facility Header File  *****/
#ifndef DBUPDATE_Interface
#define DBUPDATE_Interface

/***************************************************************
 *****  Data Base Network Object Description Record Types  *****
 ***************************************************************/

PublicVarDecl bool DBUPDATE__DumpFileIsOpen;

PublicVarDecl FILE* DBUPDATE__DumpFile;


/********************************
 *  DB Dump File Output Macros  *
 ********************************/

#define DBUPDATE_DumpFileMustBeOpen {\
    if (!DBUPDATE__DumpFileIsOpen) ERR_SignalFault (\
	EC__UsageError, "DBUPDATE: DB Dump File is not open."\
    );\
}

/*---------------------------------------------------------------------------
 ****** Macro to output a header record to the Data Base Dump File.
 *
 *  Arguments:
 *	pathName		- a string containing the path name of the
 *				  directory holding the space being updated
 *				  (typically, 'OSDPathName').
 *	space			- the integer index of the space being updated.
 *	segment 		- the integer index of the segment holding the
 *				  current container table for the space.
 *	index			- the integer index of the table being updated.
 *	replace	    		- true if this update is a replacemant,
 *                                False if this update is an append.
 *	ndf			- the NDF path name for this network (to be
 *				  used by the redundancy manager compatible
 *				  version of the incorporator).
 *	nvdID			- the NDF offset of the NVD of this network
 *				  version.
 *
 ******/
#define  DBUPDATE_OutputHeaderRecord(pathName,space,segment,index,replace,ndf,nvdID) {\
    DBUPDATE_DumpFileMustBeOpen;\
    IO_fprintf (\
	DBUPDATE__DumpFile, "100 %s %d %d %d %d %s %d\n",\
	pathName, space, segment, index, replace, ndf, nvdID\
    );\
}


/*---------------------------------------------------------------------------
 ****** Macro to output a vector record to the Data Base Dump File.
 *
 *  Arguments:
 *	itemName		- a string containing the item name.
 *	vectorIndex		- an integer containing the container table
 *                                index of the vector.
 *      ptokenIndex		- an integer containing the container table
 *                                index of the vector's ptoken.
 *	usegCount		- the number of usegments in the vector.
 *
 ******/
#define  DBUPDATE_OutputVectorRecord(itemName,vectorIndex,ptokenIndex, usegCount) {\
    DBUPDATE_DumpFileMustBeOpen;\
    IO_fprintf (DBUPDATE__DumpFile, "1 %s %d %d %d\n",\
	itemName, vectorIndex, ptokenIndex, usegCount);\
}


/*---------------------------------------------------------------------------
 ****** Macro to output an lstore content record to the Data Base Dump File.
 *
 *  Arguments:
 *	uVectorIndex		- an integer containing the container table
 *                                index of the lstore's character uvector.
 *      pTokenIndex		- an integer containing the container table
 *                                index of the lstore's character uvector's
 *                                ptoken.
 *
 ******/
#define  DBUPDATE_OutputLStoreContentRec(uVectorIndex,pTokenIndex) {\
    DBUPDATE_DumpFileMustBeOpen;\
    IO_fprintf (DBUPDATE__DumpFile, "2 8 %d %d 0\n",\
	uVectorIndex, pTokenIndex);\
}


/*---------------------------------------------------------------------------
 ****** Macro to output a USegment record to the Data Base Dump File.
 *
 *  Arguments:
 *	rtype			- an integer representing the rtype of the
 *				  uvector.
 *	uVectorIndex		- an integer containing the container table
 *                                index of the vector's uvector.
 *      ptokenIndex		- an integer containing the container table
 *                                index of the uvector's ptoken.
 *	vectorIndex		- an integer containing the container table
 *	                          index of the vector.
 *
 ******/
#define DBUPDATE_OutputUSegmentRecord(rtype,uVectorIndex,pTokenIndex,vectorIndex) {\
    DBUPDATE_DumpFileMustBeOpen;\
    IO_fprintf (DBUPDATE__DumpFile, "2 %d %d %d %d\n",\
	rtype, uVectorIndex, pTokenIndex, vectorIndex);\
}


/*---------------------------------------------------------------------------
 ****** Macro to output a lstore record to the Data Base Dump File.
 *
 *  Arguments:
 *	lstoreIndex		- an integer containing the container table
 *                                index of the lstore.
 *      ptokenIndex		- an integer containing the container table
 *                                index of the lstore's ptoken.
 *
 ******/
#define  DBUPDATE_OutputLStoreRecord(lstoreIndex,ptokenIndex) {\
    DBUPDATE_DumpFileMustBeOpen;\
    IO_fprintf (DBUPDATE__DumpFile, "3 %s %d %d 1\n",\
	rtLSTORE_StringStoreItem, lstoreIndex, ptokenIndex);\
}


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  dbupdate.h 1 replace /users/mjc/system
  860728 13:45:03 mjc Add data base update utilities facility

 ************************************************************************/
/************************************************************************
  dbupdate.h 2 replace /users/jad/system
  860804 18:34:31 jad fixed ifndef's to be for the correct module

 ************************************************************************/
/************************************************************************
  dbupdate.h 3 replace /users/mjc/translation
  870524 09:37:08 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
