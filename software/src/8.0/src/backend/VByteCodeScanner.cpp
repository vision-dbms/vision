/*****  VByteCodeScanner Implementation  *****/

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

#include "VByteCodeScanner.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VByteCodeScanner::VByteCodeScanner (M_CPD *pBlockCPD) : m_pBlockCPD (pBlockCPD), m_pBlockHandle (
    static_cast<rtBLOCK_Handle*>(pBlockCPD->containerHandle ())
), m_xBC (0) {
}

VByteCodeScanner::VByteCodeScanner (rtBLOCK_Handle *pBlockHandle) : m_pBlockHandle (pBlockHandle), m_xBC (0) {
}

VByteCodeScanner::~VByteCodeScanner () {
}
