/*****  Vca_VTypeInfoHolder Implementation  *****/
#define Vca_VTypeInfoHolder_Instantiations

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

#define   Vca_VTypeInfoHolder
#include "Vca_VTypeInfoHolder.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VInterfaceMember.h"
#include "Vca_VTypeInfoHolderForArrayOf.h"		// ==> VTypeInfoHolderForSerializable ==> VTypeInfoHolderFor
#include "Vca_VTypeInfoHolderForReferenceable.h"	// ==> VTypeInfoHolderForSerializable ==> VTypeInfoHolderFor


/**********************************************************
 **********************************************************
 *****                                                *****
 *****  Vca::VTypeInfoHolderInstance::SlackerTracker  *****
 *****                                                *****
 **********************************************************
 **********************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VTypeInfoHolderInstance::SlackerTracker::SlackerTracker (
    snitch_t pSnitch
) : m_pSuccessor (g_pSlackerTrackers), m_pSnitch (pSnitch) {
    g_pSlackerTrackers = this;
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VTypeInfoHolderInstance::SlackerTracker::~SlackerTracker () {
}

/***********************
 ***********************
 *****  Snitching  *****
 ***********************
 ***********************/

unsigned int Vca::VTypeInfoHolderInstance::SlackerTracker::ShowSlackers () {
    unsigned int cSlackers = 0;
    for (Pointer pSlacker (g_pSlackerTrackers); pSlacker; pSlacker = pSlacker->successor ()) {
	if (pSlacker->snitch ()())
	    cSlackers++;
    }
    return cSlackers;
}


/******************************************
 ******************************************
 *****                                *****
 *****  Vca::VTypeInfoHolderInstance  *****
 *****                                *****
 ******************************************
 ******************************************/

namespace Vca {
    VTypeInfoHolderInstance::SlackerTracker* VTypeInfoHolderInstance::g_pSlackerTrackers = 0;
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VTypeInfoHolderInstance::VTypeInfoHolderInstance () : m_sMemberArray (0), m_pMemberArray (0), m_pMemberListHead (0) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VTypeInfoHolderInstance::~VTypeInfoHolderInstance () {
}

/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

void Vca::VTypeInfoHolderInstance::raiseExistingTypeInfoHolderException (
    VTypeInfoHolderInstance *pExistingInstance
) {
#if 0
    fprintf (
	stderr,
	"VTypeInfoHolder %p: Master Instance %p Already Exists For %s\n",
	this, pExistingInstance, pExistingInstance->rttiName_()
    );
#endif
}

void Vca::VTypeInfoHolderInstance::raiseMissingTypeInfoHolderException (
    std::type_info const &rTypeInfo, void const *pTTIHG
) {
    V::VRTTI iRTTI (rTypeInfo);
    char const *pTypeName = iRTTI.name ();
    /* raiseApplicationException */
    fprintf (
	stderr, "%p:%s: Master Instance Missing\n", pTTIHG, pTypeName ? pTypeName : "VTypeInfoHolder<--Unknown-Type-->"
    );
}

void Vca::VTypeInfoHolderInstance::initialize () {
    static bool bShowingTypes = IsntNil (getenv ("VcaShowingTypes"));

    if (m_pTypeInfo.interlockedSetIfNil (typeInfoInstantiation_())) {
	m_pTypeInfo->setTypeInfoHolderInstanceTo (this);
	if (bShowingTypes) {
	    m_pTypeInfo->displayDescription ();
	    display ("\n");
	}
    }
}


/***************************
 ***************************
 *****  Member Access  *****
 ***************************
 ***************************/

void Vca::VTypeInfoHolderInstance::buildMemberIndices () {
    VInterfaceMember const *pMember = m_pMemberListHead;
    while (pMember) {
	if (m_sMemberArray <= pMember->index ())
	    m_sMemberArray = pMember->index () + 1;
	pMember = pMember->successor ();
    }
    m_pMemberArray = (VInterfaceMember const**)allocate (
	sizeof (VInterfaceMember const *) * m_sMemberArray
    );
    for (unsigned int xMember = 0; xMember < m_sMemberArray; xMember++)
	m_pMemberArray[xMember] = 0;

    pMember = m_pMemberListHead;
    while (pMember) {
	unsigned int xMember = pMember->index ();
	if (m_pMemberArray[xMember]) raiseApplicationException (
	    "VTypeInfoHolderInstance: Duplicate Interface Member Index %u For Type %s",
	    xMember,
	    m_pTypeInfo ? (char const*)m_pTypeInfo->name () : "<Unknown Type>"
	);
	m_pMemberArray[xMember] = pMember;
	pMember = pMember->successor ();
    }
}

Vca::VInterfaceMember const *Vca::VTypeInfoHolderInstance::interfaceMember (unsigned int xMember) {
//  Build the member array index if it doesn't already exist...
    if (memberIndicesNotInitialized ())
	buildMemberIndices ();

//  ... and return the requested member:
    return xMember < m_sMemberArray ? m_pMemberArray[xMember] : NilOf (VInterfaceMember const *);
}

Vca::VInterfaceMember const *Vca::VTypeInfoHolderInstance::interfaceMember (VString const &rMemberName) {
    VInterfaceMember const *pMember = m_pMemberListHead;
    while (pMember) {
	if (rMemberName == pMember->name ())
	    break;
	pMember = pMember->successor ();
    }
    return pMember;
}

/*********************************
 *********************************
 *****  Member Registration  *****
 *********************************
 *********************************/

void Vca::VTypeInfoHolderInstance::registerMember (VInterfaceMember *pMember) {
    pMember->m_pSuccessor = m_pMemberListHead;
    m_pMemberListHead = pMember;
}


/****************************
 ****************************
 *****  Proxy Creation  *****
 ****************************
 ****************************/

IVUnknown *Vca::VTypeInfoHolderInstance::proxy (VcaOIDX *pOIDX) const {
    return NilOf (IVUnknown*);
}

/******************************
 ******************************
 *****  Type Description  *****
 ******************************
 ******************************/

void Vca::VTypeInfoHolderInstance::showTypeInfoDescription (
    std::type_info const &rTypeInfo
) const {
    if (m_pTypeInfo.isntNil ())
	m_pTypeInfo->displayDescription ();
    else {
	V::VRTTI iRTTI (rTypeInfo);
	display ("+++  %s: Type Info Not Available", iRTTI.name ());
    }
    display ("\n\n");
}


/******************************************
 ******************************************
 *****                                *****
 *****  VTypeInfoHolderFor Instances  *****
 *****                                *****
 ******************************************
 ******************************************/

namespace Vca {
    /*----------------------------------*
     *----  Non Serializable Types  ----*
     *----------------------------------*/

    // {2A19736C-BFCE-4eca-85B8-9B6DE2C69020}
    VTypeInfoHolderFor<char const*> const VTypeInfoHolderFor_char_const_p (
	0x2a19736c, 0xbfce, 0x4eca, 0x85, 0xb8, 0x9b, 0x6d, 0xe2, 0xc6, 0x90, 0x20
    );

    // {23E24D87-7832-4329-A86F-8E86F7290EEA}
    VTypeInfoHolderFor<uuid_t const&> const VTypeInfoHolderFor_uuid_t_const_r (
	0x23e24d87, 0x7832, 0x4329, 0xa8, 0x6f, 0x8e, 0x86, 0xf7, 0x29, 0xe, 0xea
    );

    // {F5C732BF-FA88-4f4a-8ED7-6A314B40F468}
    VTypeInfoHolderFor<V::VBlob const&> const VTypeInfoHolderFor_V_VBlob_const_r (
	0xf5c732bf, 0xfa88, 0x4f4a, 0x8e, 0xd7, 0x6a, 0x31, 0x4b, 0x40, 0xf4, 0x68
    );

    // {BB3B81C8-FD00-4d46-97E8-B68FA0BB9AC5}
    VTypeInfoHolderFor<VString const&> const VTypeInfoHolderFor_VString_const_r (
	0xbb3b81c8, 0xfd00, 0x4d46, 0x97, 0xe8, 0xb6, 0x8f, 0xa0, 0xbb, 0x9a, 0xc5
    );

    // {484688A5-0571-4cf5-9550-F7CBB0E71805}
    VTypeInfoHolderFor<V::VTime const&> const VTypeInfoHolderFor_V_VTime_const_r (
	0x484688a5, 0x571, 0x4cf5, 0x95, 0x50, 0xf7, 0xcb, 0xb0, 0xe7, 0x18, 0x5
    );

    // {AD58FA42-D536-4369-A4F1-B0EC3201001A}
    VTypeInfoHolderFor<VcaPeerCharacteristics const&> const
    VTypeInfoHolderFor_VcaPeerCharacteristics_const_r (
	0xad58fa42, 0xd536, 0x4369, 0xa4, 0xf1, 0xb0, 0xec, 0x32, 0x1, 0x0, 0x1a
    );

    // {E3B46903-1F93-4129-8A9B-4F221543EDF7}
    VTypeInfoHolderFor<VPeerData const&> const
    VTypeInfoHolderFor_VPeerData_const_r (
	0xe3b46903, 0x1f93, 0x4129, 0x8a, 0x9b, 0x4f, 0x22, 0x15, 0x43, 0xed, 0xf7
    );

    // {7C3E80CE-2307-4b40-BC79-1AB957C137FE}
    VTypeInfoHolderFor<VPeerDataArray const&> const
    VTypeInfoHolderFor_VPeerDataArray_const_r (
	0x7c3e80ce, 0x2307, 0x4b40, 0xbc, 0x79, 0x1a, 0xb9, 0x57, 0xc1, 0x37, 0xfe
    );

    // {9813D7CB-F3A7-4566-8428-6A09F4037E01}
    VTypeInfoHolderFor<VcaSSID const&> const VTypeInfoHolderFor_VcaSSID_const_r (
	0x9813d7cb, 0xf3a7, 0x4566, 0x84, 0x28, 0x6a, 0x9, 0xf4, 0x3, 0x7e, 0x1
    );

    // {CBF61C5A-506F-4cf6-851A-56E47D511DBF}
    VTypeInfoHolderFor<VkDynamicArrayOf<bool> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_bool_const_r (
	0xcbf61c5a, 0x506f, 0x4cf6, 0x85, 0x1a, 0x56, 0xe4, 0x7d, 0x51, 0x1d, 0xbf
    );

    // {10FA4AC6-AA30-4cbf-B74E-2FDB70BCD659}
    VTypeInfoHolderFor<VkDynamicArrayOf<F32> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_F32_const_r (
	0x10fa4ac6, 0xaa30, 0x4cbf, 0xb7, 0x4e, 0x2f, 0xdb, 0x70, 0xbc, 0xd6, 0x59
    );

    // {E6E713D6-88CD-47a3-98DC-0AA6C6F6B01F}
    VTypeInfoHolderFor<VkDynamicArrayOf<F64> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_F64_const_r (
	0xe6e713d6, 0x88cd, 0x47a3, 0x98, 0xdc, 0xa, 0xa6, 0xc6, 0xf6, 0xb0, 0x1f
    );

    // {0A728DC3-08E3-470c-AA8C-5C57E84E685C}
    VTypeInfoHolderFor<VkDynamicArrayOf<S08> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_S08_const_r (
	0xa728dc3, 0x8e3, 0x470c, 0xaa, 0x8c, 0x5c, 0x57, 0xe8, 0x4e, 0x68, 0x5c
    );

    // {EB3125D3-2847-4632-BC97-759BC67FDB55}
    VTypeInfoHolderFor<VkDynamicArrayOf<S16> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_S16_const_r (
	0xeb3125d3, 0x2847, 0x4632, 0xbc, 0x97, 0x75, 0x9b, 0xc6, 0x7f, 0xdb, 0x55
    );

    // {AA642A32-468E-4cd2-B0B1-AAB9DE366D70}
    VTypeInfoHolderFor<VkDynamicArrayOf<S32> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_S32_const_r (
	0xaa642a32, 0x468e, 0x4cd2, 0xb0, 0xb1, 0xaa, 0xb9, 0xde, 0x36, 0x6d, 0x70
    );

    // {1AB18E2C-21CA-4bd8-8922-088B8060ECD3}
    VTypeInfoHolderFor<VkDynamicArrayOf<__int64> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf___int64_const_r (
	0x1ab18e2c, 0x21ca, 0x4bd8, 0x89, 0x22, 0x8, 0x8b, 0x80, 0x60, 0xec, 0xd3
    );

    // {6263A2EA-C9D4-4147-96CA-D239AB48534D}
    VTypeInfoHolderFor<VkDynamicArrayOf<U08> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_U08_const_r (
	0x6263a2ea, 0xc9d4, 0x4147, 0x96, 0xca, 0xd2, 0x39, 0xab, 0x48, 0x53, 0x4d
    );

    // {A70EAA07-76DB-4f1c-9777-3580157CF438}
    VTypeInfoHolderFor<VkDynamicArrayOf<U16> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_U16_const_r (
	0xa70eaa07, 0x76db, 0x4f1c, 0x97, 0x77, 0x35, 0x80, 0x15, 0x7c, 0xf4, 0x38
    );

    // {8F195C1C-9915-47d7-B4F2-FF2B074EF753}
    VTypeInfoHolderFor<VkDynamicArrayOf<U32> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_U32_const_r (
	0x8f195c1c, 0x9915, 0x47d7, 0xb4, 0xf2, 0xff, 0x2b, 0x7, 0x4e, 0xf7, 0x53
    );

    // {7F69115A-1CFF-4c5b-B161-BD6D78D780AF}
    VTypeInfoHolderFor<VkDynamicArrayOf<unsigned __int64> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_unsigned___int64_const_r (
	0x7f69115a, 0x1cff, 0x4c5b, 0xb1, 0x61, 0xbd, 0x6d, 0x78, 0xd7, 0x80, 0xaf
    );

    // {7964BB46-58CB-41d4-B02E-CCC69DF04171}
    VTypeInfoHolderFor<VkDynamicArrayOf<uuid_t> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_uuid_t_const_r (
	0x7964bb46, 0x58cb, 0x41d4, 0xb0, 0x2e, 0xcc, 0xc6, 0x9d, 0xf0, 0x41, 0x71
    );

    // {3C4CDC39-0D0C-43af-A9C8-6213FB1AA909}
    VTypeInfoHolderFor<VkDynamicArrayOf<V::VBlob> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_V_VBlob_const_r (
	0x3c4cdc39, 0xd0c, 0x43af, 0xa9, 0xc8, 0x62, 0x13, 0xfb, 0x1a, 0xa9, 0x9
    );

    // {820828C1-D569-493f-BFBC-8EF4DB1AD4E1}
    VTypeInfoHolderFor<VkDynamicArrayOf<VString> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_VString_const_r (
	0x820828c1, 0xd569, 0x493f, 0xbf, 0xbc, 0x8e, 0xf4, 0xdb, 0x1a, 0xd4, 0xe1
    );

    // {69110606-0DC5-4118-9C97-E3921E7A2F35}
    VTypeInfoHolderFor<VkDynamicArrayOf<V::VTime> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_V_VTime_const_r (
	0x69110606, 0xdc5, 0x4118, 0x9c, 0x97, 0xe3, 0x92, 0x1e, 0x7a, 0x2f, 0x35
    );

    // {4CE1B134-E2F5-407b-9E41-02AF0AFD0FB4}
    VTypeInfoHolderFor<VkDynamicArrayOf<IVUnknown::Reference> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_IVUnknown_Reference__const_r (
	0x4ce1b134, 0xe2f5, 0x407b, 0x9e, 0x41, 0x2, 0xaf, 0xa, 0xfd, 0xf, 0xb4
    );
    

    /*-----------------------------------*
     *----  Serializable References  ----*
     *-----------------------------------*/

    // {513B46F0-24DB-49c9-A3B7-7802596AE018}
    VTypeInfoHolderForReferenceable<VBSConsumer> const VTypeInfoHolderForReferenceable_Vca_VBSConsumer (
	0x513b46f0, 0x24db, 0x49c9, 0xa3, 0xb7, 0x78, 0x2, 0x59, 0x6a, 0xe0, 0x18
    );

    // {CFA1E1C0-D294-4300-9C20-A9EBE5D05145}
    VTypeInfoHolderForReferenceable<VBSProducer> const VTypeInfoHolderForReferenceable_Vca_VBSProducer (
	0xcfa1e1c0, 0xd294, 0x4300, 0x9c, 0x20, 0xa9, 0xeb, 0xe5, 0xd0, 0x51, 0x45
    );

    // {6E46A864-8E2A-49e6-B1A8-5BE1E5F331F0}
    VTypeInfoHolderForReferenceable<VcaSite> const VTypeInfoHolderForReferenceable_VcaSite (
	0x6e46a864, 0x8e2a, 0x49e6, 0xb1, 0xa8, 0x5b, 0xe1, 0xe5, 0xf3, 0x31, 0xf0
    );

    // {7A4740FF-2E2B-4DB4-8571-4A0717C94F7B}
    VTypeInfoHolderForReferenceable<VTypeInfo> const VTypeInfoHolderForReferenceable_VTypeInfo (
	0x7a4740ff, 0x2e2b, 0x4db4, 0x85, 0x71, 0x4a, 0x7, 0x17, 0xc9, 0x4f, 0x7b
    );
    

    /*------------------------------*
     *----  Serializable Types  ----*
     *------------------------------*/

    // {D4BB1BEF-54D2-4958-A22E-D59A62F90D16}
    VTypeInfoHolderForSerializable<bool> const VTypeInfoHolderForSerializable_bool (
	0xD4BB1BEF, 0x54D2, 0x4958, 0xA2, 0x2E, 0xD5, 0x9A, 0x62, 0xF9, 0x0D, 0x16
    );

    // {BA59E716-DA7C-4bc4-A76C-FBA017CE2197}
    VTypeInfoHolderForSerializable<F32> const
    VTypeInfoHolderForSerializable_ieee_float32 (
	0xba59e716, 0xda7c, 0x4bc4, 0xa7, 0x6c, 0xfb, 0xa0, 0x17, 0xce, 0x21, 0x97
    );

    // {D94E0579-1D51-40bc-BF20-00B6AC5550BC}
    VTypeInfoHolderForSerializable<F64> const VTypeInfoHolderForSerializable_ieee_float64 (
	0xd94e0579, 0x1d51, 0x40bc, 0xbf, 0x20, 0x0, 0xb6, 0xac, 0x55, 0x50, 0xbc
    );

    // {EC92636A-0E09-47a5-9AAD-C95B3069336A}
    VTypeInfoHolderForSerializable<char> const VTypeInfoHolderForSerializable___int8 (
	0xec92636a, 0xe09, 0x47a5, 0x9a, 0xad, 0xc9, 0x5b, 0x30, 0x69, 0x33, 0x6a
    );

    // {790D1C2B-006D-4200-8AE3-DAC5F243359A}
    VTypeInfoHolderForSerializable<short> const VTypeInfoHolderForSerializable___int16 (
	0x790d1c2b, 0x6d, 0x4200, 0x8a, 0xe3, 0xda, 0xc5, 0xf2, 0x43, 0x35, 0x9a
    );

    // {14A1090D-B99B-46FB-903E-AEB68C655A6D}
    VTypeInfoHolderForSerializable<int> const VTypeInfoHolderForSerializable___int32 (
	0x14A1090D, 0xB99B, 0x46fb, 0x90, 0x3E, 0xAE, 0xB6, 0x8C, 0x65, 0x5A, 0x6D
    );

    // {61589950-AD80-410b-99D6-2AE7492A7A0C}
    VTypeInfoHolderForSerializable<__int64> const VTypeInfoHolderForSerializable___int64 (
	0x61589950, 0xad80, 0x410b, 0x99, 0xd6, 0x2a, 0xe7, 0x49, 0x2a, 0x7a, 0xc
    );

    // {910EBCF1-26F8-4ff2-990C-2B5D81F0CDE8}
    VTypeInfoHolderForSerializable<unsigned char> const VTypeInfoHolderForSerializable_unsigned___int8 (
	0x910ebcf1, 0x26f8, 0x4ff2, 0x99, 0xc, 0x2b, 0x5d, 0x81, 0xf0, 0xcd, 0xe8
    );

    // {BA54E0A4-3602-42f9-A6BC-BE348E9DC564}
    VTypeInfoHolderForSerializable<unsigned short> const VTypeInfoHolderForSerializable_unsigned___int16 (
	0xba54e0a4, 0x3602, 0x42f9, 0xa6, 0xbc, 0xbe, 0x34, 0x8e, 0x9d, 0xc5, 0x64
    );

    // {DDAE3A3C-8E6C-4B80-BA3A-89C36A389137}
    VTypeInfoHolderForSerializable<unsigned int> const VTypeInfoHolderForSerializable_unsigned___int32 (
	0xDDAE3A3C, 0x8E6C, 0x4b80, 0xBA, 0x3A, 0x89, 0xC3, 0x6A, 0x38, 0x91, 0x37
    );

    // {1EC7A31A-736B-4e1f-9325-94F1D7F4CD08}
    VTypeInfoHolderForSerializable<unsigned __int64> const VTypeInfoHolderForSerializable_unsigned___int64 (
	0x1ec7a31a, 0x736b, 0x4e1f, 0x93, 0x25, 0x94, 0xf1, 0xd7, 0xf4, 0xcd, 0x8
    );

    // {E3E81B5A-9207-4962-AFD6-332AFAE62D3A}
    VTypeInfoHolderForSerializable<uuid_t> const VTypeInfoHolderForSerializable_uuid_t (
	0xe3e81b5a, 0x9207, 0x4962, 0xaf, 0xd6, 0x33, 0x2a, 0xfa, 0xe6, 0x2d, 0x3a
    );

    // {33C255B2-692A-4bf3-B7E4-87DE229F4401}
    VTypeInfoHolderForSerializable<V::VBlob> const VTypeInfoHolderForSerializable_V_VBlob (
	0x33c255b2, 0x692a, 0x4bf3, 0xb7, 0xe4, 0x87, 0xde, 0x22, 0x9f, 0x44, 0x1
    );

    // {6D51A586-2802-4168-981E-975BED3300DB}
    VTypeInfoHolderForSerializable<VString> const VTypeInfoHolderForSerializable_VString (
	0x6D51A586, 0x2802, 0x4168, 0x98, 0x1E, 0x97, 0x5B, 0xED, 0x33, 0x00, 0xDB
    );

    // {D4DDC371-715C-4def-AB1C-CFEE4D50C348}
    VTypeInfoHolderForSerializable<V::VTime> const VTypeInfoHolderForSerializable_V_VTime (
	0xd4ddc371, 0x715c, 0x4def, 0xab, 0x1c, 0xcf, 0xee, 0x4d, 0x50, 0xc3, 0x48
    );

    // {4BF8CB65-DF86-4b06-A7CE-CB143879E519}
    VTypeInfoHolderForSerializable<VInterfaceMember const*> const
    VTypeInfoHolderForSerializable_VInterfaceMember_const_p (
	0x4bf8cb65, 0xdf86, 0x4b06, 0xa7, 0xce, 0xcb, 0x14, 0x38, 0x79, 0xe5, 0x19
    );

    // {79562B0F-E03E-47e2-AC1C-54E4BEAF3486}
    VTypeInfoHolderForSerializable<VcaPeerCharacteristics> const
    VTypeInfoHolderForSerializable_VcaPeerCharacteristics (
	0x79562b0f, 0xe03e, 0x47e2, 0xac, 0x1c, 0x54, 0xe4, 0xbe, 0xaf, 0x34, 0x86
    );

    // {57FA98BB-3DC0-421b-949F-433C256FBEB4}
    VTypeInfoHolderForSerializable<VcaRouteStatistics> const
    VTypeInfoHolderForSerializable_VcaRouteStatistics (
	0x57fa98bb, 0x3dc0, 0x421b, 0x94, 0x9f, 0x43, 0x3c, 0x25, 0x6f, 0xbe, 0xb4
    );

    // {EC0601AD-ED12-4f0c-826A-A89D7F836168}
    VTypeInfoHolderForSerializable<VPeerData> const
    VTypeInfoHolderForSerializable_Vca_VPeerData (
	0xec0601ad, 0xed12, 0x4f0c, 0x82, 0x6a, 0xa8, 0x9d, 0x7f, 0x83, 0x61, 0x68
    );

    // {FA38E686-352A-44be-8610-216B3F9CC224}
    VTypeInfoHolderForSerializable<VPeerDataArray> const
    VTypeInfoHolderForSerializable_Vca_VPeerDataArray (
	0xfa38e686, 0x352a, 0x44be, 0x86, 0x10, 0x21, 0x6b, 0x3f, 0x9c, 0xc2, 0x24
    );

    // {681A7464-2047-480b-81EE-BD092D2609E9}
    VTypeInfoHolderForSerializable<VcaSSID> const
    VTypeInfoHolderForSerializable_VcaSSID (
	0x681a7464, 0x2047, 0x480b, 0x81, 0xee, 0xbd, 0x9, 0x2d, 0x26, 0x9, 0xe9
    );
    

    /*------------------------------------*
     *----  Serializable Array Types  ----*
     *------------------------------------*/

    // {DE55388A-1008-4004-A2BF-3B88EEFA5E92}
    VTypeInfoHolderForArrayOf<bool> const
    VTypeInfoHolderForSerializableArrayOf_bool (
	0xde55388a, 0x1008, 0x4004, 0xa2, 0xbf, 0x3b, 0x88, 0xee, 0xfa, 0x5e, 0x92
    );

    VTypeInfoHolderForArrayOf<F32> const
    VTypeInfoHolderForSerializableArrayOf_F32 (
	0x8c05b885, 0x29f8, 0x4aea, 0x9c, 0xa, 0xcf, 0x27, 0xa7, 0x95, 0x10, 0x90
    );

    // {70345E61-6755-470b-A597-FEAFEFBE53BA}
    VTypeInfoHolderForArrayOf<F64> const
    VTypeInfoHolderForSerializableArrayOf_F64(
	0x70345e61, 0x6755, 0x470b, 0xa5, 0x97, 0xfe, 0xaf, 0xef, 0xbe, 0x53, 0xba
    );

    // {74416F81-7432-4797-8AFC-15FE008BED75}
    VTypeInfoHolderForArrayOf<S08> const
    VTypeInfoHolderForSerializableArrayOf_S08 (
	0x74416f81, 0x7432, 0x4797, 0x8a, 0xfc, 0x15, 0xfe, 0x0, 0x8b, 0xed, 0x75
    );

    // {6429A9EC-CC71-4921-B1A9-0DADCA79B49B}
    VTypeInfoHolderForArrayOf<S16> const
    VTypeInfoHolderForSerializableArrayOf_S16 (
	0x6429a9ec, 0xcc71, 0x4921, 0xb1, 0xa9, 0xd, 0xad, 0xca, 0x79, 0xb4, 0x9b
    );

    // {7F5F5676-C3A2-4d6b-9DE4-7A9425A5B1E7}
    VTypeInfoHolderForArrayOf<S32> const
    VTypeInfoHolderForSerializableArrayOf_S32 (
	0x7f5f5676, 0xc3a2, 0x4d6b, 0x9d, 0xe4, 0x7a, 0x94, 0x25, 0xa5, 0xb1, 0xe7
    );

    // {B911575C-6D38-49be-8E8A-6081893DB387}
    VTypeInfoHolderForArrayOf<__int64> const
    VTypeInfoHolderForSerializableArrayOf___int64 (
	0xb911575c, 0x6d38, 0x49be, 0x8e, 0x8a, 0x60, 0x81, 0x89, 0x3d, 0xb3, 0x87
    );

    // {8838868E-BB2D-4113-B8DB-FA436F59DC72}
    VTypeInfoHolderForArrayOf<U08> const
    VTypeInfoHolderForSerializableArrayOf_U08 (
	0x8838868e, 0xbb2d, 0x4113, 0xb8, 0xdb, 0xfa, 0x43, 0x6f, 0x59, 0xdc, 0x72
    );

    // {8838868E-BB2D-4113-B8DB-FA436F59DC72}
    VTypeInfoHolderForArrayOf<U16> const
    VTypeInfoHolderForSerializableArrayOf_U16 (
	0x8838868e, 0xbb2d, 0x4113, 0xb8, 0xdb, 0xfa, 0x43, 0x6f, 0x59, 0xdc, 0x72
    );

    // {52842A26-55A3-4b17-AE52-EB8852314280}
    VTypeInfoHolderForArrayOf<U32> const
    VTypeInfoHolderForSerializableArrayOf_U32 (
	0x52842a26, 0x55a3, 0x4b17, 0xae, 0x52, 0xeb, 0x88, 0x52, 0x31, 0x42, 0x80
    );

    // {7B64BFE3-13BF-4b44-BA33-BE1E6CEFA7B3}
    VTypeInfoHolderForArrayOf<unsigned __int64> const
    VTypeInfoHolderForSerializableArrayOf_unsigned___int64 (
	0x7b64bfe3, 0x13bf, 0x4b44, 0xba, 0x33, 0xbe, 0x1e, 0x6c, 0xef, 0xa7, 0xb3
    );

    // {C3A20860-1609-42eb-BDA0-A66060340139}
    VTypeInfoHolderForArrayOf<uuid_t> const
    VTypeInfoHolderForSerializableArrayOf_uuid_t (
	0xc3a20860, 0x1609, 0x42eb, 0xbd, 0xa0, 0xa6, 0x60, 0x60, 0x34, 0x1, 0x39
    );

    // {95A25D20-C685-474b-8900-0686F94B31B1}
    VTypeInfoHolderForArrayOf<V::VBlob> const
    VTypeInfoHolderForSerializableArrayOf_V_VBlob (
	0x95a25d20, 0xc685, 0x474b, 0x89, 0x0, 0x6, 0x86, 0xf9, 0x4b, 0x31, 0xb1
    );

    // {7DE640C4-AB2A-46ff-B79F-1C4507CF924D}
    VTypeInfoHolderForArrayOf<VString> const
    VTypeInfoHolderForSerializableArrayOf_VString (
	0x7de640c4, 0xab2a, 0x46ff, 0xb7, 0x9f, 0x1c, 0x45, 0x7, 0xcf, 0x92, 0x4d
    );

    // {202BC302-CAE5-4313-AF66-B9B06CC7EF17}
    VTypeInfoHolderForArrayOf<V::VTime> const
    VTypeInfoHolderForSerializableArrayOf_V_VTime (
	0x202bc302, 0xcae5, 0x4313, 0xaf, 0x66, 0xb9, 0xb0, 0x6c, 0xc7, 0xef, 0x17
    );

    // {BF2BF402-57F2-497e-B738-526C82F83584}
    VTypeInfoHolderForArrayOf<IVUnknown::Reference> const
    VTypeInfoHolderForSerializableArrayOf_IVUnknown__Reference (
	0xbf2bf402, 0x57f2, 0x497e, 0xb7, 0x38, 0x52, 0x6c, 0x82, 0xf8, 0x35, 0x84
    );
}
