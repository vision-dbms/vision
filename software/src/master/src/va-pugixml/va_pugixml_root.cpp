/*****  VA::PugiXML::Root Implementation  *****/

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

#include "va_pugixml_root.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VString.h"
#include "Vxa_VCollectable.h"
#include "Vxa_VResultBuilder.h"


/*******************************
 *******************************
 *****                     *****
 *****  VA::Pugixml::Root  *****
 *****                     *****
 *******************************
 *******************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTTLITE (VA::PugiXML::Root);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::PugiXML::Root::Root () {
}


/*******************************
 *******************************
 *****  Document Creation  *****
 *******************************
 *******************************/


/************************************
 ************************************
 *****  Method Implementations  *****
 ************************************
 ************************************/

void VA::PugiXML::Root::loadDocument (Vxa::VResultBuilder &rRB, VString const &rFilename) {
    rRB = new Document (rFilename);
}

/***************************
 ***************************
 *****  Class Builder  *****
 ***************************
 ***************************/

VA::PugiXML::Root::ClassBuilder::ClassBuilder (Vxa::VClass *pClass) : Object::ClassBuilder (pClass) {
    defineMethod ("load:", &Root::loadDocument);
}

namespace {
    Vxa::VCollectable<VA::PugiXML::Root> g_iRootMeta;
}

DEFINE_VXA_COLLECTABLE(VA::PugiXML::Root);
