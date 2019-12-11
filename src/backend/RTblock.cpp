/*****  Block Representation Type Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName rtBLOCK

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "rslang.h"
#include "selector.h"

#include "verr.h"
#include "vfac.h"
#include "vmagic.h"
#include "vstdio.h"
#include "vutil.h"

#include "VByteCodeScanner.h"

#include "RTdictionary.h"
#include "RTstring.h"
#include "RTvstore.h"

/*****  Self  *****/
#include "RTblock.h"

/*****  Private Macros  *****/
#define ComputeSize(cpd)\
    ((V::pointer_t) rtBLOCK_CPDBase(cpd) + M_CPD_Size(cpd))

#define FindOffset(b, e)\
    ((V::pointer_t) (e) - (V::pointer_t) (b))

#define FindAddress(b, o)\
    ((V::pointer_t) (b) + (o))


/*************************************
 *************************************
 *****                           *****
 *****  rtBLOCK_Handle::Strings  *****
 *****                           *****
 *************************************
 *************************************/

rtBLOCK_Handle::Strings::Strings () {
}

rtBLOCK_Handle::Strings::~Strings () {
}

bool rtBLOCK_Handle::Strings::setTo (rtBLOCK_Handle *pStoreHandle) {
    bool bValid = IsntNil (pStoreHandle);
    if (bValid) {
	m_pStoreHandle.setTo (pStoreHandle);
	m_pCharacters = pStoreHandle->addressOfStringSpace ();
    }
    else {
	m_pStoreHandle.clear ();
	m_pCharacters = 0;
    }
    return bValid;
}


/****************************
 ****************************
 *****                  *****
 *****  rtBLOCK_Handle  *****
 *****                  *****
 ****************************
 ****************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (rtBLOCK_Handle);

/********************************
 ********************************
 *****  Handle Maintenance  *****
 ********************************
 ********************************/

void rtBLOCK_Handle::AdjustContainerPointers (M_CPreamble *pNewAddress, bool bWritable) {
    BaseClass::AdjustContainerPointers (pNewAddress, bWritable);
    m_pBlockHeader = typecastContent ();
}

/******************************
 ******************************
 *****  Canonicalization  *****
 ******************************
 ******************************/

bool rtBLOCK_Handle::getCanonicalization_(rtVSTORE_Handle::Reference &rpStore, DSC_Pointer const &rPointer) {
    rtPTOKEN_Handle::Reference pRPT (rPointer.RPT ());
    rpStore.setTo (
	static_cast<rtVSTORE_Handle*>(pRPT->GetBlockEquivalentClassFromPToken ().ObjectHandle ())
    );
    return true;
}


/**************************
 **************************
 *****  Store Access  *****
 **************************
 **************************/

rtDICTIONARY_Handle *rtBLOCK_Handle::getDictionary_(DSC_Pointer const &rPointer) const {
    rtPTOKEN_Handle::Reference pRPT (rPointer.RPT ());
    M_KOT *pKOT = pRPT->KOT ();
    return static_cast<rtDICTIONARY_Handle*>(
	(pRPT->Names (pKOT->TheStringPTokenHandle ())
	    ? pKOT->TheStringClassDictionary
	    : pKOT->TheSelectorClassDictionary
	).ObjectHandle ()
    );
}

rtPTOKEN_Handle *rtBLOCK_Handle::getPToken_() const {
    return TheStringClass ().PTokenHandle ();
}

/*******************************
 *******************************
 *****  Store Maintenance  *****
 *******************************
 *******************************/

/*---------------------------------------------------------------------------
 *****  Routine to align a block -- specifically its method dictionary
 *
 *  Returns:
 *	true if alignment performed, false if not
 *
 *****/
bool rtBLOCK_Handle::align () {
    bool result; {
	rtDICTIONARY_Handle::Reference pDictionary (getDictionary ());
	result = pDictionary->alignAll ();
    }

/*****  Recursively Align any nested blocks  *****/
    unsigned int nNestedBlocks = nestedBlockCount ();
    for (unsigned int xNestedBlock = 0; xNestedBlock < nNestedBlocks; xNestedBlock++) {
	rtBLOCK_Handle::Reference pNestedBlock (nestedBlockHandle (xNestedBlock));
	result = pNestedBlock->align () || result;
    }

    return result;
}


/*---------------------------------------------------------------------------
 *****  Routine to initialize a standard CPD
 *
 *  Argument:
 *	cpd	- the address of a standard Block CPD which
 *		  is to be initialized.
 *
 *  Returns:
 *	'cpd'
 *
 *****/
PublicFnDef void rtBLOCK_InitStdCPD (M_CPD* cpd) {
    rtBLOCK_BlockType *a = rtBLOCK_CPDBase (cpd);

    /****   set the local method dictionary pointer     ****/
    rtBLOCK_CPD_LocalEnvironment (cpd) = &(rtBLOCK_localEnvironmentPOP (a));

    /****   set the address of the byte code vector	****/
    rtBLOCK_CPD_PC (cpd) = rtBLOCK_CPD_ByteCodeVector (cpd) =
	(unsigned char *) FindAddress (a, rtBLOCK_byteCodeVector (a));

    /****   set the address of the string space 	****/
    rtBLOCK_CPD_StringSpace (cpd) = (V::pointer_t) FindAddress (a, rtBLOCK_stringSpace (a));

    /****   set the address of the physical literal vector	****/
    rtBLOCK_CPD_PLiteralVector (cpd) =
	(rtBLOCK_PLVectorType *) FindAddress (a, rtBLOCK_PLVector (a));
    rtBLOCK_CPD_PLVectorElement (cpd) =
	(M_POP*) ((V::pointer_t)rtBLOCK_CPD_PLiteralVector (cpd) +
	(rtBLOCK_PLVector (a) ==  0 ? 0 : sizeof (int)));

    /****   set the address of the selector if it is in string space   ****/
    rtBLOCK_CPD_SelectorName (cpd) = rtBLOCK_SelectorType (a) == rtBLOCK_C_DefinedSelector
	? (char*)(rtBLOCK_CPD_StringSpace (cpd) + rtBLOCK_SelectorIndex (a))
	: NilOf (char*);

    rtBLOCK_CPDEnd (cpd) = ComputeSize (cpd);
}


/*---------------------------------------------------------------------------
 *****  Routine to Create a new block.
 *
 *  Returns:
 *	A Standard Block CPD for the Block created.
 *
 *  Note:
 *	A block CPD starts out with 12 pointers:
 *	    1	- the base of the block
 *	    2	- the end of the block
 *	    3	- the local environment pop
 *	    4	- the program counter index into the byte code vector
 *	    5   - the program counter index that can be moved.
 *	    6	- the string space
 *	    7	- the string space element pointer
 *	    8	- the physical literal vector
 *	    9	- the physical literal vector element pointer
 *	    10	- the evaled literal entry vector
 *	    11	- the evaled literal entry vector element pointer
 *	    12  - the selector name pointer
 *
 *****/
PublicFnDef M_CPD *rtBLOCK_New (M_ASD *pContainerSpace) {
    M_CPD *cpd = pContainerSpace->CreateContainer (
	RTYPE_C_Block, rtBLOCK_BlockSize + rtBLOCK_InitialBV_Size
    )->NewCPD ();
    rtBLOCK_BlockType *blockPtr = rtBLOCK_CPDBase (cpd);

	/**  set the compiler version.... 0 for now  **/
    rtBLOCK_compilerVersion (blockPtr) = rtBLOCK_CompilerVersion;

    /****	Fill in the Block's Preamble	****/
    rtBLOCK_localEnvironment (blockPtr) =
	FindOffset (blockPtr, &(rtBLOCK_localEnvironmentPOP (blockPtr)));
    rtBLOCK_byteCodeVector (blockPtr) = rtBLOCK_BlockSize;

    /****	The following pieces are not present at initialization
		time, so set to '0' for now				****/
    rtBLOCK_stringSpace (blockPtr) =
    rtBLOCK_PLVector (blockPtr) =
    rtBLOCK_ELEVector (blockPtr) = 0;

    /****	initialize selector type to NoSelector	****/
    rtBLOCK_SelectorType (blockPtr) = rtBLOCK_C_NoSelector;

    rtBLOCK_InitStdCPD (cpd);
    cpd->constructReference (rtBLOCK_CPx_LocalEnv);

    return cpd;
}


/*****************************************
 *****  Compiler Interface Routines  *****
 *****************************************/

/*---------------------------------------------------------------------------
 *****  Routine to concatenate the string space onto the end of
 *****  the block in physical memory.
 *
 *  Arguments:
 *	cpd 		- the address of a standard rtBLOCK CPD.
 *	stringSpace	- the address of the string Space to be concatenated.
 *	stringSpaceSize - the size of the string space in bytes.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtBLOCK_AppendStringSpace (
    M_CPD* cpd, char const *stringSpace, int stringSpaceSize
) {
    char *oldend = rtBLOCK_CPDEnd (cpd);
    int adjustment = (int)((pointer_size_t)oldend % 4);
    int addition = adjustment == 0 ?  0 : 4 - adjustment;
    int modnum = stringSpaceSize % 4;
    int newsize = modnum == 0 ? stringSpaceSize : stringSpaceSize + (4 - modnum);

    /****  Assigning values to p and oldend after growing the   ****
     ****  container is necessary since GrowContainer may move  ****
     ****  the container.					****/

    cpd->GrowContainer (newsize);
    rtBLOCK_BlockType* p = rtBLOCK_CPDBase (cpd);
    oldend = rtBLOCK_CPDEnd (cpd);

    /****	Set StringSpace Preamble slot	****/
    rtBLOCK_stringSpace (p) = FindOffset (p, oldend + addition);

    /****	Reset end of block pointer	****/
    rtBLOCK_CPDEnd (cpd) = (V::pointer_t)p + M_CPD_Size (cpd);

    char *pp = oldend + addition;
    oldend = rtBLOCK_CPDEnd (cpd);

    /****  Pad end of stringSpace with '\0's	****/
    if (stringSpaceSize > 0) {
	for (adjustment = 0; adjustment < 4; adjustment++)
	    *--oldend = '\0';
    }

    memcpy (pp, stringSpace, stringSpaceSize);
}


/*---------------------------------------------------------------------------
 *****  Routine to append the physical literal vector onto the end of the
 *****   block.
 *
 *  Arguments:
 *	cpd 		- the address of a block CPD.
 *	physicalLiteralVector
 *			- the address of the physical literal vector to
 *			  be appended.
 *	size		- the size of the physical literal vector in pops.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtBLOCK_AppendPLVector (
    M_CPD* cpd, M_CPD* physicalLiteralVector[], int size
) {
    int realsize = (sizeof (int)) + (sizeof (M_POP) * size);

    cpd->GrowContainer (realsize);
    char *oldend = rtBLOCK_CPDEnd (cpd);

    rtBLOCK_BlockType *p  = rtBLOCK_CPDBase (cpd);
    rtBLOCK_PLVectorType *plv = (rtBLOCK_PLVectorType *)oldend;

    rtBLOCK_PLVector (p) =  FindOffset (p, oldend);
    rtBLOCK_PLVector_Count (plv) = size;

    rtBLOCK_CPDEnd (cpd) = (V::pointer_t) (oldend + realsize);

    rtBLOCK_CPD_PLVectorElement (cpd) = rtBLOCK_PLVector_POP (plv);
    for (int i = 0; i < size; i++) {
	cpd->constructReference (rtBLOCK_CPx_PLVElement);
	cpd->StoreReference (rtBLOCK_CPx_PLVElement, physicalLiteralVector[i]);
	rtBLOCK_CPD_PLVectorElement (cpd)++;
    }
}


/*---------------------------------------------------------------------------
 *****  Routine to concatenate the evaled entry literal vector to the
 *****  end of the block.
 *
 *  Arguments:
 *	cpd 		- the address of a block CPD.
 *	evaledLEVector	- the address of the evaled literal entry vector.
 *	size		- the size of the vector in integers.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PublicFnDef void rtBLOCK_AppendELEVector (M_CPD* cpd, int *evaledLEVector, int size) {
    int realsize = (sizeof (int) * (size + 1));
    cpd->GrowContainer (realsize);

    char const *oldend = rtBLOCK_CPDEnd (cpd);
    rtBLOCK_BlockType* p = rtBLOCK_CPDBase (cpd);
    rtBLOCK_ELEVectorType *elv = (rtBLOCK_ELEVectorType *)oldend;

    rtBLOCK_ELEVector (p) = FindOffset (p, oldend);
    rtBLOCK_ELEVector_Count (elv) = size;

    rtBLOCK_CPDEnd (cpd) = (V::pointer_t) (oldend + realsize);

    int *pp, *p1;
    for (p1 = size + (pp = rtBLOCK_ELEVector_Points (elv)); pp < p1; pp++) {
	*pp = *evaledLEVector++;
    }
}


/*******************************************************************
 *****  Standard Representation Type Handler Service Routines  *****
 *******************************************************************/

/*************************************
 *****  Block Garbage Collector  *****
 *************************************/

/*---------------------------------------------------------------------------
 *****  Routine to reclaim the contents of a container.
 *
 *  Arguments:
 *	ownerASD		- the ASD of the space which owns the container
 *	preambleAddress		- the address of the container to be reclaimed.
 *
 *****/
PrivateFnDef void ReclaimContainer (
    M_ASD* ownerASD, M_CPreamble const *preambleAddress
) {
    rtBLOCK_BlockType const *block = (rtBLOCK_BlockType const*)(preambleAddress + 1);

/*****  Save the local environment...  *****/
    ownerASD->Release (&rtBLOCK_localEnvironmentPOP (block));

/*****  ...and the physical literal vector.  *****/
    if (rtBLOCK_PLVector (block)) {
        rtBLOCK_PLVectorType *plv = (rtBLOCK_PLVectorType *) FindAddress (
	    block, rtBLOCK_PLVector (block)
	);
	ownerASD->Release (rtBLOCK_PLVector_POP (plv), rtBLOCK_PLVector_Count (plv));
    }
}


/******************
 *  Save Routine  *
 ******************/

/*---------------------------------------------------------------------------
 *****  Routine to save the references of a container.
 *****/
bool rtBLOCK_Handle::PersistReferences () {
/*****  Save the local environment...  *****/
    rtBLOCK_BlockType *block = typecastContent ();
    bool result = Persist (&rtBLOCK_localEnvironmentPOP (block));

/*****  ...and the physical literal vector.  *****/
    if (result && rtBLOCK_PLVector (block)) {
        rtBLOCK_PLVectorType *plv = (rtBLOCK_PLVectorType *) FindAddress (
	    block, rtBLOCK_PLVector (block)
	);
	result = Persist (rtBLOCK_PLVector_POP (plv), rtBLOCK_PLVector_Count (plv));
    }
    return result;
}


/********************************************
 *****  Standard Block Marking Routine  *****
 ********************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to 'mark' containers referenced by a Block
 *
 *  Arguments:
 *	pSpace			- the address of the ASD for the object
 *				  space in which this container resides.
 *	pContainer		- the address of the preamble of the
 *				  container being traversed.
 *
 *  Returns:
 *	NOTHING - Executed for side effect only.
 *
 *****/
PrivateFnDef void MarkContainers (M_ASD::GCVisitBase* pGCV, M_ASD* pSpace, M_CPreamble const *pContainer) {
    rtBLOCK_BlockType const *block = (rtBLOCK_BlockType const*)(pContainer + 1);

/*****  Save the local environment...  *****/
    pGCV->Mark (pSpace, &rtBLOCK_localEnvironmentPOP (block));

/*****  ...and the physical literal vector.  *****/
    if (rtBLOCK_PLVector (block)) {
        rtBLOCK_PLVectorType *plv = (rtBLOCK_PLVectorType *) FindAddress (
	    block, rtBLOCK_PLVector (block)
	);
	pGCV->Mark (pSpace, rtBLOCK_PLVector_POP (plv), rtBLOCK_PLVector_Count (plv));
    }
}


/********************
 *  Print Function  *
 ********************/

PrivateFnDef void PrintByteCodes (M_CPD* pBlockCPD) {
    VByteCodeScanner iByteCodeScanner (pBlockCPD);
    
    IO_printf ("Byte Codes:\n");
    int notDone = true;
    do {
	iByteCodeScanner.fetchInstruction (true);
    
	switch (iByteCodeScanner.fetchedParameterStructure ())
	{
	default:
	case VByteCodeDescriptor::ParameterStructure_Unknown:
	    /* IO_printf ("\tUnknown byte code %u at %08X\n", **pc, *pc); */
	    notDone = false;
	    break;
	case VByteCodeDescriptor::ParameterStructure_None:
	    if (iByteCodeScanner.fetchedByteCode () == VByteCodeDescriptor::ByteCode_Exit)
		notDone = false;
	case VByteCodeDescriptor::ParameterStructure_Pad:
	case VByteCodeDescriptor::ParameterStructure_PadS:
	case VByteCodeDescriptor::ParameterStructure_TagS:
	case VByteCodeDescriptor::ParameterStructure_TagInt:
	    IO_printf ("\t%s\n", iByteCodeScanner.fetchedByteCodeName ());
	    break;
	case VByteCodeDescriptor::ParameterStructure_Tag:
	    IO_printf (
		"\t%s %u\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedLiteral		()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_KSI:
	    IO_printf (
		"\t%s %u[%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedSelectorIndex	(),
		KS__ToString (iByteCodeScanner.fetchedSelectorIndex ())
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_KSI_MWI:
	    IO_printf (
		"\t%s %u[%s] %u[^%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedSelectorIndex	(),
		KS__ToString (iByteCodeScanner.fetchedSelectorIndex ()),
		iByteCodeScanner.fetchedLiteral		(),
		MAGIC_MWIToString (iByteCodeScanner.fetchedLiteral ())
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_MWI:
	    IO_printf (
		"\t%s %u[^%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedLiteral		(),
		MAGIC_MWIToString (iByteCodeScanner.fetchedLiteral ())
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSI:
	case VByteCodeDescriptor::ParameterStructure_SSIS:
	    IO_printf (
		"\t%s %u[%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedSelectorIndex	(),
		iByteCodeScanner.fetchedSelectorName	()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSI_Valence:
	case VByteCodeDescriptor::ParameterStructure_SSIS_Valence:
	    IO_printf (
		"\t%s %u[%s] %u\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedSelectorIndex	(),
		iByteCodeScanner.fetchedSelectorName	(),
		iByteCodeScanner.fetchedSelectorValence	()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSI_MWI:
	case VByteCodeDescriptor::ParameterStructure_SSIS_MWI:
	    IO_printf (
		"\t%s %u[%s] %u[^%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedSelectorIndex	(),
		iByteCodeScanner.fetchedSelectorName	(),
		iByteCodeScanner.fetchedLiteral		(),
		MAGIC_MWIToString (iByteCodeScanner.fetchedLiteral ())
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_SLI:
	case VByteCodeDescriptor::ParameterStructure_SLIS:
	    IO_printf (
		"\t%s %u[%s]\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedLiteral		(),
		iByteCodeScanner.fetchedStringText	()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_PLI:
	case VByteCodeDescriptor::ParameterStructure_PLIS:
	    IO_printf (
		"\t%s %u\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedLiteral		()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_Int:
	    IO_printf (
		"\t%s %d\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedInteger		()
	    );
	    break;
	case VByteCodeDescriptor::ParameterStructure_Double:
	    IO_printf (
		"\t%s %g\n",
		iByteCodeScanner.fetchedByteCodeName	(),
		iByteCodeScanner.fetchedDouble		()
	    );
    	    break;
    	}
    } while (notDone);
}


/***** Printer State *****/
PrivateVarDef int NestLevel;

/*---------------------------------------------------------------------------
 *****  Routine to display a Block.
 *
 *  Arguments:
 *	stdCPD			- the address of a CPD which points to a
 *				  physical object pointer for the Block.
 *	recursive		- a boolean,  if true prints Blocks contained
 *				  in the PLVector in addition to the block
 *				  container address info.
 *
 *  Returns:
 *	NOTHING
 *
 *
 *****/
PrivateFnDef void PrintBlock (M_CPD* stdCPD, int recursive) {
    rtBLOCK_BlockType* blockPtr = rtBLOCK_CPDBase (stdCPD);

    int i; char *cp, *end;

    IO_printf ("\'");
    switch (rtBLOCK_SelectorType (blockPtr)) {
    case rtBLOCK_C_DefinedSelector:
        IO_printf ("[uds]%s\'\n", (rtBLOCK_CPD_StringSpace (stdCPD)  +
				    rtBLOCK_SelectorIndex (blockPtr)));
	IO_printf ("\'[stringspace index]%u",
				     rtBLOCK_SelectorIndex (blockPtr));
	break;
    case rtBLOCK_C_KnownSelector:
	IO_printf ("[ksi#%u]", rtBLOCK_SelectorIndex (blockPtr));
	IO_printf ("%s", KS__ToString
			     (rtBLOCK_SelectorIndex (blockPtr)));
        break;
    case rtBLOCK_C_NoSelector:
	IO_printf ("No Selector");
	break;
    default:
        IO_printf ("**UnknownSelectorType#%u**",
				rtBLOCK_SelectorType (blockPtr));
        break;
    }
    IO_printf ("\'\n");


    IO_printf ("Compiler Version: %d\n", rtBLOCK_compilerVersion (blockPtr));
    IO_printf ("Block Size: %d", M_CPD_Size (stdCPD));
    IO_printf ("\nCPD Pointers:\n\
\tbase:        %08X\tend:         %08X\tlocalPOP: %08X\n\
\tbytecodeV:   %08X\tPC:          %08X\tSelector: %08X\n\
\tstringSpace: %08X\n\
\tphyLV:       %08X\tplvElement:  %08X\n",
	rtBLOCK_CPDBase (stdCPD),
	rtBLOCK_CPDEnd (stdCPD),
	rtBLOCK_CPD_LocalEnvironment (stdCPD),
	rtBLOCK_CPD_ByteCodeVector (stdCPD),
	rtBLOCK_CPD_PC (stdCPD),
	rtBLOCK_CPD_SelectorName (stdCPD),
	rtBLOCK_CPD_StringSpace (stdCPD),
	rtBLOCK_CPD_PLiteralVector (stdCPD),
	rtBLOCK_CPD_PLVectorElement (stdCPD));


    IO_printf ("\nPreamble Data:\n\tLE: %d\tBV: %d\tSS: %d\tPL: %d\tEV: %d\n",
	rtBLOCK_localEnvironment (blockPtr),
	rtBLOCK_byteCodeVector (blockPtr),
	rtBLOCK_stringSpace (blockPtr),
	rtBLOCK_PLVector (blockPtr),
	rtBLOCK_ELEVector (blockPtr));

    PrintByteCodes (stdCPD);

    if (IsntNil (rtBLOCK_CPD_StringSpace (stdCPD))) {
	if ((end = (char *)rtBLOCK_CPD_PLiteralVector (stdCPD)) == 0)
	    end = rtBLOCK_CPDEnd (stdCPD);

	IO_printf ("String Space:");
	for (i =0, cp = rtBLOCK_CPD_StringSpace (stdCPD); cp < end; ++cp, ++i) {
	    if (i % 78 == 0 && i != 0)
		IO_printf ("!\n");
	    else if (i == 0)
		IO_printf ("\n");
	    IO_printf ("%c", *cp == '\0' ? ' ' : *cp);
	    if (*cp == '\n')
		i = 0 ;
	}
    }
    IO_printf ("\n");

    if (recursive) {
        NestLevel++;
        for (i = 0;
	     i < rtBLOCK_PLVector_Count (rtBLOCK_CPD_PLiteralVector(stdCPD));
	     (rtBLOCK_CPD_PLVectorElement (stdCPD))++, i++
	)
	{
	    IO_printf ("\nBlock #%d; Level #%d\n", i, NestLevel);

	    RTYPE_RPrint (stdCPD, rtBLOCK_CPx_PLVElement);
	}
	NestLevel--;
    }

    IO_printf ("\n");

    return;
}


/***************************************************
 *****  Internal Method Debugger Dictionaries  *****
 ***************************************************/

/******************
 *  Type Methods  *
 ******************/

IOBJ_DefineNilaryMethod (New) {
    return RTYPE_QRegister (rtBLOCK_New (IOBJ_ScratchPad));
}


/**********************
 *  Instance Methods  *
 **********************/

IOBJ_DefineUnaryMethod (Decompile) {
    V::VString iSource;
    RSLANG_Decompile (iSource, RTYPE_QRegisterCPD (self));
    return RTYPE_QRegister (rtSTRING_New (iSource));
}

IOBJ_DefineUnaryMethod (NumNestedBlocks) {
    M_CPD* stdCPD = RTYPE_QRegisterCPD (self);
    return IOBJ_IntIObject (rtBLOCK_PLVector_Count (rtBLOCK_CPD_PLiteralVector (stdCPD)));
}

IOBJ_DefineMethod (GetNestedBlock) {
    M_CPD*	cpd = RTYPE_QRegisterCPD (self);
    int		i = IOBJ_NumericIObjectValue (parameterArray [0], int);

    if (i >= 0 && i < rtBLOCK_PLVector_Count (rtBLOCK_CPD_PLiteralVector (cpd))) {
	rtBLOCK_CPD_PLVectorElement (cpd) += i;
        return RTYPE_Browser (cpd, rtBLOCK_CPx_PLVElement);
    }
    return self;
}

IOBJ_DefineUnaryMethod (GetLocalEnv_DM)
{
    return RTYPE_Browser (RTYPE_QRegisterCPD (self), rtBLOCK_CPx_LocalEnv);
}


/**************************************************
 *****  Representation Type Handler Function  *****
 **************************************************/

RTYPE_DefineHandler (rtBLOCK_Handler) {
    IOBJ_BeginMD (typeMD)
	IOBJ_MDE ("qprint"	, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("print"	, RTYPE_DisplayTypeIObject)
	IOBJ_MDE ("new"		, New)
    IOBJ_EndMD;

    IOBJ_BeginMD (instanceMD)
	IOBJ_MDE ("qprint"		, RTYPE_QPrintIObject)
	IOBJ_MDE ("print"		, RTYPE_PrintIObject)
	IOBJ_MDE ("rprint"		, RTYPE_RPrintIObject)
	IOBJ_MDE ("decompile"		, Decompile)
	IOBJ_MDE ("numNestedBlocks"	, NumNestedBlocks)
	IOBJ_MDE ("getLocalEnv"	    	, GetLocalEnv_DM)
	IOBJ_MDE ("getNestedBlock:"	, GetNestedBlock)
    IOBJ_EndMD;

    switch (handlerOperation) {
    case RTYPE_InitializeMData: {
	    M_RTD *rtd = iArgList.arg<M_RTD*>();
	    rtd->SetCPDPointerCountTo	(rtBLOCK_CPD_StdPtrCount);
	    M_RTD_CPDInitFn		(rtd) = rtBLOCK_InitStdCPD;
	    M_RTD_HandleMaker		(rtd) = &rtBLOCK_Handle::Maker;
	    M_RTD_ReclaimFn		(rtd) = ReclaimContainer;
	    M_RTD_MarkFn		(rtd) = MarkContainers;
	}
	break;
    case RTYPE_TypeMD:
        *iArgList.arg<IOBJ_MD*>() = typeMD;
        break;
    case RTYPE_InstanceMD:
        *iArgList.arg<IOBJ_MD*>() = instanceMD;
        break;
    case RTYPE_PrintObject:
	PrintBlock (iArgList.arg<M_CPD*>(), false);
        break;
    case RTYPE_RPrintObject:
	PrintBlock (iArgList.arg<M_CPD*>(), true);
        break;
    default:
        return -1;
    }
    return 0;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (rtBLOCK);
    FAC_FDFCase_FacilityDescription ("Block Representation Type Handler");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTblock.c 1 replace /users/mjc/system
  860404 20:10:18 mjc Create Block Representation Type

 ************************************************************************/
/************************************************************************
  RTblock.c 2 replace /users/hjb/system
  860409 04:45:11 hjb Preliminary Release (jck)

 ************************************************************************/
/************************************************************************
  RTblock.c 3 replace /users/jck/source
  860409 08:10:21 jck #include files corrected

 ************************************************************************/
/************************************************************************
  RTblock.c 4 replace /users/hjb/system
  860409 23:12:10 hjb done with adjustment

 ************************************************************************/


/************************************************************************
  RTblock.c 5 replace /users/jck/source
  860418 00:04:01 jck Module Reorganization complete

 ************************************************************************/
/************************************************************************
  RTblock.c 6 replace /users/jck/system
  860418 12:04:31 jck String space tidied up

 ************************************************************************/
/************************************************************************
  RTblock.c 7 replace /users/hjb/system
  860423 00:38:46 hjb done updating calls

 ************************************************************************/
/************************************************************************
  RTblock.c 8 replace /users/jck/system
  860424 11:11:30 jck Print routines cleaned up

 ************************************************************************/
/************************************************************************
  RTblock.c 9 replace /users/jck/system
  860430 17:44:33 jck Minor modification to PrintByteCodes

 ************************************************************************/
/************************************************************************
  RTblock.c 10 replace /users/jck/system
  860509 07:15:14 jck Minor bug in print routine fixed

 ************************************************************************/
/************************************************************************
  RTblock.c 11 replace /users/jck/system
  860522 12:04:08 jck Debug Method to access Local Environment POP added

 ************************************************************************/
/************************************************************************
  RTblock.c 12 replace /users/mjc/system
  860527 18:00:02 mjc Changed 'rtBLOCK_CPD_PCCounter' to 'rtBLOCK_CPD_PC'

 ************************************************************************/
/************************************************************************
  RTblock.c 13 replace /users/mjc/system
  860605 16:31:01 mjc Fix non-conforming type names

 ************************************************************************/
/************************************************************************
  RTblock.c 14 replace /users/mjc/system
  860617 18:10:19 mjc Added 'save' routine

 ************************************************************************/
/************************************************************************
  RTblock.c 15 replace /users/spe/system
  860624 16:56:16 spe Changed RTblock to reflect changes in the block structure.

 ************************************************************************/
/************************************************************************
  RTblock.c 16 replace /users/mjc/system
  860708 12:47:26 mjc Deleted references to 'RTselb' and '_PI' vmachine parameter structure cases

 ************************************************************************/
/************************************************************************
  RTblock.c 17 replace /users/mjc/system
  860817 20:15:32 mjc Converted CPD initializer/instantiator/gc routines to expect nothing of new CPDs

 ************************************************************************/
/************************************************************************
  RTblock.c 18 replace /users/jck/system
  870415 09:52:54 jck Release of M_SwapContainers, including changes to
all rtype's 'InitStdCPD' functions. Instead of being void, they now return
a cpd

 ************************************************************************/
/************************************************************************
  RTblock.c 19 replace /users/mjc/translation
  870524 09:31:29 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  RTblock.c 20 replace /users/jck/system
  871007 13:35:02 jck Added a marking function for the global garbage collector

 ************************************************************************/
