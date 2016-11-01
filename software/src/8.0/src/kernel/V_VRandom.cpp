/*****  V_VRandom Implementation  *****/
#define V_VRandom_Instantiations

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

#include "V_VRandom.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VTime.h"


/************************
 ************************
 *****              *****
 *****  V::VRandom  *****
 *****              *****
 ************************
 ************************/

/************************
 ************************
 *****  Generators  *****
 ************************
 ************************/
#ifdef sun
// On Solaris, RAND_MAX is meant to be used with rand() and is defined to be 32767
// Since we are using random() instead, we need to redefine it
#undef RAND_MAX
#define RAND_MAX 2147483647
#endif

V::VRandom::fraction_t V::VRandom::Fraction () {
    return static_cast<fraction_t>(Number ()) / (RAND_MAX + 1.0);
}

V::VRandom::number_t V::VRandom::Number () {
    static bool bNotSeeded = true;
    if (bNotSeeded) {
	bNotSeeded = false;
	U64 const iNow = VTime::encodeNow ();
	U32 const iSeed = static_cast<U32>(iNow ^ (iNow >> 32));
#ifdef _WIN32
	srand (iSeed);
    }
    return rand ();
#else
	srandom (iSeed);
    }
    return random ();
#endif
}
