/*****  V_VAtom Implementation  *****/

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

#include "V_VAtom.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************
 *********************
 *****           *****
 *****  V_VAtom  *****
 *****           *****
 *********************
 *********************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (V::VAtom);

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

V::VAtom::AtomTable V::VAtom::g_iAtomTable;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::VAtom::VAtom (uuid_t const &rUUID) : m_iUUID (rUUID) {
}

V::VAtom *V::VAtom::instance (uuid_t const &rUUID) {
    unsigned int xInstance;
    if (g_iAtomTable.Insert (rUUID, xInstance))
	g_iAtomTable[xInstance] = new VAtom (rUUID);
    return g_iAtomTable[xInstance];
}

V::VAtom *V::VAtom::instance (char const *pUUID) {
    VkUUIDHolder iUUID (pUUID);
    return instance (iUUID);
}

V::VAtom *V::VAtom::instance (
    unsigned int  iD1, unsigned short  iD2, unsigned short  iD3,
    unsigned char iD4_0, unsigned char iD4_1, unsigned char iD4_2, unsigned char iD4_3,
    unsigned char iD4_4, unsigned char iD4_5, unsigned char iD4_6, unsigned char iD4_7
) {
    VkUUIDHolder iUUID (iD1, iD2, iD3, iD4_0, iD4_1, iD4_2, iD4_3, iD4_4, iD4_5, iD4_6, iD4_7);
    return instance (iUUID);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

V::VAtom::~VAtom () {
    g_iAtomTable.Delete (m_iUUID);
}
