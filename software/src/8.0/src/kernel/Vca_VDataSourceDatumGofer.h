#ifndef Vca_VDataSourceDatumGofer_Interface
#define Vca_VDataSourceDatumGofer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VInstanceGofer.h"

#include "Vca_VInstanceOfInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VReceiver.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {

/**************************************************************************************************************************
 *----  template <typename Val_T, typename Var_T = typename VTypePattern<Val_T>::var_t> class VDataSourceDatumGofer  -----*
 **************************************************************************************************************************/

    template <typename Val_T, typename Var_T = Val_T> class VDataSourceDatumGofer : public VInstanceGofer<Val_T,Var_T> {
	typedef BOGUS_TYPENAME VDataSourceDatumGofer<Val_T,Var_T> this_t;
	typedef BOGUS_TYPENAME VInstanceGofer<Val_T,Var_T> base_t;
	DECLARE_CONCRETE_RTTLITE (this_t, base_t);

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef Var_T var_t;

	typedef IDataSource<val_t>	IValueProducer;
	typedef IVReceiver<val_t>	IValueConsumer;
	typedef VReceiver<ThisClass,val_t> ValueReceiver;

    //  Construction
    public:
	template <typename source_t> VDataSourceDatumGofer (
	    source_t *pIValueProducer
	) : m_pIValueProducer (pIValueProducer) {
	}

    //  Destruction
    protected:
	~VDataSourceDatumGofer () {
	}

    //  Callbacks
    private:
	void onNeed () {
	//  Get the value producer...
	    m_pIValueProducer.materializeFor (this);
	    BaseClass::onNeed ();
	}
	void onData () {
	//  and when the value producer is available, ask it for a value...
	    typename IValueConsumer::Reference pIValueConsumer (
		new ValueReceiver (this, &ThisClass::onValue, &ThisClass::onErroR)
	    );
	    m_pIValueProducer.value()->Supply (pIValueConsumer);
	}
	void onValue (ValueReceiver *pReceiver, val_t iValue) {
	    BaseClass::setTo (iValue);
	}
	void onErroR (ValueReceiver *pReceiver, IError *pIError, VString const &rMessage) {
	    BaseClass::onError (pIError, rMessage);
	}

    //  State
    private:
	VInstanceOfInterface<IValueProducer> m_pIValueProducer;
    };
}


#endif
