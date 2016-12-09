#ifndef M_RTD_Interface
#define M_RTD_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "rtype.h"

/**************************
 *****  Declarations  *****
 **************************/

class	M_ASD;
class	M_CPD;
struct	M_CPreamble;
class	M_CTE;

/*************************
 *****  Definitions  *****
 *************************/

typedef M_CPreamble *(*M_Type_ConvertFn ) (M_ASD*, M_CPreamble const*);

typedef VContainerHandle *(*M_Type_HandleMaker) (M_CTE&);

typedef void (*M_Type_CPDInitFn	) (M_CPD*);
typedef void (*M_Type_ReclaimFn	) (M_ASD*, M_CPreamble const*);
typedef void (*M_Type_MarkFn	) (M_ASD::GCVisitBase*, M_ASD*, M_CPreamble const*);


/*---------------------------------------------------------------------------
 * The following structure is used to describe a representation type to the
 * memory manager.  At system startup, every known representation type handler
 * will be asked to initialize a structure of this type.  The structure
 * presented to the handler will be initialized with a set of default values
 * which the handler is expected to override if appropriate.  The memory
 * manager's interpretation for these fields along with their default values
 * follows:
 *
 *	m_fReuseableCPDs	- a boolean that when true designates that a
 *				  single CPD can be allocated and returned
 *				  for all 'M_Get...CPD' requests. (Default:
 *				  false).
 *	m_iCPDPointerCount	- the base number of pointers to associate
 *				  with a CPD. (Default: 1).
 *	m_iCPDCreateCount	- the total number of CPD's allocated.
 *	m_iCPDDeleteCount	- the total number of CPD's deallocated.
 *	m_pCPDInitFn		- the address of a function which will be
 *				  called to initialize a standard CPD.  This
 *				  function will be called with the address of
 *				  the CPD to be initialized.  (Default: Set
 *				  all pointers to the container base).
 *	m_pReclaimFn		- the address of a function which will be
 *				  called whenever the memory manager is about
 *				  to discard a container of this type.  This
 *				  function will be called with three arguments:
 *				  the container's preamble address, the ASD
 *				  which owns the container, and a boolean (int)
 *				  whose truth designates that the container
 *				  is in the dangling reference count of the
 *				  ASD. (Default: NONE).
 *	m_pMarkFn		- the address of a function which will be
 *				  called to mark all the POPs referenced
 *				  by a container.
 *---------------------------------------------------------------------------
 */


/*****  Type Definition(s)  *****/
class M_RTD {
//  Construction/Initialization
public:
    M_RTD ();

    void construct ();

    void SetCPDPointerCountTo (unsigned int iValue) {
	m_iCPDPointerCount = iValue;
    }
    void SetCPDReusability () {
	m_fReuseableCPDs = true;
    }

//  Access
public:
    unsigned int cpdPointerCount () const {
	return m_iCPDPointerCount;
    }

    bool cpdReuseable () const {
	return m_fReuseableCPDs;
    }

    RTYPE_Type RType () const {
	return (RTYPE_Type)m_xType;
    }
    char const *RTypeName () const {
	return RTYPE_TypeIdAsString (RType ());
    }

//  State
protected:
    RTYPE_Type const		m_xType			: 8;
    unsigned int		m_iCPDPointerCount	: 8;
    unsigned int		m_fReuseableCPDs	: 1;
public:
    unsigned int		m_iCPDCreateCount;
    unsigned int		m_iCPDDeleteCount;
    unsigned int		m_iHandleCreateCount;
    unsigned int		m_iHandleDeleteCount;
    M_Type_HandleMaker		m_pHandleMaker;
    M_Type_CPDInitFn		m_pCPDInitFn;
    M_Type_ReclaimFn		m_pReclaimFn;
    M_Type_MarkFn		m_pMarkFn;
    M_Type_ConvertFn		m_pConvertFn;
};

/***** Access Macros  *****/
#define M_RTD_CPDCreateCount(rtd)	((rtd)->m_iCPDCreateCount)
#define M_RTD_CPDDeleteCount(rtd)	((rtd)->m_iCPDDeleteCount)
#define M_RTD_HandleCreateCount(rtd)	((rtd)->m_iHandleCreateCount)
#define M_RTD_HandleDeleteCount(rtd)	((rtd)->m_iHandleDeleteCount)
#define M_RTD_HandleMaker(rtd)		((rtd)->m_pHandleMaker)
#define M_RTD_CPDInitFn(rtd)		((rtd)->m_pCPDInitFn)
#define M_RTD_ReclaimFn(rtd)		((rtd)->m_pReclaimFn)
#define M_RTD_MarkFn(rtd)		((rtd)->m_pMarkFn)
#define M_RTD_ConvertFn(rtd)		((rtd)->m_pConvertFn)


#endif
