/*****  V::VGSID Implementation  *****/

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

#include "V_VGSID.h"

/************************
 *****  Supporting  *****
 ************************/


/*****************************
 *****************************
 *****                   *****
 *****  V::VGSID::VGSID  *****
 *****                   *****
 *****************************
 *****************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VGSID V::VGSID::NewGSID () {
    VSSID iNewSSID;
    iNewSSID.generateSSID ();

    return ThisClass (VkUUID::ReturnUUID (), iNewSSID);
}


V::VGSID::VGSID (uuid_t const& rUUID, VSSID const& rSSID) : m_iUUID (rUUID), m_iSSID (rSSID) {
}

V::VGSID::VGSID (ThisClass const& rOther) : m_iUUID (rOther.uuid ()), m_iSSID (rOther.ssid ()) {
}

V::VGSID::VGSID () {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VGSID::~VGSID () {
}
