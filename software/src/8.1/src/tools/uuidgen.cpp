/*****  UUID Generator  *****/

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

/************************
 *****  Supporting  *****
 ************************/

#include "V_VCommandLine.h"
#include "V_VString.h"

#include "VkUUID.h"


/******************
 ******************
 *****  Main  *****
 ******************
 ******************/

int main (int argc, char *argv[]) {
//  Create and format a UUID...
    VkUUIDGenerate const iUUID;
    uuid_t const &rUUID = iUUID;
    VString iUUIDString;
    iUUID.GetString (iUUIDString);

//  Output the UUID in standard format...
    printf ("%s\n", iUUIDString.content ());

//  ... and Vca (a.k.a, OLECREATE format) if requested:
    V::VCommandLine const iCommandLine (argc, argv);
    if (iCommandLine.switchValue ("vca")) {
	stduuid_t const &rStdUUID = *reinterpret_cast<stduuid_t const*>(&rUUID);

	printf (
	    "0x%08x, 0x%04x, 0x%04x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
	    rStdUUID.time_low,
	    rStdUUID.time_mid,
	    rStdUUID.time_hi_and_version,
	    rStdUUID.clock_seq_hi_and_reserved,
	    rStdUUID.clock_seq_low,
	    rStdUUID.node[0],
	    rStdUUID.node[1],
	    rStdUUID.node[2],
	    rStdUUID.node[3],
	    rStdUUID.node[4],
	    rStdUUID.node[5]
	);
    }

    return NormalExitValue;
}
