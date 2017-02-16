/*****  Representation Type Definition Facility Exported Declarations  *****/
#ifndef RTYPE_H
#define RTYPE_H

/*********************************
 *********************************
 *****  Imported Interfaces  *****
 *********************************
 *********************************/

/******************
 *****  Self  *****
 ******************/

#include "rtype.d"

/************************
 *****  Supporting  *****
 ************************/

#include "viobj.h"

class M_CPD;

class rtLINK_CType;

#include "V_VArgList.h"


/*********************************************************
 *********************************************************
 *****  The Representation Type Handler Request Set  *****
 *********************************************************
 *********************************************************/
/*---------------------------------------------------------------------------
 * The following enumerated type defines the set of operations which a
 * representation type handler is expected to provide.  These operations are
 * accessed via the routine 'RTYPE_PerformHandlerOperation'.  The following
 * synopsis defines the parameters which must be passed to the handler routine
 * for each of these operations:
 *
 * RTYPE_InitializeMData:
 *	The address of a 'M_RTypeDescription' structure which is to be
 *	initialized in a manner appropriate for the type.
 *
 * RTYPE_InstanceMD:
 *	The address of an 'IOBJ_MD' which will be set to
 *	point to a method dictionary defining operations on an instance of
 *	this representation type.  The methods defined in this dictionary
 *	should expect 'self' to be an 'BROWSER_IType_QRReferent'.
 *
 * RTYPE_TypeMD:
 *	The address of an 'IOBJ_MD' which will be set to
 *	point to a method dictionary defining operations on this representation
 *	type.  This method dictionary is equivalent to the one associated with
 *	a meta class in Smalltalk terms.  The methods defined in this
 *	dictionary should expect 'self' to be an 'RTYPE_IType'.
 *
 * RTYPE_QPrintObject:
 *	The address of a standard CPD for the object to be printed.  'Quick'
 *	print format is expected to be a brief display of the object.  Most
 *	handlers will probably make this service call 'M_PrintObject' which
 *	will produce a standard, brief display of the object's type and
 *	address.
 *
 * RTYPE_PrintObject:
 *	The address of a standard CPD for the object to be printed.  'Print'
 *	format is expected to generate a full, generally non-recursive display
 *	of the object.
 *
 * RTYPE_RPrintObject:
 *	The address of a standard CPD for the object to be printed.
 *	'Recursive' print format is expected to display the object in all its
 *	gory detail.
 *
 * RType_DoLCExtract:
 *	The address of a CPD address, a CPD address, and a link constructor
 *	or CPD address identifying respectively the return address for the
 *	result CPD, the source, and subscript of the extract operation to be
 *	performed.
 *
 * A handler is expected to return 0 (zero) if it successfully processed the
 * request and -1 otherwise.
 *---------------------------------------------------------------------------
 */

enum RTYPE_HandlerOperation {
    RTYPE_InitializeMData,
    RTYPE_InstanceMD,
    RTYPE_TypeMD,
    RTYPE_QPrintObject,
    RTYPE_PrintObject,
    RTYPE_RPrintObject,
    RTYPE_DoLCExtract
};


/****************************************************************************
 ****************************************************************************
 *****  Representation Type Handler Definition and Reference Utilities  *****
 ****************************************************************************
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Macro to define the header for a representation type handler function.
 * This macro defines a function of two arguments: 'handlerOperation', the
 * handler operation desired, and 'ap', a 'varargs' argument pointer set to
 * point to the first of a series of arguments to the handler.
 *---------------------------------------------------------------------------
 */
#define RTYPE_DefineHandler(rtype) int rtype (\
    RTYPE_HandlerOperation handlerOperation, V::VArgList iArgList\
)

/*---------------------------------------------------------------------------
 *****  Representation Type Handler Reference Type
 *---------------------------------------------------------------------------
 */
typedef int (*Ref_RTYPE_Handler) (
    RTYPE_HandlerOperation handlerOperation, V::VArgList iArgList
);


/*************************************
 *************************************
 *****  Q-Register Declarations  *****
 *************************************
 *************************************/

/*****  Q-Registers  *****/
enum RTYPE_QRegisterId {
    RTYPE_qa, RTYPE_qb, RTYPE_qc, RTYPE_qd, RTYPE_qe, RTYPE_qf, RTYPE_qg,
    RTYPE_qh, RTYPE_qi, RTYPE_qj, RTYPE_qk, RTYPE_ql, RTYPE_qm, RTYPE_qn,
    RTYPE_qo, RTYPE_qp, RTYPE_qq, RTYPE_qr, RTYPE_qs, RTYPE_qt, RTYPE_qu,
    RTYPE_qv, RTYPE_qw, RTYPE_qx, RTYPE_qy, RTYPE_qz,

    RTYPE_tq0, RTYPE_tq1, RTYPE_tq2, RTYPE_tq3, RTYPE_tq4,
    RTYPE_tq5, RTYPE_tq6, RTYPE_tq7, RTYPE_tq8, RTYPE_tq9,

    RTYPE_QMAX  /*  This must always be the last entry  */
};

#define RTYPE_TotalQRegisterCount\
    static_cast<unsigned int>(RTYPE_QMAX)

#define RTYPE_PermanentQRegisterCount\
    static_cast<unsigned int>(RTYPE_tq0)

#define RTYPE_TemporaryQRegisterCount\
    (RTYPE_TotalQRegisterCount - RTYPE_PermanentQRegisterCount)


/********************************
 ********************************
 *****  Callable Interface  *****
 ********************************
 ********************************/

PublicFnDecl int __cdecl RTYPE_PerformHandlerOperation (
    RTYPE_HandlerOperation	handlerOp,
    RTYPE_Type			type,
    ...
);

PublicFnDecl char const *RTYPE_TypeIdAsString (
    RTYPE_Type			r
);

PublicFnDecl void RTYPE_QPrint (
    M_CPD*			cpd,
    int				whichPointer
);

PublicFnDecl void RTYPE_Print (
    M_CPD*			cpd,
    int				whichPointer
);

PublicFnDecl void RTYPE_RPrint (
    M_CPD*			cpd,
    int				whichPointer
);

/*****  General Q-Register Support  *****/
PublicFnDecl IOBJ_IObject RTYPE_QRegister (M_CPD *cpd);
PublicFnDecl IOBJ_IObject RTYPE_QRegister (rtLINK_CType *lc);

PublicFnDecl IOBJ_IObject RTYPE_Browser (
    M_CPD*			existingCPD,
    int				whichPointer
);

/*****  Debugger Q-Register Support  *****/
PublicFnDecl M_CPD* RTYPE_QRegisterCPD (
    IOBJ_IObject		iObject
);

PublicFnDecl IOBJ_IObject RTYPE_QRegisterIObject (
    RTYPE_QRegisterId		qRegisterIndex
);

PublicFnDecl char const *RTYPE_QRegisterName (
    RTYPE_QRegisterId		qRegisterIndex
);

IOBJ_DeclarePublicMethod (RTYPE_DisplayTypeIObject);
IOBJ_DeclareIType (RTYPE_IType);

/*****  Representation Type I-Object Support  *****/
PublicFnDecl IOBJ_IObject RTYPE_TypeIObject (
    RTYPE_Type			rtype
);

/*****  Standard Representation Type Instance Methods  *****/
IOBJ_DeclarePublicMethod (RTYPE_QPrintIObject);
IOBJ_DeclarePublicMethod (RTYPE_PrintIObject);
IOBJ_DeclarePublicMethod (RTYPE_RPrintIObject);

#define RTYPE_StandardDMDEPackage\
    IOBJ_MDE ("qprint"		, RTYPE_QPrintIObject)\
    IOBJ_MDE ("print"		, RTYPE_PrintIObject)\
    IOBJ_MDE ("rprint"		, RTYPE_RPrintIObject)

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  rtype.h 1 replace /users/mjc/system
  860226 16:24:53 mjc create

 ************************************************************************/
/************************************************************************
  rtype.h 2 replace /users/mjc/system
  860321 17:59:14 mjc Release 1 of new R-Type architecture

 ************************************************************************/
/************************************************************************
  rtype.h 3 replace /users/mjc/system
  860323 17:22:07 mjc Release of additional work on new memory architecture

 ************************************************************************/
/************************************************************************
  rtype.h 4 replace /users/mjc/system
  860410 19:18:34 mjc Added standard print services and 'RPrint'

 ************************************************************************/
/************************************************************************
  rtype.h 5 replace /users/mjc/system
  860414 12:42:26 mjc Eliminate POP Cache

 ************************************************************************/
/************************************************************************
  rtype.h 6 replace /users/mjc/system
  860415 16:22:23 mjc Added 'RTYPE_StandardDMDEPackage'

 ************************************************************************/
/************************************************************************
  rtype.h 7 replace /users/mjc/translation
  870524 09:41:20 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  rtype.h 8 replace /users/jad/system
  880310 18:15:23 jad included iobj.d

 ************************************************************************/
