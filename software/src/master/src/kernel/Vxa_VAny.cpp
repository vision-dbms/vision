/*****  Vxa_VExportableDatum Implementation  *****/

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

#include "Vxa_VAny.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VCollectableObject.h"


/*******************************
 *******************************
 *****                     *****
 *****  Vxa::VAny::Client  *****
 *****                     *****
 *******************************
 *******************************/

/**********************
 **********************
 *****  Delivery  *****
 **********************
 **********************/

void Vxa::VAny::Client::deliver (VString const &rString) {
    VCollectableObject::Reference pObject;
    if (VCollectableObject::GetObjectForTicket (pObject, rString))
        on (pObject);
    else
        on (rString);
}
