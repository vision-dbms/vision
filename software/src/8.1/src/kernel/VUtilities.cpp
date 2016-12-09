
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

#include "VUtilities.h"

/************************
 *****  Supporting  *****
 ************************/


/*********************
 *********************
 *****  DllMain  *****
 *********************
 *********************/

#if defined(USING_WINDOWS_DLL_DECLSPECS)

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD xCallReason, LPVOID lpReserved) {
    switch (xCallReason) {
    case DLL_PROCESS_ATTACH:
	DisableThreadLibraryCalls (hModule);
	break;
    case DLL_THREAD_ATTACH:
	break;
    case DLL_THREAD_DETACH:
	break;
    case DLL_PROCESS_DETACH:
	break;
    }
    return true;
}

#endif

VUtilities_API int VUtilities::Zlib_Compress (
    Bytef *dest, uLongf *destLen,
    const Bytef *source, uLong sourceLen
) {
    return compress (dest, destLen, source, sourceLen);
}
 
VUtilities_API int VUtilities::Zlib_Uncompress (
    Bytef *dest, uLongf *destLen,
    const Bytef *source, uLong sourceLen
) {
    return uncompress (dest, destLen, source, sourceLen);
}

VUtilities_API int VUtilities::Zlib_Deflate (z_streamp strm, int flush) {
    return deflate (strm, flush);
}

VUtilities_API int VUtilities::Zlib_DeflateInit (z_streamp strm, int level) {
    return deflateInit (strm, level);
}

VUtilities_API int VUtilities::Zlib_DeflateEnd (z_streamp strm) {
    return deflateEnd (strm);
}


VUtilities_API int VUtilities::Zlib_Inflate (z_streamp strm, int flush) {
    return inflate (strm, flush);
}

VUtilities_API int VUtilities::Zlib_InflateInit (z_streamp strm) {
    return inflateInit (strm);
}

VUtilities_API int VUtilities::Zlib_InflateEnd (z_streamp strm) {
    return inflateEnd (strm);
}
