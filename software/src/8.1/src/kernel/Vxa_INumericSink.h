#ifndef Vxa_INumericSink_Interface
#define Vxa_INumericSink_Interface

#ifndef Vxa_INumericSink
#define Vxa_INumericSink extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IClient.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vxa,INumericSink)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    typedef Vca::F32 f32s_t;
    typedef Vca::F64 f64s_t;

    typedef Vca::S08 i08s_t;
    typedef Vca::S16 i16s_t;
    typedef Vca::S32 i32s_t;
    typedef Vca::S64 i64s_t;

    typedef Vca::U08 u08s_t;
    typedef Vca::U16 u16s_t;
    typedef Vca::U32 u32s_t;
    typedef Vca::U64 u64s_t;

    typedef VkDynamicArrayOf<f32s_t> f32a_t;
    typedef VkDynamicArrayOf<f64s_t> f64a_t;

    typedef VkDynamicArrayOf<i08s_t> i08a_t;
    typedef VkDynamicArrayOf<i16s_t> i16a_t;
    typedef VkDynamicArrayOf<i32s_t> i32a_t;
    typedef VkDynamicArrayOf<i64s_t> i64a_t;

    typedef VkDynamicArrayOf<u08s_t> u08a_t;
    typedef VkDynamicArrayOf<u16s_t> u16a_t;
    typedef VkDynamicArrayOf<u32s_t> u32a_t;
    typedef VkDynamicArrayOf<u64s_t> u64a_t;

    VINTERFACE_ROLE (INumericSink, Vca::IClient)
	VINTERFACE_ROLE_1 (OnF32S, f32s_t);
	VINTERFACE_ROLE_1 (OnF64S, f64s_t);

	VINTERFACE_ROLE_1 (OnI08S, i08s_t);
	VINTERFACE_ROLE_1 (OnI16S, i16s_t);
	VINTERFACE_ROLE_1 (OnI32S, i32s_t);
	VINTERFACE_ROLE_1 (OnI64S, i64s_t);

	VINTERFACE_ROLE_1 (OnU08S, u08s_t);
	VINTERFACE_ROLE_1 (OnU16S, u16s_t);
	VINTERFACE_ROLE_1 (OnU32S, u32s_t);
	VINTERFACE_ROLE_1 (OnU64S, u64s_t);

	VINTERFACE_ROLE_1 (OnF32A, f32a_t const&);
	VINTERFACE_ROLE_1 (OnF64A, f64a_t const&);

	VINTERFACE_ROLE_1 (OnI08A, i08a_t const&);
	VINTERFACE_ROLE_1 (OnI16A, i16a_t const&);
	VINTERFACE_ROLE_1 (OnI32A, i32a_t const&);
	VINTERFACE_ROLE_1 (OnI64A, i64a_t const&);

	VINTERFACE_ROLE_1 (OnU08A, u08a_t const&);
	VINTERFACE_ROLE_1 (OnU16A, u16a_t const&);
	VINTERFACE_ROLE_1 (OnU32A, u32a_t const&);
	VINTERFACE_ROLE_1 (OnU64A, u64a_t const&);
    VINTERFACE_ROLE_END

    VxaINTERFACE (INumericSink, Vca::IClient)
	VINTERFACE_METHOD_1 (OnF32S, f32s_t);
	VINTERFACE_METHOD_1 (OnF64S, f64s_t);

	VINTERFACE_METHOD_1 (OnI08S, i08s_t);
	VINTERFACE_METHOD_1 (OnI16S, i16s_t);
	VINTERFACE_METHOD_1 (OnI32S, i32s_t);
	VINTERFACE_METHOD_1 (OnI64S, i64s_t);

	VINTERFACE_METHOD_1 (OnU08S, u08s_t);
	VINTERFACE_METHOD_1 (OnU16S, u16s_t);
	VINTERFACE_METHOD_1 (OnU32S, u32s_t);
	VINTERFACE_METHOD_1 (OnU64S, u64s_t);

	VINTERFACE_METHOD_1 (OnF32A, f32a_t const&);
	VINTERFACE_METHOD_1 (OnF64A, f64a_t const&);

	VINTERFACE_METHOD_1 (OnI08A, i08a_t const&);
	VINTERFACE_METHOD_1 (OnI16A, i16a_t const&);
	VINTERFACE_METHOD_1 (OnI32A, i32a_t const&);
	VINTERFACE_METHOD_1 (OnI64A, i64a_t const&);

	VINTERFACE_METHOD_1 (OnU08A, u08a_t const&);
	VINTERFACE_METHOD_1 (OnU16A, u16a_t const&);
	VINTERFACE_METHOD_1 (OnU32A, u32a_t const&);
	VINTERFACE_METHOD_1 (OnU64A, u64a_t const&);
    VINTERFACE_END
}


#endif
