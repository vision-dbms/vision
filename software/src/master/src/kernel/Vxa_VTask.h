#ifndef Vxa_VTask_Interface
#define Vxa_VTask_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VActivity.h"

#include "Vxa_VRolePlayer.h"
#include "Vxa_VCallData.h"
#include "Vxa_VTaskCursor.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_ICaller2.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCallType1;
    class VCallType2;

    class VCallType1Importer;
    class VCallType2Importer;

    class VCollectableObject;

    class VError;

/*****************
 *----
 *
 *  Why two inheritance paths to VRolePlayer? (mjc)
 *
 *    class Vxa_API VTask : public VRolePlayer, public Vca::VActivity {
 *
 *----
 *****************/
    class Vxa_API VTask : public Vca::VActivity {
	DECLARE_ABSTRACT_RTTLITE (VTask, VRolePlayer);

	friend class VCallType1;
	friend class VCallType2;
        friend class VCollectableObject;

    //  LaunchRequest
    public:
	class LaunchRequest;
	friend class LaunchRequest;

    //****************************************************************
    //  VTask::RemoteControl
    public:
        class Vxa_API RemoteControl : public VRolePlayer {
            DECLARE_CONCRETE_RTTLITE (RemoteControl, VRolePlayer);

            friend class VResultBuilder;

        //  Construction
        public:
            RemoteControl (ICaller2 *pRemoteInterface);

        //  Destruction
        private:
            ~RemoteControl ();

        //  Access
        public:
            cardinality_t suspendCount () const {
                return m_cSuspensions;
            }

        //  Control
        public:
            bool suspend ();
            bool resume  ();
        private:
            bool sendRemoteSuspend () const;
            bool sendRemoteResume  () const;

        //  State
        private:
            ICaller2::Reference const m_pRemoteInterface;
            cardinality_t             m_cSuspensions;
        };

    //****************************************************************
    //  VTask

    //  Construction
    protected:
	VTask (VCallData const &rCallData);

    //  Destruction
    protected:
	~VTask ();

    //  Access
    public:
	cardinality_t cardinality () const {
	    return m_iCallData.cardinality ();
	}
        VCollection *cluster () const {
            return m_iCallData.cluster ();
        }
        VCollectableObject *clusterObject (object_reference_t xObject) const {
            return m_iCallData.clusterObject (xObject);
        }
        VClass *clusterType () const {
            return m_iCallData.clusterType ();
        }
        cardinality_t parameterCount () const {
            return m_iCallData.parameterCount ();
        }
        bool invokedIntensionally () const {
            return m_iCallData.invokedIntensionally ();
        }
        VString const &selectorName () const {
            return m_iCallData.selectorName ();
        }
        VString const &selectorComponent (cardinality_t xComponent) const {
            return m_iCallData.selectorComponent (xComponent);
        }
        bool selectorComponent (VString &rComponent, cardinality_t xComponent) const {
            return m_iCallData.selectorComponent (rComponent, xComponent);
        }
	VTaskCursor *cursor () const {
	    return m_pCursor;
	}
        cardinality_t cursorPosition () const {
            return m_pCursor->position ();
        }

    //  Query
    public:
	bool isAlive () const {
	    return m_pCursor->isAlive ();
	}
	bool moreToDo () const {
	    return m_pCursor->moreToDo ();
	}

    //  Execution
    protected:
	virtual bool startUsing (VCallType1Importer &rImporter) = 0;
	virtual bool startUsing (VCallType2Importer &rImporter) = 0;

    private:
	bool startWithMonitorUsing (VCallType1Importer &rImporter);
	bool startWithMonitorUsing (VCallType2Importer &rImporter);

    public:
	void onErrorEvent (VError *pError);

    public:
	void suspend () {
	    m_cSuspensions++;
	}
	bool resume () {
	    return --m_cSuspensions > 0 || launch ();
	}
    private:
	bool launch ();
    public:
        bool launchInThreadPool ();

    public:
	void kill () {
	    m_pCursor->kill ();
	}
    private:
	virtual bool run () = 0;
    public:
	bool runWithMonitor ();

    //  Iteration
    public:
	void first () {
	    m_pCursor->first ();
	}
	void next () {
	    m_pCursor->next ();
	}

    //  Remote Control
    public:
        RemoteControl *getRemoteControl (Vxa::ICaller2 *pRemoteInterface);
        void wrapupRemoteControl ();

    //  State
    private:
	VCallData const m_iCallData;
	VTaskCursor::Reference m_pCursor;
	unsigned int m_cSuspensions;
        RemoteControl::Reference m_pRemoteControl;
    };
}


#endif
