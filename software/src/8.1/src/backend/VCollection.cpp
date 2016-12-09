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
    char const* pComponentName, RTYPE_Type xComponentType
) const {
    raiseException (
	EC__InternalInconsistency,
	"VCollectionOfStrings: Unexpected %s %s Component",
	RTYPE_TypeIdAsString (xComponentType),
	pComponentName
    );
}
