/*****  VA::PugiXML::ParseResult Implementation  *****/

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

#include "va_pugixml_parse_result.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"
#include "Vxa_VCollectable.h"
#include "Vxa_VResultBuilder.h"


/**************************************
 **************************************
 *****                            *****
 *****  VA::Pugixml::ParseResult  *****
 *****                            *****
 **************************************
 **************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTTLITE (VA::PugiXML::ParseResult);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::PugiXML::ParseResult::ParseResult () {
}


/************************************
 ************************************
 *****  Method Implementations  *****
 ************************************
 ************************************/

VA::PugiXML::ParseResult::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : Object::ClassBuilder (pClass) {
}

namespace {
    Vxa::VCollectable<VA::PugiXML::ParseResult> g_iParseResultMeta;
}

DEFINE_VXA_COLLECTABLE(VA::PugiXML::ParseResult);
