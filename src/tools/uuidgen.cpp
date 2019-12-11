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
    V::uuid_t const &rUUID = iUUID;
    V::VString iUUIDString;
    iUUID.GetString (iUUIDString);

//  Output the UUID in standard format...
    printf ("%s\n", iUUIDString.content ());

//  ... and Vca (a.k.a, OLECREATE format) if requested:
    V::VCommandLine const iCommandLine (argc, argv);
    if (iCommandLine.switchValue ("vca")) {
	printf (
	    "0x%08x, 0x%04x, 0x%04x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
	    rUUID.time_low,
	    rUUID.time_mid,
	    rUUID.time_hi_and_version,
	    rUUID.clock_seq_hi_and_reserved,
	    rUUID.clock_seq_low,
	    rUUID.node[0],
	    rUUID.node[1],
	    rUUID.node[2],
	    rUUID.node[3],
	    rUUID.node[4],
	    rUUID.node[5]
	);
    }

    return NormalExitValue;
}
