#ifndef Vca_IDataSource_NRK
#define Vca_IDataSource_NRK

/*************************
 *****  Definitions  *****
 *************************/

#ifndef Vca_IDataSource
#define Vca_IDataSource extern
#endif

namespace Vca {
    extern Vca_API VINTERFACE_SYMBOL_DECLARATION (IDataSource);

    /**
     * Generic data source for data of a particular type.
     *
     * @tparam Datum the data type provided by this interface.
     */
    template <class Datum> class ABSTRACT IDataSource : public IVUnknown {
	DECLARE_ABSTRACT_RTTLITE (IDataSource<Datum>, IVUnknown);
	VINTERFACE_DEFINITION_MEMBERS (IDataSource);
	typedef Datum DataType;

	VINTERFACE_ROLE_NEST
	    VINTERFACE_ROLE_1 (Supply, IVReceiver<DataType>*);
	VINTERFACE_ROLE_NEST_END

    //  Data Delivery
        /**
         * Requests of this interface's implementor that a datum is supplied to the given receiver.
         *
         * @param p1 the receiver by which the supplied datum should be returned.
         */
	VINTERFACE_METHOD_1 (Supply, IVReceiver<DataType>*);
    };

    typedef IDataSource<IVUnknown*> IObjectSource;
}


/*********************
 *****  Members  *****
 *********************/

namespace Vca {
    /***********************
     *****  Type Info  *****
     ***********************/

    template <class Datum> VINTERFACE_TYPEINFOHOLDER_DEFINITION(IDataSource<Datum>);

    template <class Datum>
    VINTERFACE_TYPEINFO(IDataSource)<Datum>
    VINTERFACE_TYPEINFO_PARAMETERS (IDataSource, 1)
	VINTERFACE_TYPEINFO_PARAMETER (Datum)
    VINTERFACE_TYPEINFO_END

    /*************************
     *****  Member Info  *****
     *************************/

    template <class Datum>
    typename IDataSource<Datum>::VINTERFACE_MEMBERTYPE(Supply)
    IDataSource<Datum>::VINTERFACE_MEMBERINFO (Supply) =
    typename IDataSource<Datum>::VINTERFACE_MEMBERTYPE(Supply)::Initializer (
	"Supply", 0, &IDataSource<Datum>::VINTERFACE_MEMBERIMPL(Supply)
    );
}


/****************************
 *****  Instantiations  *****
 ****************************/

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(Vca_IDataSource_Instantiations)

#define Vca_IDataSource_InstantiationsForPODT(t)\
    Vca_IDataSource template class Vca_API Vca::IDataSource<t >

#define Vca_IDataSource_InstantiationsForREFT(t)\
    Vca_IDataSource_InstantiationsForPODT (t const&)

#define Vca_IDataSource_InstantiationsForArrayOf(t)\
    Vca_IDataSource_InstantiationsForREFT (VkDynamicArrayOf<t >)

#ifdef Vca_IDataSource_Instantiations1
Vca_IDataSource_InstantiationsForPODT (Vca::VBSConsumer*);
Vca_IDataSource_InstantiationsForPODT (Vca::VBSProducer*);
Vca_IDataSource_InstantiationsForPODT (VTypeInfo*);
#endif  // defined(Vca_IDataSource_Instantiations1)

#ifdef Vca_IDataSource_Instantiations2
Vca_IDataSource_InstantiationsForPODT (bool);
Vca_IDataSource_InstantiationsForPODT (Vca::F32);
Vca_IDataSource_InstantiationsForPODT (Vca::F64);
Vca_IDataSource_InstantiationsForPODT (Vca::S08);
Vca_IDataSource_InstantiationsForPODT (Vca::S16);
Vca_IDataSource_InstantiationsForPODT (Vca::S32);
Vca_IDataSource_InstantiationsForPODT (Vca::S64);
Vca_IDataSource_InstantiationsForPODT (Vca::U08);
Vca_IDataSource_InstantiationsForPODT (Vca::U16);
Vca_IDataSource_InstantiationsForPODT (Vca::U32);
Vca_IDataSource_InstantiationsForPODT (Vca::U64);
#endif  // defined(Vca_IDataSource_Instantiations2)

#ifdef Vca_IDataSource_Instantiations1
Vca_IDataSource_InstantiationsForREFT (uuid_t);
Vca_IDataSource_InstantiationsForREFT (V::VBlob);
Vca_IDataSource_InstantiationsForREFT (VString);
Vca_IDataSource_InstantiationsForREFT (V::VTime);

Vca_IDataSource_InstantiationsForArrayOf (VString);
Vca_IDataSource_InstantiationsForPODT (Vca::IDataSource<VString const&>*);
#endif  // defined(Vca_IDataSource_Instantiations1)

#endif	// defined(USING_HIDDEN_DEFAULT_VISIBILITY) ...


#endif
