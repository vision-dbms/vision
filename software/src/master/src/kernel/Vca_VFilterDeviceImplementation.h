#ifndef Vca_VFilterDeviceImplementation_Interface
#define Vca_VFilterDeviceImplementation_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VDeviceImplementation.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VBSProducer.h"
#include "Vca_VBSConsumer.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VOutputSequencer;

    class Vca_API VFilterDeviceImplementation : public VDeviceImplementation {
	DECLARE_FAMILY_MEMBERS (VFilterDeviceImplementation, VDeviceImplementation);

    /***********************************
     *----  Device Implementation  ----*
     ***********************************/
    public:
	typedef ThisClass DeviceImplementation;

    /******************************
     *----  Device Reference  ----*
     ******************************/
    public:
	typedef VReference<ThisClass> DeviceReference;

    /************************
     *----  Device Use  ----*
     ************************/
    public:
	typedef VDeviceImplementation::AbstractUse AbstractUse;
	class Use : public AbstractUse {
	    DECLARE_FAMILY_MEMBERS (Use, AbstractUse);

	//  Friends
	    friend class VFilterDeviceImplementation;

	//  Construction
	protected:
	    Use (VReferenceable *pContainer, DeviceImplementation *pDevice) : BaseClass (pContainer), m_pDevice (pDevice) {
	    }

	//  Destruction
	protected:
	    ~Use () {
	    }

	//  Access
	public:
	    DeviceImplementation *device () const {
		return m_pDevice;
	    }

	//  State
	protected:
	    DeviceReference const m_pDevice;
	};
	friend class Use;

    /*************************
     *----  Device Use_  ----*
     *************************/
    public:
	template<class BS> class Use_ : public Use {
	    DECLARE_FAMILY_MEMBERS (Use_<BS>, Use);

	//  Area
	public:
	    typedef typename BS::Area Area;

	//  Construction
	protected:
	    Use_(VReferenceable *pContainer, DeviceImplementation *pDevice) : BaseClass (pContainer, pDevice) {
	    }

	//  Destruction
	protected:
	    ~Use_() {
	    }

	//  Access
	public:
	    Area const &area () const {
		return m_iArea;
	    }
	    void const *areaBase () const {
		return m_iArea.base ();
	    }
	    size_t areaSize () const {
		return m_iArea.size ();
	    }

	//  Startup
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    bool start (VkStatus &rStatus, Area const &rArea) {
		m_iArea.setTo (rArea);
		return m_pDevice->start (rStatus, this);
	    }

	//  State
	private:
	    Area m_iArea;
	};
	typedef Use_<VBSProducer> BSGet;
	typedef Use_<VBSConsumer> BSPut;

	friend class Use_<VBSProducer>;
	friend class Use_<VBSConsumer>;

    /******************************
     *----  Pipe Source Data  ----*
     ******************************/
    public:
	class PipeSourceData {
	//  Construction
	public:
	    PipeSourceData () {
	    }
	//  Destruction
	public:
	    ~PipeSourceData () {
	    }
	};

    /**************************
     *----  Factory Data  ----*
     **************************/
    public:
	class FactoryData {
	    friend class VFilterDeviceImplementation;

	//  Construction
	public:
	    FactoryData (
		PipeSourceData const &rData, VBSProducer *pBSProducer, VBSConsumer *pBSConsumer
	    );

	//  Destruction
	public:
	    ~FactoryData ();

	//  Access/Use
	private:
	    virtual VCohort *cohort_() {
		return cohort ();
	    }
	public:
	    VBSProducer *bsProducer () const {
		return m_pBSProducer;
	    }
	    VBSConsumer *bsConsumer () const {
		return m_pBSConsumer;
	    }
	    VCohort *cohort () {
		return VCohort::Here ();
	    }
	    VOutputSequencer *newOutputSequencer () const;

	//  State
	protected:
	    VReference<VBSProducer> m_pBSProducer;
	    VReference<VBSConsumer> m_pBSConsumer;
	};

    /****************************
     *----  The Main Event  ----*
     ****************************/

    //  Construction
    protected:
	VFilterDeviceImplementation (VReferenceable *pContainer, FactoryData &rOther);

    //  Destruction
    protected:
	~VFilterDeviceImplementation ();

    //  Device Use
    private:
	virtual bool start (VkStatus &rStatus, BSGet *pGet) = 0;
	virtual bool start (VkStatus &rStatus, BSPut *pPut) = 0;
    };
}


#endif
