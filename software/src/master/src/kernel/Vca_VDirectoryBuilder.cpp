/*****  Vca_VDirectoryBuilder Implementation  *****/

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

#include "Vca_VDirectoryBuilder.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

/************************************
 ************************************
 *****                          *****
 *****  Vca::VDirectoryBuilder  *****
 *****                          *****
 ************************************
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VDirectoryBuilder::VDirectoryBuilder () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VDirectoryBuilder::~VDirectoryBuilder () {
}

/*******************
 *******************
 *****  Build  *****
 *******************
 *******************/

Vca::VDirectory* Vca::VDirectoryBuilder::newDirectory_() const {
    return new VDirectory ();
}

void Vca::VDirectoryBuilder::create (VDirectory::Reference& rpDirectory) {
    rpDirectory.setTo (newDirectory_());
    build (rpDirectory);
}
