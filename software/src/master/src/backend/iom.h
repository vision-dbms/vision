/*****  I/O Manager Interface  *****/
#ifndef IOM_Interface
#define IOM_Interface

/**************************
 **************************
 *****  Declarations  *****
 **************************
 **************************/

class M_CPD;

class rtPTOKEN_Handle;
class rtVECTOR_Handle;

template <class R> class VReference;


/*************************
 *************************
 *****  Definitions  *****
 *************************
 *************************/


/***********************
 *****  Constants  *****
 ***********************/

enum IOMChannelType {
    IOMChannelType_Null,
    IOMChannelType_Service,
    IOMChannelType_Stream,
    IOMChannelType_File
};

enum IOMState {
    IOMState_Unused,
    IOMState_Ready,
    IOMState_Starting,
    IOMState_Restarting,
    IOMState_Running,
    IOMState_Closing
};

enum IOMOption {
    IOMOption_None,
    IOMOption_PromptFormat,
    IOMOption_BinaryFormat,
    IOMOption_TrimFormat,
    IOMOption_SeekOffset,
    IOMOption_TCPNODELAY,
    IOMOption_AutoMutex
};

enum IOMPromptFormat {
    IOMPromptFormat_Normal,
    IOMPromptFormat_Editor
};

enum IOMBinaryFormat {
    IOMBinaryFormat_Untranslated,
    IOMBinaryFormat_PARisc,
    IOMBinaryFormat_Sparc,
    IOMBinaryFormat_Intel
};

enum IOMTrimFormat {
    IOMTrimFormat_Untrimmed,
    IOMTrimFormat_TrimLeadingBlanks,
    IOMTrimFormat_TrimTrailingBlanks,
    IOMTrimFormat_TrimBoundingBlanks
};


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

PublicVarDecl VReference<rtVECTOR_Handle> IOM_HandlerVector;
PublicVarDecl VReference<rtPTOKEN_Handle> IOM_HandlerPToken;


/***********************
 ***********************
 *****  Utilities  *****
 ***********************
 ***********************/

/**************************
 *****  Descriptions  *****
 **************************/

PublicFnDecl char const* IOM_StateDescription (
    IOMState			code
);

PublicFnDecl char const* IOM_ChannelTypeDescription (
    IOMChannelType		code
);

PublicFnDecl char const* IOM_OptionDescription (
    IOMOption			code
);

PublicFnDecl char const* IOM_PromptFormatDescription (
    IOMPromptFormat		code
);

PublicFnDecl char const* IOM_BinaryFormatDescription (
    IOMBinaryFormat		code
);

PublicFnDecl char const* IOM_TrimFormatDescription (
    IOMTrimFormat		code
);


/*******************************
 *******************************
 *****  Subsystem Manager  *****
 *******************************
 *******************************/

/*************************
 *****  Table Query  *****
 *************************/

PublicFnDecl size_t IOM_DriverTableSize ();


/********************************
 *****  Event Loop Helpers  *****
 ********************************/

PublicFnDecl void IOM_PutBufferedData ();


#endif
