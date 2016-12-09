/*****  Shared Error Code Definitions  *****/
#ifndef ERRDEF_D
#define ERRDEF_D

/*---------------------------------------------------------------------------
 * The following enumerated type definition defines the set of error codes
 * which can be signalled.  Error codes are organized into a hierarchy by
 * their more detailed definition in 'errdef.i'.  Two error codes are of
 * special significance - EC__AnError, the 'super-code' of all errors,
 * and EC__MaxError, the last error code defined in this enumerated type.
 *---------------------------------------------------------------------------
 */
enum ERRDEF_ErrorCode {

EC__AnError,
    EC__SystemFault,
	EC__AnUnknownError,
	EC__UnimplementedCase,

	EC__ErrorHandlerError,
	    EC__InvalidErrorCode,
	    EC__HandlerStackEmpty,
	    EC__CantReturn,

	EC__IObjectError,
	    EC__IObjectMethodNotFound,

	EC__FileError,
	    EC__FileStatusAccessError,

	EC__MemoryError,
	    EC__SegmentationFault,
	    EC__PrivateMemoryError,
		EC__MallocError,
		EC__ReallocError,
		EC__FreeError,
	    EC__SharedMemoryError,
		EC__FileMappingError,
	    EC__MError,
		EC__ContainerFramingError,
		EC__MemoryManagerLimit,

	EC__RTypeFault,
	    EC__UnknownRType,
	    EC__UnknownRTypeHandlerOp,
	    EC__RTypeHandlerFault,

	EC__VirtualMachineError,

	EC__InternalInconsistency,

	EC__ExternalInterfaceError,

    EC__UsageError,

    EC__UnknownUVectorRType, 

    EC__AnInterrupt,
	EC__HangupSignal,
	EC__InterruptSignal,
	EC__TerminateSignal,
	EC__QuitSignal,
	EC__FPESignal,
	EC__UserSignal1,
	EC__UserSignal2,
	EC__AlarmSignal,
	EC__VTAlarmSignal,
	EC__PTAlarmSignal,

    EC__MaxError	/*  Note that this code must be LAST in the typedef  */

};


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  errdef.d 1 replace /users/mjc/system
  860226 16:18:41 mjc create

 ************************************************************************/
/************************************************************************
  errdef.d 2 replace /users/mjc/system
  860403 11:22:42 mjc Updated memory, virtual machine, and interrupt error codes

 ************************************************************************/
/************************************************************************
  errdef.d 3 replace /users/mjc/system
  860428 07:49:14 mjc Added 'InternalInconsistency' error

 ************************************************************************/
/************************************************************************
  errdef.d 4 replace /users/mjc/makework
  860503 16:38:58 mjc Added hangup and alarm signal error codes

 ************************************************************************/
/************************************************************************
  errdef.d 5 replace /users/jad/system
  860914 11:15:04 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  errdef.d 6 replace /users/mjc/translation
  870524 09:38:07 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
