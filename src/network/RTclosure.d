/*****  Closure Representation Type Shared Declarations  *****/
#ifndef rtCLOSURE_D
#define rtCLOSURE_D

/*************************************
 *****  Realized Closure Format  *****
 *************************************/
/*---------------------------------------------------------------------------
 * A realized closure consists of a POP for the context of the closure and
 * either a POP for a block or the index of a primitive.
 *
 *  Realized Closure Field Descriptions:
 *	context			- a POP for the context associated with this
 *				  closure.
 *	block			- an optional POP for the block closed by this
 *				  closure if this is a block closure.  If this
 *				  is a primitive closure, this POP is 'Nil'.
 *	primitive		- an optional index for the primitive closed by
 *				  this closure if this is a primitive closure.
 *---------------------------------------------------------------------------
 */
struct rtCLOSURE_Closure {
    M_POP			context;
    M_POP			block;
    int				primitive;
};

/*****  Access Macros  *****/
#define rtCLOSURE_Closure_Context(closurep)\
    (closurep)->context

#define rtCLOSURE_Closure_Block(closurep)\
    (closurep)->block

#define rtCLOSURE_Closure_Primitive(closurep)\
    (closurep)->primitive


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  RTclosure.d 1 replace /users/mjc/system
  861002 17:55:43 mjc New method, context, and closure virtual machine support types

 ************************************************************************/
/************************************************************************
  RTclosure.d 2 replace /users/mjc/translation
  870524 09:31:59 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
