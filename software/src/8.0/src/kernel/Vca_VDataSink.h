#ifndef VDataSink_Interface
#define VDataSink_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <class Actor, class Datum> class VDataSink : public VRolePlayer {
    public:
	typedef VDataSink<Actor,Datum> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t, VRolePlayer);

//  Aliases
    public:
	typedef typename Actor::Reference ActorReference;

	typedef IVReceiver<Datum> IDataSink;

	typedef void (Actor::*data_sink_t) (Datum iDatum);
	typedef void (Actor::*error_sink_t) (IError *pInterface, VString const &rMessage);

//  Construction
    public:
	VDataSink (
	    Actor *pActor, data_sink_t pDataSink, error_sink_t pErrorSink
	) : m_pIDataSink (this), m_pActor (pActor), m_pDataSink (pDataSink), m_pErrorSink (pErrorSink) {
	}

//  Destruction
    protected:
	~VDataSink () {
	}

//  Update
    public:
	void setActorTo (Actor *pActor) {
	    m_pActor.setTo (pActor);
	}
	void setDataSinkTo (data_sink_t pDataSink) {
	    m_pDataSink = pDataSink;
	}
	void setErrorSinkTo (error_sink_t pDataSink) {
	    m_pErrorSink = pDataSink;
	}

    //  Roles
    public:
	using BaseClass::getRole;

    //  IDataSink Role
    private:
	VRole<ThisClass,IDataSink> m_pIDataSink;
    public:
	void getRole (typename IDataSink::Reference& rpRole) {
	    m_pIDataSink.getRole (rpRole);
	}

    //  IDataSink Callbacks
    public:
	void OnData (IDataSink* pRole, Datum iDatum) {
	    (m_pActor.referent()->*m_pDataSink) (iDatum);
	}

    //  IClient Callbacks
    private:
	void OnError_(IError *pInterface, VString const &rMessage) {
	    (m_pActor.referent()->*m_pErrorSink) (pInterface, rMessage);
	}

//  State
    protected:
	ActorReference	m_pActor;
	data_sink_t	m_pDataSink;
	error_sink_t	m_pErrorSink;
    };
} /* namespace Vca */

#endif
