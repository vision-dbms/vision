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

    void decompilation (VString& rString, VCall const* pContext) const;
    void decompilation (VString& rString) const {
	decompilation (rString, 0);
    }

    void decompilationPrefix (VString& rString, VCall const* pContext) const;
    void decompilationPrefix (VString& rString) const {
	decompilationPrefix (rString, 0);
    }

    void decompilationSuffix (VString& rString, VCall const* pContext) const;
    void decompilationSuffix (VString& rString) const {
	decompilationSuffix (rString, 0);
    }

    unsigned int decompilationIndent (VCall const* pContext) const;
    unsigned int decompilationIndent () const {
	return decompilationIndent (0);
    }

    unsigned int decompilationOffset (VCall const* pContext) const;
    unsigned int decompilationOffset () const {
	return decompilationOffset (0);
    }

//  Execution...
protected:
    void run ();

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
    void dumpByteCodes (VCall const* pContext) const;
    void dumpByteCodes () const {
	dumpByteCodes (0);
    }

    void reportInfo	(unsigned int xLevel, VCall const* pContext = 0) const;
    void reportTrace	() const;

    char const* description () const;

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
