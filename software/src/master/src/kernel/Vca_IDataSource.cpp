/*****  Vca_IDataSource Implementation  *****/

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

#define   Vca_IDataSource
#define   Vca_IDataSource_Instantiations
#define   Vca_IDataSource_Instantiations1
#ifndef __VMS
#define   Vca_IDataSource_Instantiations2
#endif
#include "Vca_IDataSource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::IDataSource  *****
 *****                    *****
 ******************************
 ******************************/

namespace Vca {
    // {97F019C1-DDCE-45d6-9939-9983D2530EC1}
    VINTERFACE_SYMBOL_DEFINITION (
	IDataSource, 0x97f019c1, 0xddce, 0x45d6, 0x99, 0x39, 0x99, 0x83, 0xd2, 0x53, 0xe, 0xc1
    );
}
