
/*************************
 *************************
 *****  Interfaces  ******
 *************************
 *************************/

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

#include "VkMemory.h"

#include "VStaticTransient.h"


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

typedef unsigned int checksum_t;

static VkMemory::Share g_xMappingMode = VkMemory::Share_Private;

static void CheckSum (char const *file, checksum_t *cs1, checksum_t *cs2) {
    checksum_t cs = 0;
    *cs1 = *cs2 = cs;

    VkMemory mapping;
    if (!mapping.Map (file, true, g_xMappingMode))
	return;

    V::pointer_t baddr, paddr, eaddr;
    baddr = (V::pointer_t)mapping.RegionAddress ();
    for (
	*cs1 = *(checksum_t *)(baddr + 28),
	eaddr = baddr + mapping.RegionSize (),
	paddr = baddr + 32;

	paddr < eaddr;

	paddr += sizeof (checksum_t)
    ) cs ^= *(checksum_t *)paddr;
    *cs2 = cs;
}

int main (int argc, char *argv[]) {
/*****  ... Global Reclamation Manager Initialization, ...  *****/
    VkMemory::StartGRM (argv[0]);

    int mismatchCount = 0;
    for (int argi = 1; argi < argc; argi++) {
	if (strcmp (argv[argi], "-p") == 0)
	    g_xMappingMode = VkMemory::Share_Private;
	else if (strcmp (argv[argi], "-s") == 0)
	    g_xMappingMode = VkMemory::Share_Group;
	else {
	    checksum_t cs1, cs2;
	    CheckSum (argv[argi], &cs1, &cs2);
	    if (cs1 != 0 && cs1 != cs2) mismatchCount++;
		printf (
		    "%-10s: stored cs = %10u, computed cs = %10u (%s)\n",
		    argv[argi],
		    cs1,
		    cs2,
		    cs1 == 0 ? "NA" : cs1 == cs2 ? "OK" : "MISMATCH"
	    );
	}
    }
    return mismatchCount;
}
