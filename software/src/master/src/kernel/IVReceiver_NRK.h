#ifndef Vca_IVReceiver_NRK
#define Vca_IVReceiver_NRK

/*************************
 *****  Definitions  *****
 *************************/

#ifndef Vca_IVReceiver
#define Vca_IVReceiver extern
#endif

extern Vca_API VINTERFACE_SYMBOL_DECLARATION (IVReceiver);

template <class Datum> class ABSTRACT IVReceiver : public Vca::IClient {
    DECLARE_ABSTRACT_RTTLITE (IVReceiver<Datum>, Vca::IClient);
    VINTERFACE_DEFINITION_MEMBERS (IVReceiver);
    typedef Datum DataType;

    VINTERFACE_ROLE_NEST
	VINTERFACE_ROLE_1 (OnData, DataType);
    VINTERFACE_ROLE_NEST_END

//  Data Delivery
    VINTERFACE_METHOD_1 (OnData, DataType);
};

namespace Vca {
    typedef IVReceiver<IVUnknown*> IObjectSink;
}

/*********************
 *****  Members  *****
 *********************/

/***********************
 *----  Type Info  ----*
 ***********************/

template <class Datum> VINTERFACE_TYPEINFOHOLDER_DEFINITION(IVReceiver<Datum>);

template <class Datum>
VINTERFACE_TYPEINFO(IVReceiver)<Datum>
VINTERFACE_TYPEINFO_PARAMETERS (IVReceiver, 1)
    VINTERFACE_TYPEINFO_PARAMETER (Datum)
VINTERFACE_TYPEINFO_END

/*************************
 *----  Member Info  ----*
 *************************/

template <class Datum>
typename IVReceiver<Datum>::VINTERFACE_MEMBERTYPE(OnData)
IVReceiver<Datum>::VINTERFACE_MEMBERINFO (OnData) =
typename IVReceiver<Datum>::VINTERFACE_MEMBERTYPE(OnData)::Initializer (
    "OnData", 0, &IVReceiver<Datum>::VINTERFACE_MEMBERIMPL(OnData)
);


/****************************
 *****  Instantiations  *****
 ****************************/

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(Vca_IVReceiver_Instantiations)

#define Vca_IVReceiver_InstantiationsForPODT(t)\
    Vca_IVReceiver template class Vca_API IVReceiver<t >

#define Vca_IVReceiver_InstantiationsForREFT(t)\
    Vca_IVReceiver_InstantiationsForPODT (t const&)

#define Vca_IVReceiver_InstantiationsForArrayOf(t)\
    Vca_IVReceiver_InstantiationsForREFT (VkDynamicArrayOf<t >)

Vca_IVReceiver_InstantiationsForPODT (bool);
Vca_IVReceiver_InstantiationsForPODT (Vca::F32);
Vca_IVReceiver_InstantiationsForPODT (Vca::F64);
Vca_IVReceiver_InstantiationsForPODT (Vca::S08);
Vca_IVReceiver_InstantiationsForPODT (Vca::S16);
Vca_IVReceiver_InstantiationsForPODT (Vca::S32);
Vca_IVReceiver_InstantiationsForPODT (__int64);
Vca_IVReceiver_InstantiationsForPODT (Vca::U08);
Vca_IVReceiver_InstantiationsForPODT (Vca::U16);
Vca_IVReceiver_InstantiationsForPODT (Vca::U32);
Vca_IVReceiver_InstantiationsForPODT (Vca::U64);

Vca_IVReceiver_InstantiationsForPODT (Vca::VBSConsumer*);
Vca_IVReceiver_InstantiationsForPODT (Vca::VBSProducer*);
Vca_IVReceiver_InstantiationsForPODT (VTypeInfo*);

Vca_IVReceiver_InstantiationsForREFT (V::uuid_t);
Vca_IVReceiver_InstantiationsForREFT (V::VBlob);
Vca_IVReceiver_InstantiationsForREFT (VString);
Vca_IVReceiver_InstantiationsForREFT (V::VTime);

Vca_IVReceiver_InstantiationsForArrayOf (Vca::U32);
Vca_IVReceiver_InstantiationsForArrayOf (Vca::U64);
Vca_IVReceiver_InstantiationsForArrayOf (Vca::F64);
Vca_IVReceiver_InstantiationsForArrayOf (V::VTime);
Vca_IVReceiver_InstantiationsForArrayOf (VString);
Vca_IVReceiver_InstantiationsForPODT (Vca::IDataSource<VString const&>*);

#endif


#endif
