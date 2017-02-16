#ifndef VUtilities_Interface_809B4D32_61E0_49BD_AD17_ADD164B7C6E4
#define VUtilities_Interface_809B4D32_61E0_49BD_AD17_ADD164B7C6E4

#include "zconf.h"
#include "zlib.h"


/*************************
 *****  Compilation  *****
 *************************/

#if !defined(USING_WINDOWS_DLL_DECLSPECS)
#define VUtilities_API

#elif defined(VUTILITIES_EXPORTS)
#define VUtilities_API __declspec(dllexport)

#else
#define VUtilities_API __declspec(dllimport)

#endif


/***********************
 *****  Namespace  *****
 ***********************/

namespace VUtilities {

    /********************************************
     *****  Zlib Compression/Decompression  *****
     ********************************************/
    
    /* Compresses the source buffer into the destination buffer.  
       sourceLen is the byte length of the source buffer. Upon entry, 
       destLen is the total size of the destination buffer, 
       which must be at least 0.1% larger than sourceLen plus 12 bytes. 
       Upon exit, destLen is the actual size of the compressed buffer.
       Returns Z_OK if success, Z_MEM_ERROR if there was not enough memory, 
       Z_BUF_ERROR if there was not enough room in the output buffer.

    */

    VUtilities_API int Zlib_Compress (Bytef *dest, uLongf *destLen,
		       const Bytef *source, uLong sourceLen
    );

   /* Decompresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be large enough to hold the
   entire uncompressed data. (The size of the uncompressed data must have
   been saved previously by the compressor and transmitted to the decompressor
   by some mechanism outside the scope of this compression library.)
   Upon exit, destLen is the actual size of the compressed buffer.
     Returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_BUF_ERROR if there was not enough room in the output
   buffer, or Z_DATA_ERROR if the input data was corrupted.
   */

    VUtilities_API int Zlib_Uncompress (Bytef *dest, uLongf *destLen,
		       const Bytef *source, uLong sourceLen
    );

    VUtilities_API int Zlib_DeflateInit (z_streamp strm, int level);
    VUtilities_API int Zlib_Deflate (z_streamp strm, int flush);
    VUtilities_API int Zlib_DeflateEnd (z_streamp strm);


    VUtilities_API int Zlib_InflateInit (z_streamp strm);
    VUtilities_API int Zlib_Inflate (z_streamp strm, int flush);
    VUtilities_API int Zlib_InflateEnd (z_streamp strm);

}

#endif
