/*****  Long Unsigneds Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

#include "Vk.h"

#include "VkLongUnsigneds.h"

/***********************
 ***********************
 *****  Constants  *****
 ***********************
 ***********************/

PublicVarDef unsigned int VkLongUnsigneds_DummyElement = 0;

/*************************************
 *************************************
 *****  Template Instantiations  *****
 *************************************
 *************************************/

template class VkUnsignedTemplate<(unsigned int)2>;
template class VkUnsignedTemplate<(unsigned int)3>;
template class VkUnsignedTemplate<(unsigned int)4>;
