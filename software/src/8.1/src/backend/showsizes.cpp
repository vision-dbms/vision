
#include "Vk.h"

#include "VkLongUnsigneds.h"
#include "VkMemory.h"
#include "VkMessage.h"

#include "rtype.d"
#include "ps.d"
#include "m.d"

#ifndef sun
#include "IOMHandle.h"
#endif

#include "RTblock.d"
#include "RTcharuv.d"
#include "RTclosure.d"
#include "RTcontext.d"
#include "RTdictionary.d"
#include "RTdoubleuv.d"
#include "RTdsc.d"
#include "RTfloatuv.d"
#include "RTindex.d"
#include "RTintuv.d"
#include "RTlink.d"
#include "RTlstore.d"
#include "RTmethod.d"
#include "RTparray.d"
#include "RTptoken.d"
#include "RTrefuv.d"
#include "RTseluv.d"
#include "RTstring.d"
#include "RTtct.d"
#include "RTundefuv.d"
#include "RTvector.d"
#include "RTvstore.d"

#include "Vca_VcaPeerCharacteristics.h"


/***********************
 ***********************
 *****  Utilities  *****
 ***********************
 ***********************/

#define DisplaySizeLabelWidth 50

#define DisplaySize(x) printf ("sizeof (%*.*s) = %u\n", DisplaySizeLabelWidth, DisplaySizeLabelWidth, #x, sizeof (x))
#define DisplaySizes(t1,t2) printf (\
    "sizeof (%*.*s) = %2u %s %2u - %u\n",\
    DisplaySizeLabelWidth, DisplaySizeLabelWidth, #t1,\
    sizeof (t1),\
    sizeof (t1) ==  sizeof (t2) - sizeof (t2::BaseClass) ? "==" : "!=",\
    sizeof (t2), sizeof (t2::BaseClass)\
)


/********************************
 ********************************
 *****  Experimental Types  *****
 ********************************
 ********************************/

class VContainerHandle;

class M_DCTE {
//  State
public:
    union address_t {
	M_CPreamble*		asContainerAddress;
	VContainerHandle*	asCPCC;
	M_POP			asPOP;
	int			asCTI;
    } address;
    unsigned int		m_iReferenceCount		: 16;
    unsigned int		m_xAddressType			: 2;
    unsigned int		m_iAttentionMask		: 3;
    unsigned int		m_fIsNew			: 1;
    unsigned int		m_fHasBeenAccessed		: 1;
    unsigned int		m_fMustBeRemapped		: 1;
};

class M_DCTE2 {
//  State
public:
    union address_t {
	M_CPreamble*		asContainerAddress;
	VContainerHandle*	asCPCC;
	M_POP			asPOP;
	int			asCTI;
    } address;
    unsigned short		m_iReferenceCount;
    unsigned short		m_xAddressType			: 2;
    unsigned short		m_iAttentionMask		: 3;
    unsigned short		m_fIsNew			: 1;
    unsigned short		m_fHasBeenAccessed		: 1;
    unsigned short		m_fMustBeRemapped		: 1;
//    unsigned int						: 6;
};

class M_DCTE3 {
//  State
public:
    union address_t {
	M_CPreamble*		asContainerAddress;
	VContainerHandle*	asCPCC;
	M_POP			asPOP;
	int			asCTI;
    } address;
    unsigned short		m_iReferenceCount;
    unsigned int		m_xAddressType			: 2;
    unsigned int		m_iAttentionMask		: 3;
    unsigned int		m_fIsNew			: 1;
    unsigned int		m_fHasBeenAccessed		: 1;
    unsigned int		m_fMustBeRemapped		: 1;
//    unsigned int						: 6;
};

class XrtPopvector : public M_CPreamble {
public:
    typedef M_CPreamble BaseClass;

    unsigned int		isInconsistent : 1;
    unsigned int		elementCount   :31;
};

class XrtBlock : public M_CPreamble {
public:
    typedef M_CPreamble BaseClass;

    int				compilerVersion;
    rtBLOCK_SelectorType	selector;
    rtBLOCK_PreambleType	preamble;
    M_POP			localEnvironmentPOP;
};

class XrtClosure : public M_CPreamble {
public:
    typedef M_CPreamble BaseClass;

    M_POP			context;
    M_POP			block;
    int				primitive;
};

class XrtContext : public XrtPopvector {
public:
    typedef XrtPopvector BaseClass;

    M_POP			current;
    M_POP			self;
    M_POP			origin;
};

class XrtDescriptor : public M_CPreamble {
public:
    typedef M_CPreamble BaseClass;

    DSC_PointerType		pointerType;
    RTYPE_Type			rType;
    M_POP			store;
    M_POP			pointer;
    DSC_ScalarValue		scalarValue;
};

class XrtIndex : public XrtPopvector {
public:
    typedef XrtPopvector BaseClass;

    M_POP			lists;
    M_POP			keyStore;
    M_POP			keyValues;
    M_POP			map;
};

class XrtLink : public M_CPreamble {
public:
    typedef M_CPreamble BaseClass;

    M_POP
	posPToken,
	refPToken;
    unsigned int
	tracking	:  1, 
	hasRepeats	:  1, 
	hasRanges	:  1, 
	isInconsistent	:  1, 
	rrdCount	: 28;
    rtLINK_RRDType
	rrdArray[1];
};

class XrtLStore : public M_CPreamble {
public:
    typedef M_CPreamble BaseClass;

    M_POP
	rowPToken,
	content,
	contentPToken;
    unsigned int
    	stringStore	:  1,
	isInconsistent	:  1, 
	breakpointCount	: 30,
	breakpointArray[1];
};

class XrtMethod : public XrtPopvector {
public:
    typedef XrtPopvector BaseClass;

    M_POP
	block,
	origin;
};

class XrtPToken : public M_CPreamble {
public:
    typedef M_CPreamble BaseClass;

    M_POP			nextGeneration;
    unsigned int		independent	:  1,
				tokenType	:  6,
				closed		:  1,
				count		: 24,
				baseElementCount;
};

class XrtVector : public M_CPreamble {
public:
    typedef M_CPreamble BaseClass;

    M_POP
	pToken;
    unsigned int
	isASet		: 1, 
	isInconsistent  :  1, 
	pMapSize	: 30;
    int
	pMapTransition,
	uSegArraySize,
	uSegFreeBound,
	uSegIndexSize;
    rtVECTOR_PMRDType
	pMap[1];
};

class XrtVStore : public XrtPopvector {
public:
    typedef XrtPopvector BaseClass;

    M_POP
	rowPToken,
	inheritanceStore,
	inheritancePointer,
	columnPToken,
	methodDictionary,
	auxiliaryColumn,
	columnArray[1];
};

class XrtUVector : public M_CPreamble {
public:
    typedef M_CPreamble BaseClass;

    M_POP			pToken,
				refPToken,
				auxiliaryPOP;
    unsigned int		elementCount;
    unsigned short		granularity;

#if defined(_AIX)
    unsigned int 
#else
    unsigned short
#endif
				isInconsistent	: 1,
				isASetUV	: 1,
				padding		: 14;
};


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int Unused(argc),char *Unused(argv)[]) {

    DisplaySize (M_POP);

    DisplaySize (PS_UID);
    DisplaySize (PS_TS);
    DisplaySize (PS_Type_FO);
    DisplaySize (PS_TID);
    DisplaySize (PS_RID);

    DisplaySize (PS_NDH);
    DisplaySize (PS_NVD);
    DisplaySize (PS_SRD);
    DisplaySize (PS_SD);
    DisplaySize (PS_SVD);

    DisplaySize (PS_SH);
    DisplaySize (PS_CTE);
    DisplaySize (PS_CT);

    DisplaySizes(POPVECTOR_PVType	, XrtPopvector);

    DisplaySizes(rtBLOCK_BlockType	, XrtBlock);
    DisplaySize (rtBLOCK_BCVectorType);
    DisplaySize (rtBLOCK_PLVectorType);
    DisplaySizes(rtCLOSURE_Closure	, XrtClosure);
    DisplaySizes(rtCONTEXT_Context	, XrtContext);
    DisplaySizes(rtDSC_Descriptor	, XrtDescriptor);
    DisplaySize (DSC_ScalarValue);
    DisplaySizes(rtINDEX_Index		, XrtIndex);
    DisplaySize (rtLINK_RRDType);
    DisplaySizes(rtLINK_Type		, XrtLink);
    DisplaySizes(rtLSTORE_LStore	, XrtLStore);
    DisplaySizes(rtMETHOD_Type_Method	, XrtMethod);
    DisplaySize (rtPTOKEN_SDType);
    DisplaySize (rtPTOKEN_CartesianType);
    DisplaySizes(rtPTOKEN_Type		, XrtPToken);
    DisplaySize (rtSELUV_Type_Element);
    DisplaySize (rtVECTOR_USDType);
    DisplaySize (rtVECTOR_PMRDType);
    DisplaySizes(rtVECTOR_Type		, XrtVector);
    DisplaySizes(rtVSTORE_Type		, XrtVStore);

    DisplaySizes(UV_PreambleType	, XrtUVector);
    DisplaySize (UV_UVType);

    DisplaySize (VkUnsigned64);
    DisplaySize (VkUnsigned96);
    DisplaySize (VkUnsigned128);

    DisplaySize (M_DCTE);
    DisplaySize (M_DCTE2);
    DisplaySize (M_DCTE3);

    DisplaySize (VTransient);
    DisplaySize (VkUUID);
    DisplaySize (Vca::VcaPeerCharacteristics);
    DisplaySize (Vca::VcaPeerCharacteristics::VersionRange);
    DisplaySize (Vca::VcaPeerCharacteristics::VersionRange::Version);
    DisplaySize (Vca::VcaSSID);

    DisplaySize (VkMessage);
    DisplaySize (VkMessageHeader);
    DisplaySize (VkMessageTargetIndex);

    DisplaySize (bool);
    DisplaySize (Vca::F32);
    DisplaySize (Vca::F64);
    DisplaySize (Vca::S08);
    DisplaySize (Vca::S16);
    DisplaySize (Vca::S32);
    DisplaySize (Vca::S64);
    DisplaySize (Vca::U08);
    DisplaySize (Vca::U16);
    DisplaySize (Vca::U32);
    DisplaySize (Vca::U64);

#ifndef sun
    DisplaySize (IOMHandle);
#endif

    DisplaySize (caddr_t);
    DisplaySize (ptrdiff_t);
    DisplaySize (off_t);
    DisplaySize (size_t);
    DisplaySize (ssize_t);

    DisplaySize (pointer_t);
    DisplaySize (pointer_diff_t);
    DisplaySize (pointer_size_t);

    DisplaySize (int);
    DisplaySize (long int);
    DisplaySize (long long int);

    return 0;
}
