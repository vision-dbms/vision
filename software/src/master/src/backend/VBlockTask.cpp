/*****  VBlockTask Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VBlockTask.h"

/************************
 *****  Supporting  *****
 ************************/

#include "rslang.h"

#include "vmagic.h"
#include "verrdef.h"

#include "RTclosure.h"
#include "RTdictionary.h"
#include "RTstring.h"
#include "RTvstore.h"

#include "V_VString.h"

#include "IOMDriver.h"

#include "VSelector.h"

#include "VSymbol.h"
#include "VSymbolImplementation.h"


/***************************************
 ***************************************
 *****                             *****
 *****  VBlockTask::Decompilation  *****
 *****                             *****
 ***************************************
 ***************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VBlockTask::Decompilation::Decompilation (
    VBlockTask const *pBlockTask, VCall const *pContext
) {
    RSLANG_Decompile (
        m_iSource, pBlockTask->blockCPD (), &m_iOffset, VCall::CallerPCOf (pContext)
    );
    m_pPrefix = m_iSource;

    //  Adjust the output for cosmetic reasons, ...
    m_iOffset += strspn (m_pPrefix + m_iOffset, " \t\n");
    if ('\n' == *m_pPrefix) {
	m_pPrefix++;
	if (m_iOffset > 0)
	    m_iOffset--;
    }

    //  Locate the end of the line containing the selector, ...
    char *pSuffix = (char *)strchr (m_pPrefix + m_iOffset, '\n');

    //  ... break the source into a prefix and suffix after that line, ...
    if (pSuffix) {
	*pSuffix++ = '\0';
	m_pSuffix = pSuffix;
    }
    else m_pSuffix = m_pPrefix + strlen (m_pPrefix);

    //  ... display a marker under the selector, ...
    char const *pBOL = strrchr (m_pPrefix, '\n');
    m_iIndent = pBOL ? (m_iOffset - (unsigned int)(pBOL - m_pPrefix)) : (m_iOffset + 1);
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

void VBlockTask::decompilation (VString& rString, VCall const *pContext) const {
    Decompilation iDecompilation (this, pContext);

    iDecompilation.display (rString.storage (iDecompilation.displayLength ()));
}

void VBlockTask::decompilationPrefix (VString& rString, VCall const *pContext) const {
    Decompilation iDecompilation (this, pContext);
    rString.setTo (iDecompilation.prefix ());
}

void VBlockTask::decompilationSuffix (VString& rString, VCall const *pContext) const {
    Decompilation iDecompilation (this, pContext);
    rString.setTo (iDecompilation.suffix ());
}

unsigned int VBlockTask::decompilationIndent (VCall const *pContext) const {
    Decompilation iDecompilation (this, pContext);
    return iDecompilation.indent ();
}
unsigned int VBlockTask::decompilationOffset (VCall const *pContext) const {
    Decompilation iDecompilation (this, pContext);
    return iDecompilation.offset ();
}


/*********************
 *********************
 *****  Display  *****
 *********************
 *********************/

size_t VBlockTask::Decompilation::displayLength () const {
    return strlen (m_pPrefix) + strlen (m_pSuffix) + m_iIndent + 4;
}

void VBlockTask::Decompilation::display (char *pBuffer) const {
    sprintf (pBuffer, "%s\n%*c\n%s\n", m_pPrefix, m_iIndent, '^', m_pSuffix);
}

void VBlockTask::Decompilation::produceReportFor (VTask const *pTask) const {
    pTask->report ("%s\n%*c\n%s\n", m_pPrefix, m_iIndent, '^', m_pSuffix);
}


/************************
 ************************
 *****              *****
 *****  VBlockTask  *****
 *****              *****
 ************************
 ************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VBlockTask);

/************************
 ************************
 *****  Meta Maker  *****
 ************************
 ************************/

template class Vsi_getr_c<VBlockTask, VString>;
template class Vsi_f0_c<VBlockTask, unsigned int>;
template class Vsi_p0_c<VBlockTask>;

void VBlockTask::MetaMaker () {
    //  Explicit casts needed to satisfy Solaris compiler, ...
    static Vsi_getr_c<VBlockTask, VString> const
	si_blockText	((void (VBlockTask::*) (VString&) const)&VBlockTask::decompilation),
	si_blockPrefix	((void (VBlockTask::*) (VString&) const)&VBlockTask::decompilationPrefix),
	si_blockSuffix	((void (VBlockTask::*) (VString&) const)&VBlockTask::decompilationSuffix);

    static Vsi_f0_c<VBlockTask, unsigned int> const
	si_blockIndent	((unsigned int (VBlockTask::*) () const)&VBlockTask::decompilationIndent),
	si_blockOffset	((unsigned int (VBlockTask::*) () const)&VBlockTask::decompilationOffset);

    static Vsi_p0_c<VBlockTask> const
	si_dumpByteCodes((void (VBlockTask::*) () const)&VBlockTask::dumpByteCodes);

    CSym ("isABlockTask"	)->set (RTT, &g_siTrue);

    CSym ("blockText"		)->set (RTT, &si_blockText);
    CSym ("blockPrefix"		)->set (RTT, &si_blockPrefix);
    CSym ("blockSuffix"		)->set (RTT, &si_blockSuffix);

    CSym ("blockIndent"		)->set (RTT, &si_blockIndent);
    CSym ("blockOffset"		)->set (RTT, &si_blockOffset);

    CSym ("dumpByteCodes"	)->set (RTT, &si_dumpByteCodes);
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VBlockTask::VBlockTask (
    ConstructionData const& rTCData, rtBLOCK_Handle *pBlock, bool bBound
) : VTask (rTCData), m_iByteCodeScanner (pBlock), m_pBlockKOT (pBlock->KOT ()) {
    BlockTaskCount++;

    if (bBound)
	return;

/*++++  ... create the new store, ...  ++++*/
    DSC_Descriptor& rCurrent = getCurrent ();

    rtDICTIONARY_Handle::Reference pDictionary (pBlock->getDictionary ());

    rtPTOKEN_Handle::Reference pCurrentPPT (
	new rtPTOKEN_Handle (pBlock->ScratchPad (), cardinality ())
    );
    M_CPD *pSuperPointer = rCurrent.pointerCPD (pCurrentPPT);

    Vdd::Store::Reference pCurrentStore (
	rtVSTORE_NewCluster (pCurrentPPT, pDictionary, rCurrent.store (), pSuperPointer)
    );

/*++++
 *  ... replace ^current with a descriptor for the store just created, ...
 *+++*/
    rCurrent.clear ();
    rCurrent.constructCorrespondence (ptoken (), pCurrentStore);

/*++++  ... and clean up.  ++++*/
    pSuperPointer->release ();
}


/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

/*****************
 *****  Run  *****
 *****************/

void VBlockTask::run () {
    switch (m_xStage) {
    case Stage_Unrun:
	if (pausedOnEntry ())
	    return;
	/*****  NO BREAK  *****/

    case Stage_Entry:
	//  Fetch the first/next instruction, ...
	m_iByteCodeScanner.fetchInstruction ();

	m_xStage = Stage_Running;
	/*****  NO BREAK  *****/

    case Stage_Running:
	while (m_iByteCodeScanner.fetchedByteCodeIsSuspendSafe () || runnable ()) {

	//  Decode the current instruction, ...
	    switch (m_iByteCodeScanner.fetchedDispatchCase ()) {

    /******************************
     *  Undefined and NoOp Codes  *
     ******************************/

	    default:
    /*---------------------------------------------------------------------------*
     * Signal an error if an unrecognized instruction is encountered.  If the
     * error handler returns, execution will resume at the current program counter.
     *---------------------------------------------------------------------------*/
		raiseUnimplementedByteCodeException ();
		break;


	    case VByteCodeDescriptor::DispatchCase_NoOp:
    /*---------------------------------------------------------------------------*
     * Do nothing if the byte code fetched is a NoOp (i.e., one of the pad, tag,
     * or comment byte codes).
     *---------------------------------------------------------------------------*/
		break;


    /**************************************
     *  Parameter Acquisition Byte Codes  *
     **************************************/

	    case VByteCodeDescriptor::DispatchCase_AcquireParameter: {
		    VSelector iParameterSelector (m_iByteCodeScanner.fetchedSelectorName ());
		    acquireParameter (iParameterSelector);
		}
		if (pausedOnEntry ())
		    return;

		//  Make sure stage is Running if not pausedOnEntry, ...
		m_xStage = Stage_Running;
		break;

	    case VByteCodeDescriptor::DispatchCase_AcquireKnownParameter: {
		    VSelector iParameterSelector (m_iByteCodeScanner.fetchedSelectorIndex ());
		    acquireParameter (iParameterSelector);
		}
		if (pausedOnEntry ())
		    return;

		//  Make sure stage is Running if not pausedOnEntry, ...
		m_xStage = Stage_Running;
		break;

    /******************************
     *  Message Setup Byte Codes  *
     ******************************/

	    case VByteCodeDescriptor::DispatchCase_UnaryMessage:
	    case VByteCodeDescriptor::DispatchCase_KeywordMessage:
		beginMessageCall (m_iByteCodeScanner);
		break;

	    case VByteCodeDescriptor::DispatchCase_KnownMessage:
		beginMessageCall (m_iByteCodeScanner.fetchedSelectorIndex ());
		break;

	    case VByteCodeDescriptor::DispatchCase_OptimizedMessage:
		beginMessageCall (m_iByteCodeScanner.fetchedAssociatedKSI ());
		break;

    /*********************************
     *  Message Dispatch Byte Codes  *
     *********************************/

	    case VByteCodeDescriptor::DispatchCase_DispatchMagic:
		commitRecipient (m_iByteCodeScanner.fetchedMagicWord ());
		break;

	    case VByteCodeDescriptor::DispatchCase_Dispatch:
		commitRecipient ();
		break;


    /************************************
     *  Parameter Completion Byte Code  *
     ************************************/

	    case VByteCodeDescriptor::DispatchCase_NextMessageParameter:
		commitParameter ();
		break;

    /************************************
     *  Value and Intension Byte Codes  *
     ************************************/

	    case VByteCodeDescriptor::DispatchCase_Value:
		commitCall ();
		break;

	    case VByteCodeDescriptor::DispatchCase_Intension:
		commitCall (Return_Intension);
		break;


    /*****************************************
     *  Unary Byte Code Sequence Byte Codes  *
     *****************************************/

	    case VByteCodeDescriptor::DispatchCase_KnownUnaryValue:
		beginMessageCall(m_iByteCodeScanner.fetchedSelectorIndex ());
		commitRecipient	(m_iByteCodeScanner.fetchedMagicWord ());
		commitCall	();
		break;

	    case VByteCodeDescriptor::DispatchCase_KnownUnaryIntension:
		beginMessageCall(m_iByteCodeScanner.fetchedSelectorIndex ());
		commitRecipient	(m_iByteCodeScanner.fetchedMagicWord ());
		commitCall	(Return_Intension);
		break;

	    case VByteCodeDescriptor::DispatchCase_UnaryValue:
		beginMessageCall(m_iByteCodeScanner);
		commitRecipient	(m_iByteCodeScanner.fetchedMagicWord ());
		commitCall	();
		break;

	    case VByteCodeDescriptor::DispatchCase_UnaryIntension:
		beginMessageCall(m_iByteCodeScanner);
		commitRecipient	(m_iByteCodeScanner.fetchedMagicWord ());
		commitCall	(Return_Intension);
		break;

	    case VByteCodeDescriptor::DispatchCase_WrapupUnaryValue:
		commitRecipient ();
		commitCall	();
		break;

	    case VByteCodeDescriptor::DispatchCase_WrapupUnaryIntension:
		commitRecipient ();
		commitCall	(Return_Intension);
		break;


    /***********************
     *  Store Magic Value  *
     ***********************/

	    case VByteCodeDescriptor::DispatchCase_StoreMagic:
		switch (m_iByteCodeScanner.fetchedMagicWord ()) {
		case VMagicWord_Current:
		    loadDucWithCurrent ();
		    break;
		case VMagicWord_Self:
		case VMagicWord_Here:
		case VMagicWord_Super:
		case VMagicWord_Ground:
		    loadDucWithSelf ();
		    break;
		case VMagicWord_My:
		    loadDucWithMy ();
		    break;
		case VMagicWord_Local:
		    loadDucWithLocal ();
		    break;
		case VMagicWord_Global:
		    loadDucWithCoerced (ENVIR_KDsc_TheTLE);
		    break;
		case VMagicWord_Tmp:
		    loadDucWithCoerced (ENVIR_KDsc_TheTmpTLE);
		    break;
		case VMagicWord_Date:
		    loadDucWithDate ();
		    break;
		case VMagicWord_Today:
		    loadDucWithCoerced (ENVIR_KTemporalContext_Today);
		    break;
		default:
		    raiseUnimplementedMagicWordException ();
		    break;
		}
		setStepPoint ();
		break;


    /**********************
     *  Store Byte Codes  *
     **********************/

	    case VByteCodeDescriptor::DispatchCase_StoreContext:
		loadDucWithIdentity (
		    new rtCLOSURE_Handle (
			m_pBlockContext, m_iByteCodeScanner.fetchedBlockHandle ()
		    )
		);
		setStepPoint ();
		break;

	    case VByteCodeDescriptor::DispatchCase_StoreInteger:
		loadDucWithInteger (m_pBlockKOT, m_iByteCodeScanner.fetchedInteger ());
		setStepPoint ();
		break;

	    case VByteCodeDescriptor::DispatchCase_StoreDouble:
		loadDucWithDouble (m_pBlockKOT, m_iByteCodeScanner.fetchedDouble ());
		setStepPoint ();
		break;

	    case VByteCodeDescriptor::DispatchCase_StoreSelector:
		loadDucWithSelector (
		    blockHandle (), m_iByteCodeScanner.fetchedSelectorIndex ()
		);
		setStepPoint ();
		break;

	    case VByteCodeDescriptor::DispatchCase_StoreKnownSelector:
		loadDucWithSelector (
		    m_pBlockKOT, m_iByteCodeScanner.fetchedSelectorIndex ()
		);
		setStepPoint ();
		break;

	    case VByteCodeDescriptor::DispatchCase_StoreString:
		loadDucWithInteger (
		    blockHandle (), blockHandle ()->TheStringClass ().PTokenHandle (),
		    m_iByteCodeScanner.fetchedLiteral ()
		);
		setStepPoint ();
		break;

	    case VByteCodeDescriptor::DispatchCase_StoreNoValue:
		loadDucWithUndefined (m_pBlockKOT->TheNoValueClass);
		setStepPoint ();
		break;


    /********************
     *  Exit Byte Code  *
     ********************/

	    case VByteCodeDescriptor::DispatchCase_Exit:
		if (!pausedOnExit ())
		    exit ();
		return;

    /**************************************
     *  End of Byte Code Dispatch Switch  *
     **************************************/
	    }

    /************************************
     *  End of Byte Code Dispatch Loop  *
     ************************************/

	//  Fetch the next instruction, ...
	    m_iByteCodeScanner.fetchInstruction ();
	    if (m_fAtStepPoint)
		m_fAtStepPoint = m_iByteCodeScanner.fetchedDispatchCase () != VByteCodeDescriptor::DispatchCase_Exit;
	}
	break;

    default:
	exit ();
	break;
    }
}


/***********************************
 *****  Parameter Acquisition  *****
 ***********************************/

/*---------------------------------------------------------------------------
 *****  Internal method to access and store the next parameter to a block.
 *
 *  Argument:
 *	rParameterSelector	- a selector holding the parameter's name.
 *
 *---------------------------------------------------------------------------*/
void VBlockTask::acquireParameter (VSelector const& rSelector) {
//  Obtain ^current's store, ...
    rtVSTORE_Handle *pCurrentStore = static_cast<rtVSTORE_Handle*>(current ().store ());

//  Find the parameter, ...
    unsigned int xPropertySlot;
    if (Vdd::Store::DictionaryLookup_FoundProperty != pCurrentStore->lookup (rSelector, xPropertySlot))
	raiseInstructionException ("Parameter Not Found");

/*****  ... obtain the parameter value, ...  *****/
    loadDucWithNextParameter (&rSelector);

/*****  ... as a normalized vector, ...  *****/
    normalizeDuc ();

    VDescriptor::Type xInitialDucType = duc().type ();
    rtVECTOR_Handle::Reference pVector (convertDucToVector ());

/*****  ... update the vector's ptoken, ...  *****/
    pVector->setPTokenTo (pCurrentStore);

/*****  ... install the vector in the block's store, ...  *****/
    pCurrentStore->StoreReference (rtVSTORE_CPx_Column + xPropertySlot, static_cast<VContainerHandle*>(pVector));

/*****  ... clean up, ...  *****/
    duc().clear ();
}

void rtVECTOR_Handle::setPTokenTo (rtVSTORE_Handle *pStore) {
    EnableModifications ();
    StoreReference (ptokenPOP (), pStore, pStore->ptokenPOP ());
}


/*************************************
 *************************************
 *****  Display and Description  *****
 *************************************
 *************************************/

/******************************
 *----  Byte Code Dumper  ----*
 ******************************/

void VBlockTask::dumpByteCodes (VCall const *pContext) const {
    VCPDReference pBlockCPD (0, m_iByteCodeScanner.blockCPD ()->GetCPD ());

    void const *const pBC = pContext
	? rtBLOCK_CPD_ByteCodeVector (pBlockCPD) + pContext->callerPC ()
	: rtBLOCK_CPD_PC (pBlockCPD);

    VByteCodeScanner iByteCodeScanner (pBlockCPD);

    display ("Byte Codes:\n");
    bool notDone = true;
    do {
	void const *const pBCNow = rtBLOCK_CPD_PC (pBlockCPD);
	if (pBC == pBCNow)
	    display (">");

	iByteCodeScanner.fetchInstruction (true);
	if (iByteCodeScanner.fetchedByteCodeIsSuspendSafe ())
	    display ("S");

	switch (iByteCodeScanner.fetchedParameterStructure ()) {
	default:
	case VByteCodeDescriptor::ParameterStructure_Unknown:
	    notDone = false;
	    break;
	case VByteCodeDescriptor::ParameterStructure_None:
	    if (iByteCodeScanner.fetchedByteCode () == VByteCodeDescriptor::ByteCode_Exit)
		notDone = false;
	case VByteCodeDescriptor::ParameterStructure_Pad:
	case VByteCodeDescriptor::ParameterStructure_PadS:
	case VByteCodeDescriptor::ParameterStructure_TagS:
	case VByteCodeDescriptor::ParameterStructure_TagInt:
	    display ("\t%s\n", iByteCodeScanner.fetchedByteCodeName ());
	    break;
	case VByteCodeDescriptor::ParameterStructure_Tag:
	    display (
		"\t%s %u\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedLiteral		()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_KSI:
	    display (
		"\t%s %u[%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedSelectorIndex	(),
		KS__ToString (iByteCodeScanner.fetchedSelectorIndex ())
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_KSI_MWI:
	    display (
		"\t%s %u[%s] %u[%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedSelectorIndex	(),
		KS__ToString (iByteCodeScanner.fetchedSelectorIndex ()),
		iByteCodeScanner.fetchedLiteral		(),
		MAGIC_MWIToString (iByteCodeScanner.fetchedLiteral ())
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_MWI:
	    display (
		"\t%s %u[%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedLiteral		(),
		MAGIC_MWIToString (iByteCodeScanner.fetchedLiteral ())
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSI:
	case VByteCodeDescriptor::ParameterStructure_SSIS:
	    display (
		"\t%s %u[%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedSelectorIndex	(),
		iByteCodeScanner.fetchedSelectorName	()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSI_Valence:
	case VByteCodeDescriptor::ParameterStructure_SSIS_Valence:
	    display (
		"\t%s %u[%s] %u\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedSelectorIndex	(),
		iByteCodeScanner.fetchedSelectorName	(),
		iByteCodeScanner.fetchedSelectorValence	()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSI_MWI:
	case VByteCodeDescriptor::ParameterStructure_SSIS_MWI:
	    display (
		"\t%s %u[%s] %u[%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedSelectorIndex	(),
		iByteCodeScanner.fetchedSelectorName	(),
		iByteCodeScanner.fetchedLiteral		(),
		MAGIC_MWIToString (iByteCodeScanner.fetchedLiteral ())
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_SLI:
	case VByteCodeDescriptor::ParameterStructure_SLIS:
	    display (
		"\t%s %u[%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedLiteral		(),
		iByteCodeScanner.fetchedStringText	()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_PLI:
	case VByteCodeDescriptor::ParameterStructure_PLIS:
	    display (
		"\t%s %u\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedLiteral		()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_Int:
	    display (
		"\t%s %d\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedInteger		()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_Double:
	    display (
		"\t%s %g\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedDouble		()
	    );
    	    break;
    	}
    } while (notDone);
}


void VBlockTask::reportInfo (unsigned int xLevel, VCall const *pContext) const {
    reportInfoHeader (xLevel);

    //  Create the decompilation, ...
    Decompilation iDecompilation (this, pContext);

    //  ... and display it:
    iDecompilation.produceReportFor (this);
}

void VBlockTask::reportTrace () const {
    reportTraceHeader ("B");

    /*****  Get a new CPD to get freshly initialized CPD pointers,  ...  *****/
    VCPDReference pBlock  (blockCPD (), -1, RTYPE_C_Block);

    VString iSource;
    RSLANG_Decompile (iSource, pBlock);

    report (iSource);
    report ("\n");
}

char const *VBlockTask::description () const {
    return call()->selector_().messageName ();
}


/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

void VBlockTask::raiseUnimplementedByteCodeException () {
    raiseInstructionException ("Unimplemented Byte Code");
}

void VBlockTask::raiseUnimplementedMagicWordException () {
    raiseInstructionException (
	string (
	    "Unimplemented Magic Word '%s'",
	    MAGIC_MWIToString (m_iByteCodeScanner.fetchedMagicWord ())
	)
    );
}

void VBlockTask::raiseInstructionException (char const *pMessage) {
    raiseException (
	EC__VirtualMachineError,
	"VMACHINE: Fault In '%s[%u]': %s",
	m_iByteCodeScanner.fetchedByteCodeName (),
	m_iByteCodeScanner.fetchedByteCode (),
	pMessage
    );
}
