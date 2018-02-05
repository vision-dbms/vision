#ifndef VBlockTask_Interface
#define VBlockTask_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTask.h"

#include "VByteCodeScanner.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/*---------------------------------------------------------------------------
 *  Block Activation Field Descriptions:
 *	m_iByteCodeScanner	- the address of the byte code scanner used
 *				  to execute this block.
 *---------------------------------------------------------------------------
 */
class VBlockTask : public VTask {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VBlockTask, VTask);

//  Types
public:
    class Decompilation : public VTransient {
    //  Construction
    public:
	Decompilation (VBlockTask const* pBlockTask, VCall const* pContext = 0);

    //  Destruction
    public:
	~Decompilation () {
	}

    //  Access
    public:
	char const* prefix () const { return m_pPrefix; }
	char const* suffix () const { return m_pSuffix; }

	unsigned int offset () const { return m_iOffset; }
	unsigned int indent () const { return m_iIndent; }

    //  Display
    public:
	size_t displayLength () const;

	void display (char* pBuffer) const;

	void produceReportFor (VTask const* pTask) const;

    //  State
    protected:
	VString		m_iSource;
	char const	*m_pPrefix;
	char const	*m_pSuffix;
	unsigned int	 m_iOffset;
	unsigned int	 m_iIndent;
    };
    friend class Decompilation;

//  Meta Maker
protected:
    static void MetaMaker ();

//  Construction
public:
    VBlockTask (
	ConstructionData const &rTCData, rtBLOCK_Handle *pBlock, bool bBound = false
    );

//  Destruction
private:
    ~VBlockTask () {
    }

//  Access
protected:
    M_CPD* blockCPD () const {
	return m_iByteCodeScanner.blockCPD ();
    }
    rtBLOCK_Handle *blockHandle () const {
	return m_iByteCodeScanner.blockHandle ();
    }

    virtual void decompilation (VString& rString, VCall const* pContext) const OVERRIDE;
    void decompilation (VString& rString) const {
	decompilation (rString, 0);
    }

    virtual void decompilationPrefix (VString& rString, VCall const* pContext) const OVERRIDE;
    void decompilationPrefix (VString& rString) const {
	decompilationPrefix (rString, 0);
    }

    virtual void decompilationSuffix (VString& rString, VCall const* pContext) const OVERRIDE;
    void decompilationSuffix (VString& rString) const {
	decompilationSuffix (rString, 0);
    }

    virtual unsigned int decompilationIndent (VCall const* pContext) const OVERRIDE;
    unsigned int decompilationIndent () const {
	return decompilationIndent (0);
    }

    virtual unsigned int decompilationOffset (VCall const* pContext) const OVERRIDE;
    unsigned int decompilationOffset () const {
	return decompilationOffset (0);
    }

//  Execution...
protected:
    virtual void run () OVERRIDE;

//  ... Call Commit
    void commitCall (ReturnCase xReturnCase = Return_Value) {
	VTask::commitCall (xReturnCase, m_iByteCodeScanner.bcOffset ());
    }

//  ... Parameter Acquisition
protected:
    void acquireParameter (VSelector const& rParameterSelector);

//  ... Pause Control
protected:
    bool pausedOnEntry () {
	return 0 == m_xNextParameter && VTask::pausedOnEntry ();
    }

//  Display
public:
    virtual void dumpByteCodes (VCall const* pContext) const OVERRIDE;
    void dumpByteCodes () const {
	dumpByteCodes (0);
    }

    virtual void reportInfo	(unsigned int xLevel, VCall const* pContext = 0) const OVERRIDE;
    virtual void reportTrace	() const OVERRIDE;

    virtual char const* description () const OVERRIDE;

//  Exception Generation
protected:
    void raiseUnimplementedByteCodeException ();
    void raiseUnimplementedMagicWordException ();

    void raiseInstructionException (char const* pMessage);

//  State
protected:
    VByteCodeScanner	m_iByteCodeScanner;
    M_KOT*	const	m_pBlockKOT;
};


#endif
