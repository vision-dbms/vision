/*****  Vxa_VCallType2Exporter Implementation  *****/

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

#include "Vxa_VCallType2Exporter.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vxa_VTask.h"


/*************************************
 *************************************
 *****                           *****
 *****  Vxa::VCallType2Exporter  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vxa::VCallType2Exporter::VCallType2Exporter (
    VTask *pTask, VCallType2Importer const &rCallImporter
) : BaseClass (pTask->cursor ()), VCallType2 (rCallImporter) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vxa::VCallType2Exporter::~VCallType2Exporter () {
    if (!m_iMapMaker.transmitUsing (this))
	returnError ("Call Type 2 Transport Failed");
}

/***************************
 ***************************
 *****  Result Return  *****
 ***************************
 ***************************/

bool Vxa::VCallType2Exporter::returnResult (VExportableDatum const &rDatum) {
    return rDatum.returnResultUsing (this);
}

/***********************************************
 ***********************************************
 *****  MapMaker, MapMaker, Make Me A Map  *****
 ***********************************************
 ***********************************************/

Vxa::VMapMaker &Vxa::VCallType2Exporter::getMapMakerFor (VSet *pCodomain) {
    return m_iMapMaker.getMapMakerFor (cursorPosition (), pCodomain, tailHints ());
}
