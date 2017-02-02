#ifndef VByteCodeScanner_Interface
#define VByteCodeScanner_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"
#include "VCPDReference.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VByteCodeDescriptor.h"
#include "VMagicWordDescriptor.h"

#include "RTblock.h"


/*************************
 *****  Definitions  *****
 *************************/

class VByteCodeScanner : public VTransient {
//  Aliases
public:
    typedef rtBLOCK_Handle::Reference	HandleReference;
    typedef rtBLOCK_Handle::offset_t	offset_t;
    typedef VByteCodeDescriptor::ByteCode ByteCode;

//  Type Definitions
public:
    struct FetchedSandSData {
	unsigned int	m_xFetchedLiteral,
			m_xFetchedSelector,
			m_iFetchedValence;
    };

//  Construction
public:
    VByteCodeScanner (M_CPD *pBlockCPD);
    VByteCodeScanner (rtBLOCK_Handle *pBlockHandle);

//  Destruction
public:
    ~VByteCodeScanner ();


//  Access
public:
    M_CPD *blockCPD () const {
	if (m_pBlockCPD.isNil ())
	    m_pBlockCPD.claim (m_pBlockHandle->GetCPD ());
	rtBLOCK_CPD_PC (m_pBlockCPD) = rtBLOCK_CPD_ByteCodeVector (m_pBlockCPD) + m_xBC;
	return m_pBlockCPD;
    }
    rtBLOCK_Handle *blockHandle () const {
	return m_pBlockHandle;
    }

    unsigned char const *bcAddress () const {
	return m_pBlockHandle->addressOfByteCode (m_xBC);
    }

    offset_t bcOffset () const {
	return m_xBC;
    }
    unsigned int getUByte (offset_t xOffset) {
	return *m_pBlockHandle->addressOfByteCode (xOffset);
    }

    VByteCodeDescriptor const *fetchedByteCodeDescription () const {
	return m_pFetchedByteCodeDescription;
    }

    unsigned int fetchedAssociatedKSI () const {
	return m_pFetchedByteCodeDescription->associatedKSI ();
    }
    VByteCodeDescriptor::ByteCode fetchedByteCode () const {
	return m_pFetchedByteCodeDescription->byteCode ();
    }
    unsigned int fetchedByteCodeIsSuspendSafe () const {
	return m_pFetchedByteCodeDescription->suspendSafe ();
    }
    char const* fetchedByteCodeName () const {
	return m_pFetchedByteCodeDescription->name ();
    }
    VByteCodeDescriptor::DispatchCase fetchedDispatchCase () const {
	return m_pFetchedByteCodeDescription->dispatchCase ();
    }
    VByteCodeDescriptor::ParameterStructure fetchedParameterStructure () const {
	return m_pFetchedByteCodeDescription->parameterStructure ();
    }

    rtBLOCK_Handle *fetchedBlockHandle () const {
	return m_pBlockHandle->nestedBlockHandle (fetchedLiteral ());
    }
    M_CPD *fetchedBlockCPD () const {
	return m_pBlockHandle->nestedBlockCPD (fetchedLiteral ());
    }
    double fetchedDouble () const {
	return m_iImmediateData.as_iFetchedDouble;
    }
    int fetchedInteger () const {
	return m_iImmediateData.as_iFetchedInteger;
    }
    unsigned int fetchedLiteral () const {
	return m_iImmediateData.as_iObject.m_xFetchedLiteral;
    }
    VMagicWord fetchedMagicWord () const {
	return (VMagicWord)m_iImmediateData.as_iObject.m_xFetchedLiteral;
    }
    unsigned int fetchedSelectorIndex () const {
	return m_iImmediateData.as_iObject.m_xFetchedSelector;
    }
    char const *fetchedSelectorName () const {
	return m_pBlockHandle->addressOfString (fetchedSelectorIndex ());
    }
    unsigned int fetchedSelectorValence () const {
	return m_iImmediateData.as_iObject.m_iFetchedValence;
    }
    char const *fetchedStringText () const {
	return m_pBlockHandle->addressOfString (fetchedLiteral ());
    }


//  Fetch Utilities
private:
    unsigned int getUByte () {
	return getUByte (m_xBC++);
    }
    unsigned short getUShort () {
	unsigned short iValue = *reinterpret_cast<unsigned short const*>(bcAddress ());;
	m_xBC += sizeof (unsigned short);
	return iValue;
    }
    int getInteger () { 
	int iValue = *reinterpret_cast<int const*>(bcAddress ());;
	m_xBC += sizeof (int);
	return iValue;
    }
    double getDouble () {
	double iValue = *reinterpret_cast<double const*>(bcAddress ());;
	m_xBC += sizeof (double);
	return iValue;
    }   

    ByteCode getByteCode () {
	return (ByteCode)getUByte ();
    }

    unsigned short getXUShort () {
	unsigned short iValue;
	memcpy (&iValue, bcAddress (), sizeof (unsigned short));
	m_xBC += sizeof (unsigned short);
	return iValue;
    }
    unsigned int getXUInteger () {
	unsigned int iValue;
	memcpy (&iValue, bcAddress (), sizeof (unsigned int));
	m_xBC += sizeof (unsigned int);
	return iValue;
    }

    void skipByte   () { skip (sizeof (char  )); }
    void skipShort  () { skip (sizeof (short )); }

    void skip (unsigned int cBytes) { m_xBC += cBytes; }


//  Instruction Fetcher
public:
    void fetchInstruction (bool bFetchingTag = false) {
	m_pFetchedByteCodeDescription = VByteCodeDescriptor::Description (
	    getByteCode ()
	);
	switch (fetchedParameterStructure ()) {
	default:
	    break;
	case VByteCodeDescriptor::ParameterStructure_Pad:
	    skipByte ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_PadS:
	    skipShort ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_Tag:
	    if (bFetchingTag)
		m_iImmediateData.as_iObject.m_xFetchedLiteral = getUByte ();
	    else skip (sizeof (unsigned char));
	    break;
	case VByteCodeDescriptor::ParameterStructure_TagS:
	    if (bFetchingTag)
		m_iImmediateData.as_iObject.m_xFetchedLiteral = getUByte ();
	    else skip (sizeof (unsigned short));
	    break;
	case VByteCodeDescriptor::ParameterStructure_TagInt:
	    if (bFetchingTag)
		m_iImmediateData.as_iObject.m_xFetchedLiteral = getXUInteger ();
	    else skip (sizeof (unsigned int));
	    break;
	case VByteCodeDescriptor::ParameterStructure_KSI:
	    m_iImmediateData.as_iObject.m_xFetchedSelector = getUByte ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_KSI_MWI:
	    m_iImmediateData.as_iObject.m_xFetchedSelector = getUByte ();
	    m_iImmediateData.as_iObject.m_xFetchedLiteral  = getUByte ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_MWI:
	    m_iImmediateData.as_iObject.m_xFetchedLiteral = getUByte ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSI:
	    m_iImmediateData.as_iObject.m_xFetchedSelector = getUByte ();
	    m_iImmediateData.as_iObject.m_iFetchedValence  = 0;
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSI_Valence:
	    m_iImmediateData.as_iObject.m_xFetchedSelector = getUByte ();
	    m_iImmediateData.as_iObject.m_iFetchedValence  = getUByte ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSI_MWI:
	    m_iImmediateData.as_iObject.m_xFetchedSelector = getUByte ();
	    m_iImmediateData.as_iObject.m_iFetchedValence  = 0;
	    m_iImmediateData.as_iObject.m_xFetchedLiteral  = getUByte ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSIS:
	    m_iImmediateData.as_iObject.m_xFetchedSelector = getUShort ();
	    m_iImmediateData.as_iObject.m_iFetchedValence  = 0;
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSIS_Valence:
	    m_iImmediateData.as_iObject.m_xFetchedSelector = getUShort ();
	    m_iImmediateData.as_iObject.m_iFetchedValence  = getUByte  ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_SSIS_MWI:
	    m_iImmediateData.as_iObject.m_xFetchedSelector = getUShort ();
	    m_iImmediateData.as_iObject.m_iFetchedValence  = 0;
	    m_iImmediateData.as_iObject.m_xFetchedLiteral  = getUByte  ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_SLI:
	    m_iImmediateData.as_iObject.m_xFetchedLiteral = getUByte ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_SLIS:
	    m_iImmediateData.as_iObject.m_xFetchedLiteral = getUShort ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_PLI:
	    m_iImmediateData.as_iObject.m_xFetchedLiteral = getUByte ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_PLIS:
	    m_iImmediateData.as_iObject.m_xFetchedLiteral = getUShort ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_Int:
	    m_iImmediateData.as_iFetchedInteger = getInteger ();
	    break;
	case VByteCodeDescriptor::ParameterStructure_Double:
	    m_iImmediateData.as_iFetchedDouble = getDouble ();
	    break;
	}
    }


//  State
protected:
    VCPDReference mutable	m_pBlockCPD;
    HandleReference const	m_pBlockHandle;
    offset_t			m_xBC;
    union immediate_data_t {
	FetchedSandSData	as_iObject;
	int			as_iFetchedInteger;
	double			as_iFetchedDouble;
    } m_iImmediateData;
    VByteCodeDescriptor const*	m_pFetchedByteCodeDescription;
};


#endif
