/*****  VPath Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/************************
 *****  Supporting  *****
 ************************/

/******************
 *****  Self  *****
 ******************/

#include "VPath.h"

/*******************
 *******************
 *****         *****
 *****  VPath  *****
 *****         *****
 *******************
 *******************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT(VPath);

void VPath::concatenate (VPath *pPath) {

    if(!pPath)
        return;
        
    // if one of the  path sets is empty
    if(pPath->isEmpty() || isEmpty())
            return; 
   
    // if both path sets are non empty
    VReference<VPath>    pSourcePathList;
    VReference<VPath>    pDestinationPathList;
    VReference<VPath>    pResult(new VPath());
   
    pSourcePathList.setTo(this);

    while(pSourcePathList.isntNil()) {

        pDestinationPathList.setTo(pPath);
        while(pDestinationPathList.isntNil()) {

            VReference<VPath> pNewSourcePath;
            VReference<VPath> pNewDestinationPath;
         
            pNewSourcePath.setTo      (new VPath (*new VPathPeer (*pSourcePathList->m_pHeadPeer)));
            pNewDestinationPath.setTo (new VPath (*new VPathPeer (*pDestinationPathList->m_pHeadPeer)));
        
            pNewSourcePath->tailPeer()->m_pNextPeer.setTo(pNewDestinationPath->m_pHeadPeer->m_pNextPeer);
          
            pResult->appendPath(pNewSourcePath);
            pDestinationPathList = pDestinationPathList->m_pNextPath;
        }

        pSourcePathList = pSourcePathList->m_pNextPath;
    }

    // replacing old list with new list
    m_pHeadPeer.setTo (pResult->m_pHeadPeer); 
    m_pNextPath.setTo (pResult->m_pNextPath);
}

void VPath::summation  (VPath *pPath) {

    if (!pPath)
        return;

    if (m_pHeadPeer.isNil())  {
            m_pHeadPeer.setTo(pPath->m_pHeadPeer); 
            m_pNextPath.setTo(pPath->m_pNextPath);
    }
    else {  
            if(pPath->m_pHeadPeer.isntNil())
                tailPath()->m_pNextPath.setTo(pPath);
    }
}

/*******************
 *******************
 *****         *****
 *****  VPath::VPathPeer  *****
 *****         *****
 *******************
 *******************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_ABSTRACT_RTT(VPath::VPathPeer);
