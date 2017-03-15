/*****  Internal Object Type Description Facility  *****/
/*---------------------------------------------------------------------------
 * Theory of Operation:
 *
 * I-Objects are a uniform mechanism for describing internal system objects.
 * Among their uses, I-Objects provide type independent information to the
 * error handler and a means of interrogating and modifying the state of
 * various system objects in the debugger.  Unlike objects created by the
 * object memory manager, I-Objects describe permanent objects in the system.
 * Because they represent permanent objects, I-Objects are never garbage
 * collected.  Because I-Objects are never garbage collected, there is
 * no need to account for their referencers.  As a consequence, I-Object
 * representations can be copied and stored without restriction.
 *---------------------------------------------------------------------------
 */

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName IOBJ

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "m.h"
#include "rtype.h"

#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "RTstring.h"

/*****  Shared  *****/
#include "viobj.h"


/******************************
 ******************************
 *****  Global Variables  *****
 ******************************
 ******************************/

PublicVarDef M_ASD *IOBJ_ScratchPad = 0;

PublicVarDef IOBJ_IObject IOBJ_TheNilIObject; /* added by m2 */


/***************************************
 ***************************************
 *****  I-Object Message Dispatch  *****
 ***************************************
 ***************************************/

/*---------------------------------------------------------------------------
 *****  Routine to send a message to an I-Object.
 *
 *  Arguments:
 *	self			- the recipient of the message.
 *	messageSelector		- the selector of the message to be sent.
 *	parameterIObjectArray	- an array of the I-Objects which are the
 *				  parameters of the message.  The size of
 *				  this array is fully determined by the
 *				  message being sent.
 *
 *  Returns:
 *	The I-Object returned from the method invoked.
 *
 *  Note:
 *	This routine signals the error 'EC__IObjectMethodNotFound'
 *****/
PublicFnDef IOBJ_IObject IOBJ_SendMessage (
    IOBJ_IObject		self,
    IOBJ_MessageSelector	messageSelector,
    IOBJ_IObject		parameterIObjectArray[]
)
{
    IOBJ_MD md;

    for (
	md = (*IOBJ_IObjectIType (self))(self);
	IsntNil (md->messageSelector) ||
	IsntNil (md->method) && IsntNil (
	    md = (*(IOBJ_IType)(md->method))(self)
	);
	md++
    ) if (strcmp (md->messageSelector, messageSelector) == 0)
	    return (*md->method) (self, parameterIObjectArray);

    ERR_SignalWarning (
	EC__IObjectMethodNotFound,
	UTIL_FormatMessage ("Selector: %s", messageSelector)
    );

    return self;
}


/***************************************
 ***************************************
 *****  I-Object Packing Routines  *****
 ***************************************
 ***************************************/

/*---------------------------------------------------------------------------
 *****  Routine to pack and return an I-Object with an 'int' I-Content.
 *
 *  Arguments:
 *	iType			- the I-Type for the new I-Object.
 *	value			- the I-Content value for the new I-Object.
 *
 *  Returns:
 *	The I-Object packed.
 *
 *****/
PublicFnDef IOBJ_IObject IOBJ_PackIntIObject (
    IOBJ_IType iType, int value
) {
    IOBJ_IObject newIObject;

    IOBJ_IObjectIType	    (newIObject) = iType;
    IOBJ_IObjectValueAsInt  (newIObject) = value;

    return newIObject;
}

/*---------------------------------------------------------------------------
 *****  Routine to pack and return an I-Object with an 'double' I-Content.
 *
 *  Arguments:
 *	iType			- the I-Type for the new I-Object.
 *	value			- the I-Content value for the new I-Object.
 *
 *  Returns:
 *	The I-Object packed.
 *
 *****/
PublicFnDef IOBJ_IObject IOBJ_PackDoubleIObject (
    IOBJ_IType			iType,
    double			value
)
{
    IOBJ_IObject newIObject;

    IOBJ_IObjectIType		(newIObject) = iType;
    IOBJ_IObjectValueAsDouble	(newIObject) = value;

    return newIObject;
}


/*---------------------------------------------------------------------------
 *****  Routine to pack and return an I-Object with an 'pointer_t' I-Content.
 *
 *  Arguments:
 *	iType			- the I-Type for the new I-Object.
 *	value			- the I-Content value for the new I-Object.
 *
 *  Returns:
 *	The I-Object packed.
 *
 *****/
PublicFnDef IOBJ_IObject IOBJ_PackAddressIObject (
    IOBJ_IType			iType,
    void *			value
)
{
    IOBJ_IObject newIObject;

    IOBJ_IObjectIType		(newIObject) = iType;
    IOBJ_IObjectValueAsAddress  (newIObject) = value;

    return newIObject;
}


/*---------------------------------------------------------------------------
 *****  Routine to pack and return an I-Object with an 'pointer_t' I-Content.
 *
 *  Arguments:
 *	iType			- the I-Type for the new I-Object.
 *	value			- the I-Content value for the new I-Object.
 *
 *  Returns:
 *	The I-Object packed.
 *
 *****/
PublicFnDef IOBJ_IObject IOBJ_PackFacilityIObject (
    IOBJ_IType			iType,
    FAC_Facility		value
)
{
    IOBJ_IObject newIObject;

    IOBJ_IObjectIType		(newIObject) = iType;
    IOBJ_IObjectValueAsFacility (newIObject) = value;

    return newIObject;
}


/******************************
 *****  Built-In I-Types  *****
 ******************************/
/*---------------------------------------------------------------------------
 *************************  The 'Nil' I-Object Type  ************************
 *---------------------------------------------------------------------------
 */

/****************************
 *  'Nil' I-Object Methods  *
 ****************************/

IOBJ_DefineUnaryMethod (DisplayNil)
{
    IO_printf ("*NIL*");
    return self;
}

/***************************************
 *  'Nil' I-Object Public Definitions  *
 ***************************************/

/*****  I-Type Definition  *****/
IOBJ_BeginIType (IOBJ_IType_Nil)
    IOBJ_MDE ("qprint"		, DisplayNil)
    IOBJ_MDE ("print"		, DisplayNil)
IOBJ_EndIType


/*---------------------------------------------------------------------------
 ************************  The 'Char' I-Object Type  ************************
 *---------------------------------------------------------------------------
 */

/*****************************
 *  'Char' I-Object Methods  *
 *****************************/

IOBJ_DefineUnaryMethod (DisplayChar) {
    int c = (int)IOBJ_IObjectValueAsInt (self);

    IO_printf (isascii (c) && isprint (c) ? "$%c" : "$%02X", c);

    return self;
}

/****************************************
 *  'Char' I-Object Public Definitions  *
 ****************************************/

/*****  I-Type Definition  *****/
IOBJ_BeginIType (IOBJ_IType_Char)
    IOBJ_MDE ("qprint"		, DisplayChar)
    IOBJ_MDE ("print"		, DisplayChar)
IOBJ_EndIType


/*---------------------------------------------------------------------------
 ************************  The 'Int' I-Object Type  *************************
 *---------------------------------------------------------------------------
 */

/****************************
 *  'Int' I-Object Methods  *
 ****************************/

IOBJ_DefineUnaryMethod (DisplayInt)
{
    IO_printf ("%d", IOBJ_IObjectValueAsInt (self));
    return self;
}

/***************************************
 *  'Int' I-Object Public Definitions  *
 ***************************************/

/*****  I-Type Definition  *****/
IOBJ_BeginIType (IOBJ_IType_Int)
    IOBJ_MDE ("qprint"		, DisplayInt)
    IOBJ_MDE ("print"		, DisplayInt)
IOBJ_EndIType


/*---------------------------------------------------------------------------
 ***********************  The 'Double' I-Object Type  ***********************
 *---------------------------------------------------------------------------
 */

/*******************************
 *  'Double' I-Object Methods  *
 *******************************/

IOBJ_DefineUnaryMethod (DisplayDouble)
{
    IO_printf ("%g", IOBJ_IObjectValueAsDouble (self));
    return self;
}

/***************************************
 *  'Double' I-Object Public Definitions  *
 ***************************************/

/*****  I-Type Definition  *****/
IOBJ_BeginIType (IOBJ_IType_Double)
    IOBJ_MDE ("qprint"		, DisplayDouble)
    IOBJ_MDE ("print"		, DisplayDouble)
IOBJ_EndIType


/*---------------------------------------------------------------------------
 ***********************  The 'Switch' I-Object Type  ***********************
 *---------------------------------------------------------------------------
 */

/*******************************
 *  'Switch' I-Object Methods  *
 *******************************/

IOBJ_DefineUnaryMethod (DisplaySwitch)
{
    IO_printf
	(*(bool*)IOBJ_IObjectValueAsAddress (self) ? "*IsOn*" : "*IsOff*");
    return self;
}

IOBJ_DefineUnaryMethod (TurnSwitchOn)
{
    *(bool*)IOBJ_IObjectValueAsAddress (self) = true;
    return self;
}

IOBJ_DefineUnaryMethod (TurnSwitchOff)
{
    *(bool*)IOBJ_IObjectValueAsAddress (self) = false;
    return self;
}

/******************************************
 *  'Switch' I-Object Public Definitions  *
 ******************************************/

/*****  I-Type Definition  *****/
IOBJ_BeginIType (IOBJ_IType_Switch)
    IOBJ_MDE ("qprint"		, DisplaySwitch)
    IOBJ_MDE ("print"		, DisplaySwitch)
    IOBJ_MDE ("on"		, TurnSwitchOn)
    IOBJ_MDE ("off"		, TurnSwitchOff)
IOBJ_EndIType


/****************************************************
 *****  Primitive Universal I-Object 'Methods'  *****
 ****************************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to implement the default display of an I-Object
 *
 *  Arguments:
 *	self			- the recipient of the 'print' or
 *				  'qprint' message.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
IOBJ_DefineUnaryMethod (DefaultIObjectDisplay)
{
    IO_printf (
	"I-Object{%08X:%d}",
	IOBJ_IObjectIType (self),
	IOBJ_IObjectValueAsInt(self)
    );
    return self;
}


/*---------------------------------------------------------------------------
 ***** Routine to send the specified message to self, send a print message
 *     to the result, and return the original self
 *****/
IOBJ_DefineMethod (DefaultSendPrint) {
    M_CPD *messageCPD  = RTYPE_QRegisterCPD (parameterArray[0]);

    IOBJ_IObject orig_self = self;
    RTYPE_MustBeA ("(sendPrint: message)", messageCPD->RType(), RTYPE_C_String);

    rtSTRING_Trim (messageCPD);
    IOBJ_MessageSelector message = (IOBJ_MessageSelector)rtSTRING_CPD_Begin (messageCPD);

    if (message[strlen (message)-1] == ':') ERR_SignalWarning (
	EC__UsageError, "The message must not have any arguments."
    );

    IOBJ_SendMessage (
	IOBJ_SendMessage (
	    self, message, NilOf (IOBJ_IObject *)
	), "print", NilOf (IOBJ_IObject *)
    );

    return orig_self;
}


/*---------------------------------------------------------------------------
 *****  Internal routine to display the messages in a method dictionary
 *
 *****/
IOBJ_DefineUnaryMethod (DisplayMessagesInMD )
{
    IOBJ_MD mdp, md = (*IOBJ_IObjectIType (self))(self);
    int i, maxSymbolLength = 0, symbolLength, symbolsPerLine;

/*****  Determine maximum symbol length  *****/
    for (
	mdp = md;
	IsntNil (mdp->messageSelector) ||
	IsntNil (mdp->method) && IsntNil (
	    mdp = (*(IOBJ_IType)(mdp->method))(self)
	);
	mdp++
    ) if ((symbolLength = strlen (mdp->messageSelector)) > maxSymbolLength)
	maxSymbolLength = symbolLength;

/*****  Display Symbols  *****/
    symbolsPerLine = maxSymbolLength > 79 ? 1 : 79 / (maxSymbolLength + 1);

    IO_printf ("Messages Understood:");
    for (
	mdp = md, i = 0;
	IsntNil (mdp->messageSelector) || (
	    IO_printf ("\n**"),
	    i = 0,
	    IsntNil (mdp->method) &&
	    IsntNil (mdp = (*(IOBJ_IType)(mdp->method))(self))
	);
	mdp++, i++
    ) IO_printf (
	"%c%-*s",
	i % symbolsPerLine ? ' ' : '\n',
	maxSymbolLength,
	mdp->messageSelector
    );

    IO_printf ("\n");
    return self;

}


/********************************************************
 *****  'Basic' I-Object Method Public Definitions  *****
 ********************************************************/

IOBJ_BeginIType (IOBJ_BasicMethod)
    IOBJ_MDE ("print"			, DefaultIObjectDisplay)
    IOBJ_MDE ("qprint"			, DefaultIObjectDisplay)
    IOBJ_MDE ("sendPrint:"		, DefaultSendPrint)
    IOBJ_MDE ("messagesUnderstood"	, DisplayMessagesInMD)
    IOBJ_MDE ("MU"			, DisplayMessagesInMD)
    IOBJ_MDE (
	NilOf (IOBJ_MessageSelector)	, NilOf (IOBJ_Method)
    )
IOBJ_EndIType


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (IOBJ);
    FAC_FDFCase_FacilityDescription ("Internal Object Manager");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  iobj.c 1 replace /users/mjc/system
  860226 16:22:55 mjc create

 ************************************************************************/
/************************************************************************
  iobj.c 2 replace /users/mjc/system
  860322 10:36:04 mjc Changed :quickPrint to :qprint

 ************************************************************************/
/************************************************************************
  iobj.c 3 replace /users/mjc/system
  860327 16:21:46 mjc Removed : from all unary selectors

 ************************************************************************/
/************************************************************************
  iobj.c 4 replace /users/jad
  860421 18:44:18 jad now uses the new io module

 ************************************************************************/
/************************************************************************
  iobj.c 5 replace /users/hjb/system
  860428 19:10:05 hjb added inheritance of Method Dictionaries

 ************************************************************************/
/************************************************************************
  iobj.c 6 replace /users/mjc/system
  860508 18:14:34 mjc Added 'character' I-Object

 ************************************************************************/
/************************************************************************
  iobj.c 7 replace /users/mjc/system
  860509 10:25:36 mjc Changed printers to deal with non-printing characters

 ************************************************************************/
/************************************************************************
  iobj.c 8 replace /users/jad/system
  860509 17:35:30 jad added a synonym for messagesUnderstood: MU

 ************************************************************************/
/************************************************************************
  iobj.c 9 replace /users/jad/system
  860513 15:16:59 jad added a sendPrint message which remembers self,
sends the message to self, sends print to the result, and returns the original
 self

 ************************************************************************/
/************************************************************************
  iobj.c 10 replace /users/mjc/system
  860924 09:44:47 mjc Made switch I-Object methods access the switch as an 'int'

 ************************************************************************/
/************************************************************************
  iobj.c 11 replace /users/mjc/system
  870519 17:48:57 mjc Added omitted '*' in function address dereference

 ************************************************************************/
/************************************************************************
  iobj.c 12 replace /users/mjc/translation
  870524 09:39:11 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  iobj.c 13 replace /users/mjc/system
  870806 23:23:14 mjc Test for 'nil' function address before invocation

 ************************************************************************/
/************************************************************************
  iobj.c 14 replace /users/m2/backend
  890927 15:36:16 m2 dummyiobj added, IOBJ_GlobalNilIObject

 ************************************************************************/
