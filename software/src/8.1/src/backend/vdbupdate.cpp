/*****  Data Base Update Utilities Facility  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName DBUPDATE

/*******************************************
 ******  Header and Declaration Files  *****
 *******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "verr.h"
#include "vfac.h"
#include "viobj.h"
#include "vstdio.h"

/*****  Shared  *****/
#include "vdbupdate.h"


/****************************************
 ****************************************
 *****  Data Base Dump File Globals *****
 ****************************************
 ****************************************/

/********************
 *  File Variables  *
 ********************/

PublicVarDef bool		DBUPDATE__DumpFileIsOpen = false;
PublicVarDef FILE		*DBUPDATE__DumpFile;


/*********************************
 *********************************
 *****  Facility Definition  *****
 *********************************
 *********************************/

FAC_DefineFacility
{
    IOBJ_BeginMD (methodDictionary)
	IOBJ_MDE ("qprint"			, FAC_DisplayFacilityIObject)
	IOBJ_MDE ("print"			, FAC_DisplayFacilityIObject)
    IOBJ_EndMD;

    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (DBUPDATE);
    FAC_FDFCase_FacilityDescription ("Data Base Update Utilities");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  dbupdate.c 1 replace /users/mjc/system
  860728 13:44:57 mjc Add data base update utilities facility

 ************************************************************************/
/************************************************************************
  dbupdate.c 2 replace /users/jad/system
  860731 17:47:44 jad added update debug methods

 ************************************************************************/
/************************************************************************
  dbupdate.c 3 replace /users/jck/system
  860820 17:58:52 jck Modified DataBase Dumpers

 ************************************************************************/
/************************************************************************
  dbupdate.c 4 replace /users/jck/system
  860910 14:12:36 jck Added TStore as possible Object Space Root

 ************************************************************************/
/************************************************************************
  dbupdate.c 5 replace /users/jad/system
  860914 11:14:54 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  dbupdate.c 6 replace /users/jad/system
  870120 15:00:26 jad modified DB update procedure

 ************************************************************************/
/************************************************************************
  dbupdate.c 7 replace /users/mjc/system
  870215 23:03:51 lcn Changed format of DB update dump

 ************************************************************************/
/************************************************************************
  dbupdate.c 8 replace /users/mjc/translation
  870524 09:36:59 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  dbupdate.c 9 replace /users/jck/system
  890912 16:05:45 jck Removing RTtstore from the system

 ************************************************************************/
