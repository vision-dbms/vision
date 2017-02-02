#ifndef VTopTaskBase_Interface
#define VTopTaskBase_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTask.h"

#include "VDescriptor.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT VTopTaskBase : public VTask {
    DECLARE_ABSTRACT_RTT (VTopTaskBase, VTask);

//  Context Class
public:
    class Context : public VTask::Context {
    //  Friends
	friend class VTopTaskBase;

    //  Construction
    protected:
	Context (VTopTaskBase* pController);

    //  Destruction
    protected:
	~Context ();

    //  State
    protected:
	ControlPoints m_iControlPoints;
    };

//  Meta Maker
protected:
    static void MetaMaker ();

//  Construction
protected:
    VTopTaskBase (VTask* pSpawningTask, IOMDriver* pChannel);

//  Destruction
protected:
    ~VTopTaskBase ();

//  Execution Control
public:
    void start ();
protected:
    void exit ();

//  State
protected:
    Context		m_iContext;
    VDescriptor		m_iDatum;
    bool		m_fDatumAvailable;
};


#endif
