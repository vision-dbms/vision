/*****  VMagicWordDescriptor Implementation  *****/

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

#include "VMagicWordDescriptor.h"

/************************
 *****  Supporting  *****
 ************************/

/*************************************
 *****  Template Instantiations  *****
 *************************************/

template class VCodeTable<VMagicWord, VMagicWordDescriptor, (unsigned int)VMagicWord_MaximumMagic>;


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VMagicWordDescriptor::VMagicWordDescriptor (
    VMagicWord xMagicWord, char const* pMagicWordName
) : VCodeDescriptor (xMagicWord, pMagicWordName)
{
}


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

/************************************************
 *****  The Default Magic Word Description  *****
 ************************************************/

VMagicWordDescriptor const VMagicWordDescriptor::DefaultDescription (
    VMagicWord_MaximumMagic, "error"
);


/*****************************************
 *****  The Magic Word Descriptions  *****
 *****************************************/

static VMagicWordDescriptor const Descriptions[] = {
    VMagicWordDescriptor (VMagicWord_Current	, "current"),
    VMagicWordDescriptor (VMagicWord_Self	, "self"),
    VMagicWordDescriptor (VMagicWord_Super	, "super"),
    VMagicWordDescriptor (VMagicWord_Here	, "here"),
    VMagicWordDescriptor (VMagicWord_Global	, "global"),
    VMagicWordDescriptor (VMagicWord_My		, "my"),
    VMagicWordDescriptor (VMagicWord_Date	, "date"),
    VMagicWordDescriptor (VMagicWord_Today	, "today"),
    VMagicWordDescriptor (VMagicWord_Tmp	, "tmp"),
    VMagicWordDescriptor (VMagicWord_Local	, "local"),
    VMagicWordDescriptor (VMagicWord_Ground	, "ground")
};


/***************************************
 *****  The Magic Word Code Table  *****
 ***************************************/

VMagicWordDescriptorCodeTable const VMagicWordDescriptor__CodeTable (
    "Magic Word",
    sizeof (Descriptions) / sizeof (VMagicWordDescriptor),
    Descriptions,
    &VMagicWordDescriptor::DefaultDescription
);



