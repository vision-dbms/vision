#ifndef Vca_VTuple_Interface
#define Vca_VTuple_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "Vca_VInstanceHolder.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VcaSerializer;

    /********************
     *----  VTuple  ----*
     ********************/
    class VTuple : public VTransient {
    //  Construction
    protected:
	VTuple () {
	}

    //  Destruction
    protected:
	~VTuple () {
	}

    //  Access
    public:
	virtual unsigned int cardinality_() const = 0;

    //  Serializer Creation
    public:
	virtual VcaSerializer *serializer (
	    VcaSerializer *pCaller, unsigned int xField
	) = 0;

    //  State
    protected:
    };


    /**********************
     *----  VTuple_1  ----*
     **********************/

    template <class F1> class VTuple_1 : public VTuple {
    //  Family
	DECLARE_FAMILY_MEMBERS (VTuple_1<F1>, VTuple);

    //  Instance Holder
    public:
	typedef VInstanceHolder<F1> IH_1;
	typedef typename IH_1::StorageType IH_1_ST;

    //  Construction
    public:
	VTuple_1 (F1 iF1) : m_iF1(iF1) {
	}
	VTuple_1 () {
	}

    //  Destruction
    public:
	~VTuple_1 () {
	}

    //  Access
    public:
	unsigned int cardinality () const {
	    return 1;
	}
	unsigned int cardinality_() const {
	    return cardinality ();
	}

	IH_1_ST const &f1 () const {
	    return m_iF1;
	}
	IH_1_ST &f1 () {
	    return m_iF1;
	}

    //  Serialization
    public:
	VcaSerializer *serializer (VcaSerializer *pCaller, unsigned int xField) {
	    switch (xField) {
	    case 0:
		return m_iF1.serializer (pCaller);
	    default:
		break;
	    }
	    return NilOf (VcaSerializer*);
	}

    //  State
    protected:
	IH_1 m_iF1;
    };


    /**********************
     *----  VTuple_2  ----*
     **********************/

    template <class F1,class F2> class VTuple_2 : public VTuple_1<F1> {
    //  Family
    public:
	typedef VTuple_2<F1,F2> ThisClass;
	typedef VTuple_1<F1> BaseClass;

    //  Instance Holder
    public:
	typedef VInstanceHolder<F2> IH_2;
	typedef typename IH_2::StorageType IH_2_ST;

    //  Construction
    public:
	VTuple_2 (F1 iF1, F2 iF2) : BaseClass (iF1), m_iF2(iF2) {
	}
	VTuple_2 () {
	}

    //  Destruction
    public:
	~VTuple_2 () {
	}

    //  Access
    public:
	unsigned int cardinality () const {
	    return 2;
	}
	unsigned int cardinality_() const {
	    return cardinality ();
	}

	IH_2_ST const &f2 () const {
	    return m_iF2;
	}
	IH_2_ST &f2 () {
	    return m_iF2;
	}

    //  Serialization
    public:
	VcaSerializer *serializer (VcaSerializer *pCaller, unsigned int xField) {
	    switch (xField) {
	    case 0:
		return m_iF1.serializer (pCaller);
	    case 1:
		return m_iF2.serializer (pCaller);
	    default:
		break;
	    }
	    return NilOf (VcaSerializer*);
	}

    //  State
    protected:
	USING (BaseClass::m_iF1)
	IH_2 m_iF2;
    };


    /**********************
     *----  VTuple_3  ----*
     **********************/

    template <class F1,class F2,class F3> class VTuple_3 : public VTuple_2<F1,F2> {
    //  Family
    public:
	typedef VTuple_3<F1,F2,F3> ThisClass;
	typedef VTuple_2<F1,F2> BaseClass;

    //  Instance Holder
    public:
	typedef VInstanceHolder<F3> IH_3;
	typedef typename IH_3::StorageType IH_3_ST;

    //  Construction
    public:
	VTuple_3 (F1 iF1, F2 iF2, F3 iF3) : BaseClass (iF1, iF2), m_iF3(iF3) {
	}
	VTuple_3 () {
	}

    //  Destruction
    public:
	~VTuple_3 () {
	}

    //  Access
    public:
	unsigned int cardinality () const {
	    return 3;
	}
	unsigned int cardinality_() const {
	    return cardinality ();
	}

	IH_3_ST const &f3 () const {
	    return m_iF3;
	}
	IH_3_ST &f3 () {
	    return m_iF3;
	}

    //  Serialization
    public:
	VcaSerializer *serializer (VcaSerializer *pCaller, unsigned int xField) {
	    switch (xField) {
	    case 0:
		return m_iF1.serializer (pCaller);
	    case 1:
		return m_iF2.serializer (pCaller);
	    case 2:
		return m_iF3.serializer (pCaller);
	    default:
		break;
	    }
	    return NilOf (VcaSerializer*);
	}

    //  State
    protected:
	USING (BaseClass::m_iF1)
	USING (BaseClass::m_iF2)
	IH_3 m_iF3;
    };


    /**********************
     *----  VTuple_4  ----*
     **********************/

    template <class F1,class F2,class F3,class F4> class VTuple_4 : public VTuple_3<F1,F2,F3> {
    //  Family
    public:
	typedef VTuple_4<F1,F2,F3,F4> ThisClass;
	typedef VTuple_3<F1,F2,F3> BaseClass;

    //  Instance Holder
    public:
	typedef VInstanceHolder<F4> IH_4;
	typedef typename IH_4::StorageType IH_4_ST;

    //  Construction
    public:
	VTuple_4 (F1 iF1, F2 iF2, F3 iF3, F4 iF4) : BaseClass (iF1, iF2, iF3), m_iF4(iF4) {
	}
	VTuple_4 () {
	}

    //  Destruction
    public:
	~VTuple_4 () {
	}

    //  Access
    public:
	unsigned int cardinality () const {
	    return 4;
	}
	unsigned int cardinality_() const {
	    return cardinality ();
	}

	IH_4_ST const &f4 () const {
	    return m_iF4;
	}
	IH_4_ST &f4 () {
	    return m_iF4;
	}

    //  Serialization
    public:
	VcaSerializer *serializer (VcaSerializer *pCaller, unsigned int xField) {
	    switch (xField) {
	    case 0:
		return m_iF1.serializer (pCaller);
	    case 1:
		return m_iF2.serializer (pCaller);
	    case 2:
		return m_iF3.serializer (pCaller);
	    case 3:
		return m_iF4.serializer (pCaller);
	    default:
		break;
	    }
	    return NilOf (VcaSerializer*);
	}

    //  State
    protected:
	USING (BaseClass::m_iF1)
	USING (BaseClass::m_iF2)
	USING (BaseClass::m_iF3)
	IH_4 m_iF4;
    };


    /**********************
     *----  VTuple_5  ----*
     **********************/

    template <class F1,class F2,class F3,class F4,class F5>
    class VTuple_5 : public VTuple_4<F1,F2,F3,F4> {
    //  Family
    public:
	typedef VTuple_5<F1,F2,F3,F4,F5> ThisClass;
	typedef VTuple_4<F1,F2,F3,F4> BaseClass;

    //  Instance Holder
    public:
	typedef VInstanceHolder<F5> IH_5;
	typedef typename IH_5::StorageType IH_5_ST;

    //  Construction
    public:
	VTuple_5 (F1 iF1, F2 iF2, F3 iF3, F4 iF4, F5 iF5)
	    : BaseClass (iF1, iF2, iF3, iF4), m_iF5(iF5)
	{
	}
	VTuple_5 () {
	}

    //  Destruction
    public:
	~VTuple_5 () {
	}

    //  Access
    public:
	unsigned int cardinality () const {
	    return 5;
	}
	unsigned int cardinality_() const {
	    return cardinality ();
	}

	IH_5_ST const &f5 () const {
	    return m_iF5;
	}
	IH_5_ST &f5 () {
	    return m_iF5;
	}

    //  Serialization
    public:
	VcaSerializer *serializer (VcaSerializer *pCaller, unsigned int xField) {
	    switch (xField) {
	    case 0:
		return m_iF1.serializer (pCaller);
	    case 1:
		return m_iF2.serializer (pCaller);
	    case 2:
		return m_iF3.serializer (pCaller);
	    case 3:
		return m_iF4.serializer (pCaller);
	    case 4:
		return m_iF5.serializer (pCaller);
	    default:
		break;
	    }
	    return NilOf (VcaSerializer*);
	}

    //  State
    protected:
	USING (BaseClass::m_iF1)
	USING (BaseClass::m_iF2)
	USING (BaseClass::m_iF3)
	USING (BaseClass::m_iF4)
	IH_5 m_iF5;
    };


    /**********************
     *----  VTuple_6  ----*
     **********************/

    template <class F1,class F2,class F3,class F4,class F5,class F6>
    class VTuple_6 : public VTuple_5<F1,F2,F3,F4,F5> {
    //  Family
    public:
	typedef VTuple_6<F1,F2,F3,F4,F5,F6> ThisClass;
	typedef VTuple_5<F1,F2,F3,F4,F5> BaseClass;

    //  Instance Holder
    public:
	typedef VInstanceHolder<F6> IH_6;
	typedef typename IH_6::StorageType IH_6_ST;

    //  Construction
    public:
	VTuple_6 (F1 iF1, F2 iF2, F3 iF3, F4 iF4, F5 iF5, F6 iF6)
	    : BaseClass (iF1, iF2, iF3, iF4, iF5), m_iF6(iF6)
	{
	}
	VTuple_6 () {
	}

    //  Destruction
    public:
	~VTuple_6 () {
	}

    //  Access
    public:
	unsigned int cardinality () const {
	    return 6;
	}
	unsigned int cardinality_() const {
	    return cardinality ();
	}

	IH_6_ST const &f6 () const {
	    return m_iF6;
	}
	IH_6_ST &f6 () {
	    return m_iF6;
	}

    //  Serialization
    public:
	VcaSerializer *serializer (VcaSerializer *pCaller, unsigned int xField) {
	    switch (xField) {
	    case 0:
		return m_iF1.serializer (pCaller);
	    case 1:
		return m_iF2.serializer (pCaller);
	    case 2:
		return m_iF3.serializer (pCaller);
	    case 3:
		return m_iF4.serializer (pCaller);
	    case 4:
		return m_iF5.serializer (pCaller);
	    case 5:
		return m_iF6.serializer (pCaller);
	    default:
		break;
	    }
	    return NilOf (VcaSerializer*);
	}

    //  State
    protected:
	USING (BaseClass::m_iF1)
	USING (BaseClass::m_iF2)
	USING (BaseClass::m_iF3)
	USING (BaseClass::m_iF4)
	USING (BaseClass::m_iF5)
	IH_6 m_iF6;
    };


    /**********************
     *----  VTuple_7  ----*
     **********************/

    template <class F1,class F2,class F3,class F4,class F5,class F6,class F7>
    class VTuple_7 : public VTuple_6<F1,F2,F3,F4,F5,F6> {
    //  Family
    public:
	typedef VTuple_7<F1,F2,F3,F4,F5,F6,F7> ThisClass;
	typedef VTuple_6<F1,F2,F3,F4,F5,F6> BaseClass;

    //  Instance Holder
    public:
	typedef VInstanceHolder<F7> IH_7;
	typedef typename IH_7::StorageType IH_7_ST;

    //  Construction
    public:
	VTuple_7 (F1 iF1, F2 iF2, F3 iF3, F4 iF4, F5 iF5, F6 iF6, F7 iF7)
	    : BaseClass (iF1, iF2, iF3, iF4, iF5, iF6), m_iF7(iF7)
	{
	}
	VTuple_7 () {
	}

    //  Destruction
    public:
	~VTuple_7 () {
	}

    //  Access
    public:
	unsigned int cardinality () const {
	    return 7;
	}
	unsigned int cardinality_() const {
	    return cardinality ();
	}

	IH_7_ST const &f7 () const {
	    return m_iF7;
	}
	IH_7_ST &f7 () {
	    return m_iF7;
	}

    //  Serialization
    public:
	VcaSerializer *serializer (VcaSerializer *pCaller, unsigned int xField) {
	    switch (xField) {
	    case 0:
		return m_iF1.serializer (pCaller);
	    case 1:
		return m_iF2.serializer (pCaller);
	    case 2:
		return m_iF3.serializer (pCaller);
	    case 3:
		return m_iF4.serializer (pCaller);
	    case 4:
		return m_iF5.serializer (pCaller);
	    case 5:
		return m_iF6.serializer (pCaller);
	    case 6:
		return m_iF7.serializer (pCaller);
	    default:
		break;
	    }
	    return NilOf (VcaSerializer*);
	}

    //  State
    protected:
	USING (BaseClass::m_iF1)
	USING (BaseClass::m_iF2)
	USING (BaseClass::m_iF3)
	USING (BaseClass::m_iF4)
	USING (BaseClass::m_iF5)
	USING (BaseClass::m_iF6)
	IH_7 m_iF7;
    };


    /**********************
     *----  VTuple_8  ----*
     **********************/

    template <class F1,class F2,class F3,class F4,class F5,class F6,class F7,class F8>
    class VTuple_8 : public VTuple_7<F1,F2,F3,F4,F5,F6,F7> {
    //  Family
    public:
	typedef VTuple_8<F1,F2,F3,F4,F5,F6,F7,F8> ThisClass;
	typedef VTuple_7<F1,F2,F3,F4,F5,F6,F7> BaseClass;

    //  Instance Holder
    public:
	typedef VInstanceHolder<F8> IH_8;
	typedef typename IH_8::StorageType IH_8_ST;

    //  Construction
    public:
	VTuple_8 (F1 iF1, F2 iF2, F3 iF3, F4 iF4, F5 iF5, F6 iF6, F7 iF7, F8 iF8)
	    : BaseClass (iF1, iF2, iF3, iF4, iF5, iF6, iF7), m_iF8(iF8)
	{
	}
	VTuple_8 () {
	}

    //  Destruction
    public:
	~VTuple_8 () {
	}

    //  Access
    public:
	unsigned int cardinality () const {
	    return 8;
	}
	unsigned int cardinality_() const {
	    return cardinality ();
	}

	IH_8_ST const &f8 () const {
	    return m_iF8;
	}
	IH_8_ST &f8 () {
	    return m_iF8;
	}

    //  Serialization
    public:
	VcaSerializer *serializer (VcaSerializer *pCaller, unsigned int xField) {
	    switch (xField) {
	    case 0:
		return m_iF1.serializer (pCaller);
	    case 1:
		return m_iF2.serializer (pCaller);
	    case 2:
		return m_iF3.serializer (pCaller);
	    case 3:
		return m_iF4.serializer (pCaller);
	    case 4:
		return m_iF5.serializer (pCaller);
	    case 5:
		return m_iF6.serializer (pCaller);
	    case 6:
		return m_iF7.serializer (pCaller);
	    case 7:
		return m_iF8.serializer (pCaller);
	    default:
		break;
	    }
	    return NilOf (VcaSerializer*);
	}

    //  State
    protected:
	USING (BaseClass::m_iF1)
	USING (BaseClass::m_iF2)
	USING (BaseClass::m_iF3)
	USING (BaseClass::m_iF4)
	USING (BaseClass::m_iF5)
	USING (BaseClass::m_iF6)
	USING (BaseClass::m_iF7)
	IH_8 m_iF8;
    };
}


#endif
