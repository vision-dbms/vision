/*****  VCollection Implementation  *****/

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

#include "VCollection.h"

/************************
 *****  Supporting  *****
 ************************/

#include "verrdef.h"


/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

void VCollection::raiseComponentTypeException (
    char const* pComponentName, M_CPD* pComponent
) const {
    raiseException (
	EC__InternalInconsistency,
	"VCollectionOfStrings: Unexpected %s %s Component",
	RTYPE_TypeIdAsString ((RTYPE_Type)M_CPD_RType (pComponent)),
	pComponentName
    );
}
